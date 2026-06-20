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

	struct StagedCityMap {
		Array<Array<Tile>> tiles;
		map<String, CBAddon*> addons;
		map<int, Object*> objects;
		map<String, Object*> commonObjects;
		int maxObjectID = 0;

		~StagedCityMap() {
			for (auto& [id, object] : objects) {
				delete object;
			}
			for (auto& [name, addon] : addons) {
				delete addon;
			}
		}
	};
}

bool CityMap::load(String loadMapFilePath) {
	m_last_load_error.clear();
	if (FileSystem::Extension(loadMapFilePath) != U"cbj") {
		m_last_load_error = U"対応していないファイル形式です。";
		return false;
	}

	try {
		return m_load_CBJ(loadMapFilePath);
	}
	catch (const std::exception& exception) {
		m_last_load_error = U"読み込み処理中にエラーが発生しました: {}"_fmt(Unicode::FromUTF8(exception.what()));
	}
	catch (...) {
		m_last_load_error = U"読み込み処理中に不明なエラーが発生しました。";
	}
	return false;
}

bool CityMap::m_load_CBJ(String loadMapFilePath) {
	auto loadResult = m_rust_core->load_city_map(loadMapFilePath.toUTF8());
	if (!loadResult.success) {
		m_last_load_error = fromRustString(loadResult.error_message);
		return false;
	}
	const auto& loadedCity = loadResult.city;
	const Size loadedMapSize{ loadedCity.map_width, loadedCity.map_height };
	String loadedAddonSet = fromRustString(loadedCity.addon_set_name);
	String loadedCityName = fromRustString(loadedCity.city_name);
	String loadedMayorName = fromRustString(loadedCity.mayor_name);

	StagedCityMap staged;
	m_load_addons(loadedAddonSet, staged.addons);
	if (staged.addons.empty()) {
		m_last_load_error = U"アドオンセット「{}」を読み込めませんでした。"_fmt(loadedAddonSet);
		return false;
	}

	// Rustで検証済みの定義から、C++描画用Objectだけを生成する。
	for (const auto& loaded : loadedCity.objects) {
		const String addonName = fromRustString(loaded.addon_name);
		const auto addonIt = staged.addons.find(addonName);
		if (addonIt == staged.addons.end()) {
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
		else if (addonIt->second->isInCategories(CategoryID::Tile) && staged.commonObjects.find(addonName) == staged.commonObjects.end()) {
			object = new NormalObject(loaded.id, addonIt->second, originalName, typeID, directionID, origin);
			object->setCommonObject();
			staged.commonObjects[addonName] = object;
		}
		else {
			object = new NormalObject(loaded.id, addonIt->second, originalName, typeID, directionID, origin);
		}

		object->setVisible(loaded.visible);
		staged.objects[loaded.id] = object;
		staged.maxObjectID = Max(staged.maxObjectID, loaded.id);
	}

	const size_t expectedTileCount = static_cast<size_t>(loadedMapSize.x) * static_cast<size_t>(loadedMapSize.y);
	if (loadedCity.tiles.size() != expectedTileCount) {
		m_last_load_error = U"Rust側から受け取ったタイル数がマップサイズと一致しません。";
		return false;
	}

	staged.tiles.resize(loadedMapSize.y);
	for (auto& row : staged.tiles) {
		row.resize(loadedMapSize.x);
	}
	Array<int> duplicateTileObjectIDs;

	for (int y = 0; y < loadedMapSize.y; ++y) {
		for (int x = 0; x < loadedMapSize.x; ++x) {
			const auto& loaded = loadedCity.tiles[static_cast<size_t>(y) * loadedMapSize.x + x];
			Tile& tile = staged.tiles[y][x];
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
				const auto objectIt = staged.objects.find(objectRef.object_id);
				if (objectIt == staged.objects.end()) {
					m_last_load_error = U"存在しないObjectID {}がタイルから参照されています。"_fmt(objectRef.object_id);
					return false;
				}

				Object* sourceObject = objectIt->second;
				Object* tileObject = sourceObject;
				if (sourceObject->getAddonP()->isInCategories(CategoryID::Tile)) {
					const String addonName = sourceObject->getAddonP()->getName(NameMode::English);
					const auto commonIt = staged.commonObjects.find(addonName);
					if (commonIt != staged.commonObjects.end()) {
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

	for (const int objectID : duplicateTileObjectIDs) {
		const auto objectIt = staged.objects.find(objectID);
		if (objectIt != staged.objects.end()) {
			delete objectIt->second;
			staged.objects.erase(objectIt);
		}
	}

	// C++側の構築が完了してからRust状態を確定し、両側を同時に切り替える。
	if (!m_rust_core->commit_loaded_city_map()) {
		m_last_load_error = U"Rust側のロード結果を確定できませんでした。";
		return false;
	}

	// 旧データを先に明示解放する。以降のswapでは新データの所有権だけが
	// CityMapへ移り、StagedCityMapのデストラクタは新しいポインタに触れない。
	freeMapAndAddons();
	m_tiles.swap(staged.tiles);
	m_addons.swap(staged.addons);
	m_objects.swap(staged.objects);
	m_common_objects.swap(staged.commonObjects);
	m_max_object_id = staged.maxObjectID;
	m_saved_version = loadedCity.version;
	m_addon_set_name.swap(loadedAddonSet);
	m_city_name.swap(loadedCityName);
	m_mayor_name.swap(loadedMayorName);
	m_change_weather = loadedCity.change_weather;
	m_dark_on_night = loadedCity.dark_on_night;
	m_map_size = loadedMapSize;
	m_map_file_path.swap(loadMapFilePath);
	return true;
}

void CityMap::m_load_addons(String addonSetName, map<String, CBAddon*>& destination) {
	Array<FileStruct> addonsPath = specific::getAllFilesName("./addons", "adj");

	for (int i = 0; i < static_cast<int>(addonsPath.size()); ++i) {
		UnitaryTools::debugLog(U"m_load_addons", U"from " + Unicode::Widen(addonsPath[i].file_path));
		CBAddon* loadingAddon = new CBAddon();
		if (loadingAddon->load(addonsPath[i], addonSetName)) {
			const String name = loadingAddon->getName(NameMode::English);
			const auto existing = destination.find(name);
			if (existing != destination.end()) {
				delete existing->second;
			}
			destination[name] = loadingAddon;
		}
		else {
			delete loadingAddon;
		}
		System::Sleep(20);
	}
}
