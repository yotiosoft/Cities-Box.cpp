//
//  CityMap_SaveData.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "CityMap.hpp"

void CityMap::syncToRust() {
    m_rust_core->set_save_version(RELEASE_NUMBER);

    // 基本メタデータの同期
    m_rust_core->set_city_metadata(
        m_city_name.toUTF8(),
        m_mayor_name.toUTF8(),
        m_addon_set_name.toUTF8()
    );

    // 予算の同期
    m_rust_core->set_budget(
        m_budget.police,
        m_budget.fireDepertment,
        m_budget.postOffice,
        m_budget.education
    );

    m_rust_core->set_tax(
        m_tax.residential,
        m_tax.commercial,
        m_tax.office,
        m_tax.industrial,
        m_tax.farm
    );

    // 環境設定の同期
    auto time = getCityTime();
    m_rust_core->set_environment(
        time.year, time.month, time.date, time.hour, time.minutes,
        m_change_weather, m_dark_on_night
    );
}

bool CityMap::save() {
	// 1. C++側の最新状態をRustに送る
    syncToRust();
    
    // 2. Rust側でJSONを生成
    std::vector<rust::citymap::RawTileData> buffer;
    buffer.reserve(m_map_size.x * m_map_size.y);

	for (int y = 0; y < m_map_size.y; ++y) {
        for (int x = 0; x < m_map_size.x; ++x) {
            auto& t = m_tiles[y][x];
            buffer.push_back({
                t.residents,
                t.workers.commercial,
                t.workers.office,
                t.workers.industrial,
                t.workers.farm,
                t.workers.publicFacility,
                t.students,
                t.reservation
            });
        }
    }

	// 3. Rustへ一括転送
    m_rust_core->bulk_set_tiles(
        rust::Slice<const rust::citymap::RawTileData>(buffer.data(), buffer.size()),
        m_map_size.x, 
        m_map_size.y
    );

    // 4. 一括転送に含まれない詳細データの同期
    // *オブジェクトリスト (m_objects) の同期
    m_rust_core->clear_objects();
    for (auto const& [id, object] : m_objects) {
        m_rust_core->add_object(
            id,
            object->getAddonName(NameMode::English).toUTF8(),
            object->getOriginalName().toUTF8(),
            UnitaryTools::typeIDToTypeName(object->getTypeID()).toUTF8(),
            UnitaryTools::directionIDToDirectionName(object->getDirectionID()).toUTF8(),
            object->getOriginCoordinate().x,
            object->getOriginCoordinate().y,
            object->getVisible()
        );
    }

    // 5. 各タイルの動的データ（オブジェクト参照・統計・レート・名前）の同期
    for (int y = 0; y < m_map_size.y; ++y) {
        for (int x = 0; x < m_map_size.x; ++x) {
            auto& t = m_tiles[y][x];
            
            // 地名の同期
            m_rust_core->set_tile_basic(x, y, t.residents, t.students, t.reservation, t.getOriginalName().toUTF8());

            // 統計の同期
            rust::Vec<int32_t> rustAges;
            rust::Vec<rust::String> rustGenders;
            for (const auto& a : t.age) {
                rustAges.push_back(static_cast<int32_t>(a));
            }
            for (const auto& g : t.gender) {
                rustGenders.push_back(g.toUTF8());
            }
            m_rust_core->set_tile_stats(x, y, std::move(rustAges), std::move(rustGenders));

            for (const auto& work_place : t.workPlaces) {
                m_rust_core->add_tile_work_place(
                    x, y,
                    static_cast<int32_t>(work_place.workPlace),
                    work_place.workPlacesSerialNumber
                );
            }

            for (const auto& school : t.schools) {
                m_rust_core->add_tile_school(
                    x, y,
                    static_cast<int32_t>(school.school),
                    school.schoolSerialNumber
                );
            }

            // レートの同期
            for (auto const& [id, value] : t.rate) {
                if (value != 0) {
                    m_rust_core->add_tile_rate(x, y, UnitaryTools::rateIDToRateName(id).toUTF8(), value);
                }
            }

            // オブジェクト参照の同期
            for (auto const& obj_struct : t.getObjectStructs()) {
                if (obj_struct.object_p) { // ポインタチェック
                    m_rust_core->add_tile_object_ref(
                        x, y, 
                        obj_struct.object_p->getObjectID(),
                        obj_struct.relative_coordinate.relative.x,
                        obj_struct.relative_coordinate.relative.y,
                        obj_struct.object_p->getVisible()
                    );
                }
            }
        }
    }

    // 6. 保存実行
    return m_rust_core->save_to_file(m_map_file_path.toUTF8());

	//saveTextFile(map_file_path.toUTF8()+".cbj", map_file.get().toUTF8());
	//map_file.save(map_file_path+U".cbj");
}

void CityMap::freeMapAndAddons() {
	for (auto i = m_objects.begin(); i != m_objects.end() ; i++) {
		delete(i->second);
	}

	for (auto i = m_addons.begin(); i != m_addons.end() ; i++) {
		delete(i->second);
	}
	
	Array<Array<Tile>>().swap(m_tiles);
	m_objects.clear();
	m_common_objects.clear();
	m_addons.clear();
}
