//
//  CityMap_LoadData.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/25.
//

#include "CityMap.hpp"

namespace {
	String fromRustString(const rust::String& value) {
		return Unicode::FromUTF8(std::string(value.data(), value.size()));
	}
}

bool CityMap::load(String loadMapFilePath) {
	m_last_load_error.clear();
	if (FileSystem::Extension(loadMapFilePath) != U"cbj") {
		m_last_load_error = U"対応していないファイル形式です。";
		return false;
	}

	try {
		if (m_load_CBJ(loadMapFilePath)) {
			return true;
		}
	}
	catch (const std::exception& exception) {
		m_last_load_error = U"読み込み処理中にエラーが発生しました: {}"_fmt(Unicode::FromUTF8(exception.what()));
	}
	catch (...) {
		m_last_load_error = U"読み込み処理中に不明なエラーが発生しました。";
	}

	freeMapAndAddons();
	return false;
}

bool CityMap::m_load_CBJ(String loadMapFilePath) {
	m_map_file_path = loadMapFilePath;
	if (!m_rust_core->load_from_file(loadMapFilePath.toUTF8())) {
		m_last_load_error = fromRustString(m_rust_core->get_last_load_error());
		return false;
	}

	// 復号、JSON解析、検証、旧形式変換はRust側で完了済み。
	// C++側では描画・ゲームロジック用オブジェクトだけを復元する。
	m_saved_version = 142;
	m_addon_set_name = fromRustString(m_rust_core->get_addon_set_name());
	m_city_name = fromRustString(m_rust_core->get_city_name());
	m_mayor_name = fromRustString(m_rust_core->get_mayor_name());
	m_change_weather = m_rust_core->get_change_weather();
	m_dark_on_night = m_rust_core->get_dark_on_night();
	m_map_size = Size{ m_rust_core->get_map_width(), m_rust_core->get_map_height() };

	m_budget.police = m_rust_core->get_budget_police();
	m_budget.fireDepertment = m_rust_core->get_budget_fire();
	m_budget.postOffice = m_rust_core->get_budget_post();
	m_budget.education = m_rust_core->get_budget_education();
	m_tax.residential = m_rust_core->get_tax_residential();
	m_tax.commercial = m_rust_core->get_tax_commercial();
	m_tax.office = m_rust_core->get_tax_office();
	m_tax.industrial = m_rust_core->get_tax_industrial();
	m_tax.farm = m_rust_core->get_tax_farm();

	m_load_addons(m_addon_set_name);
	if (m_addons.empty()) {
		m_last_load_error = U"アドオンセット「{}」を読み込めませんでした。"_fmt(m_addon_set_name);
		return false;
	}

	m_max_object_id = 0;
	auto loadedObjects = m_rust_core->get_loaded_objects();
	for (const auto& loaded : loadedObjects) {
		const String addonName = fromRustString(loaded.addon_name);
		const auto addonIt = m_addons.find(addonName);
		if (addonIt == m_addons.end()) {
			m_last_load_error = U"必要なアドオン「{}」が見つかりません。"_fmt(addonName);
			return false;
		}

		const String originalName = fromRustString(loaded.original_name);
		const TypeID::Type typeID = UnitaryTools::typeNameToTypeID(fromRustString(loaded.type_name));
		const DirectionID::Type directionID = UnitaryTools::directionNameToDirectionID(fromRustString(loaded.direction_name));
		const CoordinateStruct origin{ loaded.origin_x, loaded.origin_y };
		Object* object = nullptr;

		if (addonIt->second->isInCategories(CategoryID::Connectable)) {
			object = new ConnectableObject(loaded.id, addonIt->second, originalName, typeID, directionID, origin);
		}
		else if (addonIt->second->isInCategories(CategoryID::Tile) && m_common_objects.find(addonName) == m_common_objects.end()) {
			object = new NormalObject(loaded.id, addonIt->second, originalName, typeID, directionID, origin);
			object->setCommonObject();
			m_common_objects[addonName] = object;
		}
		else {
			object = new NormalObject(loaded.id, addonIt->second, originalName, typeID, directionID, origin);
		}

		object->setVisible(loaded.visible);
		m_objects[loaded.id] = object;
		m_max_object_id = Max(m_max_object_id, loaded.id);
	}

	auto loadedTiles = m_rust_core->get_loaded_tiles();
	const size_t expectedTileCount = static_cast<size_t>(m_map_size.x) * static_cast<size_t>(m_map_size.y);
	if (loadedTiles.size() != expectedTileCount) {
		m_last_load_error = U"Rust側から受け取ったタイル数がマップサイズと一致しません。";
		return false;
	}

	m_tiles.clear();
	m_tiles.resize(m_map_size.y);
	for (auto& row : m_tiles) {
		row.resize(m_map_size.x);
	}
	Array<int> duplicateTileObjectIDs;

	for (int y = 0; y < m_map_size.y; ++y) {
		for (int x = 0; x < m_map_size.x; ++x) {
			const auto& loaded = loadedTiles[static_cast<size_t>(y) * m_map_size.x + x];
			Tile& tile = m_tiles[y][x];
			tile.residents = loaded.residents;
			tile.workers.commercial = loaded.workers_commercial;
			tile.workers.office = loaded.workers_office;
			tile.workers.industrial = loaded.workers_industrial;
			tile.workers.farm = loaded.workers_farm;
			tile.workers.publicFacility = loaded.workers_public;
			tile.students = loaded.students;
			tile.reservation = UnitaryTools::getRCOIFP(loaded.reservation);
			tile.setOriginalName(fromRustString(loaded.original_name));

			for (const int age : loaded.ages) {
				tile.age << age;
			}
			for (const auto& gender : loaded.genders) {
				tile.gender << fromRustString(gender);
			}
			for (const auto& rate : loaded.rates) {
				tile.rate[UnitaryTools::rateNameToRateID(fromRustString(rate.name))] = rate.value;
			}
			for (const auto& workPlace : loaded.work_places) {
				tile.workPlaces << WorkPlaceStruct{ UnitaryTools::getRCOIFP(workPlace.kind), workPlace.serial_number };
			}
			for (const auto& school : loaded.schools) {
				tile.schools << SchoolStruct{ UnitaryTools::getSchool(school.kind), school.serial_number };
			}

			for (const auto& objectRef : loaded.objects) {
				const auto objectIt = m_objects.find(objectRef.object_id);
				if (objectIt == m_objects.end()) {
					m_last_load_error = U"存在しないObjectID {}がタイルから参照されています。"_fmt(objectRef.object_id);
					return false;
				}

				Object* sourceObject = objectIt->second;
				Object* tileObject = sourceObject;
				if (sourceObject->getAddonP()->isInCategories(CategoryID::Tile)) {
					const String addonName = sourceObject->getAddonP()->getName(NameMode::English);
					const auto commonIt = m_common_objects.find(addonName);
					if (commonIt != m_common_objects.end()) {
						tileObject = commonIt->second;
						if (tileObject->getObjectID() != objectRef.object_id && !duplicateTileObjectIDs.contains(objectRef.object_id)) {
							duplicateTileObjectIDs << objectRef.object_id;
						}
					}
				}

				RelativeCoordinateStruct relativeCoordinate;
				relativeCoordinate.origin = sourceObject->getOriginCoordinate();
				relativeCoordinate.relative = CoordinateStruct{ objectRef.relative_x, objectRef.relative_y };
				tile.addObject(tileObject, relativeCoordinate);
			}
		}
	}

	// Tileカテゴリは全タイルで共通オブジェクトを使うため、個別定義を破棄する。
	for (const int objectID : duplicateTileObjectIDs) {
		const auto objectIt = m_objects.find(objectID);
		if (objectIt != m_objects.end()) {
			delete objectIt->second;
			m_objects.erase(objectIt);
		}
	}

	return true;
}

void CityMap::m_load_addons(String addonSetName) {
	Array<FileStruct> addonsPath = specific::getAllFilesName("./addons", "adj");

	for (int i = 0; i < static_cast<int>(addonsPath.size()); ++i) {
		UnitaryTools::debugLog(U"m_load_addons", U"from " + Unicode::Widen(addonsPath[i].file_path));
		CBAddon* loadingAddon = new CBAddon();
		if (loadingAddon->load(addonsPath[i], addonSetName)) {
			m_addons[loadingAddon->getName(NameMode::English)] = loadingAddon;
		}
		else {
			delete loadingAddon;
		}
		System::Sleep(20);
	}
}
