//
//  CityMap_SaveData.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "CityMap.hpp"

void CityMap::syncToRust() {
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
    std::string jsonStr = static_cast<std::string>(m_rust_core->generate_save_json());

	System::MessageBoxOK(Unicode::FromUTF8(jsonStr));

	// 3. XORでエンコードして保存
	UnitaryTools::saveTextFile(m_map_file_path.toUTF8(), UnitaryTools::stringXOR(jsonStr, "citiesboxmapdatafilexor"));
	//saveTextFile(map_file_path.toUTF8()+".cbj", map_file.get().toUTF8());
	//map_file.save(map_file_path+U".cbj");
	
	return true;
}

void CityMap::freeMapAndAddons() {
	for (auto i = m_addons.begin(); i != m_addons.end() ; i++) {
		delete(i->second);
	}
	
	for (auto i = m_objects.begin(); i != m_objects.end() ; i++) {
		delete(i->second);
	}
	
	Array<Array<Tile>>().swap(m_tiles);
}
