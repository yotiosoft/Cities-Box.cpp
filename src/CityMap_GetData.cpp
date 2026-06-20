//
//  CityMap_GetData.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/26.
//

#include "CityMap.hpp"

SimulationSnapshot CityMap::updateWorld(int minutesDelta) {
	rust::Vec<rust::citymap::ResidentialTileState> residentialTiles;

	// 日付をまたぐ場合だけ、C++が所有する最新の住宅状態を値として渡す。
	if (m_rust_core->will_run_daily_update(minutesDelta)) {
		for (int y = 0; y < m_map_size.y; ++y) {
			for (int x = 0; x < m_map_size.x; ++x) {
				Tile& tile = m_tiles[y][x];
				Object* residentialObject = tile.hasCategory(CategoryID::Residential);
				if (residentialObject == nullptr || residentialObject->getAddonP() == nullptr) {
					continue;
				}

				rust::citymap::ResidentialTileState state;
				state.x = x;
				state.y = y;
				state.residents = tile.residents;
				state.maximum_capacity = residentialObject->getAddonP()->getMaximumCapacity();
				for (const int age : tile.age) {
					state.ages.push_back(age);
				}
				for (const String& gender : tile.gender) {
					state.genders.push_back(gender.toUTF8());
				}
				residentialTiles.push_back(std::move(state));
			}
		}
	}

	auto update = m_rust_core->update_world(minutesDelta, std::move(residentialTiles));
	for (const auto& state : update.residential_tiles) {
		if (state.x < 0 || state.y < 0 || state.x >= m_map_size.x || state.y >= m_map_size.y) {
			continue;
		}

		Tile& tile = m_tiles[state.y][state.x];
		tile.residents = state.residents;
		tile.age.clear();
		tile.gender.clear();
		for (const int age : state.ages) {
			tile.age << age;
		}
		for (const auto& gender : state.genders) {
			tile.gender << Unicode::FromUTF8(std::string(gender.data(), gender.size()));
		}
	}

	return update.snapshot;
}

Array<CBAddon> CityMap::getAddon(CoordinateStruct coordinate) {
	Array<CBAddon> retAddons;

	for (int i = 0; i < (int)m_tiles[coordinate.y][coordinate.x].addons.size(); i++) {
		retAddons << *(m_tiles[coordinate.y][coordinate.x].addons[i]);
	}

	return retAddons;
}

Size CityMap::getMapSize() {
	return m_map_size;
}

CategoryID::Type CityMap::m_get_connectable_CategoryID(CBAddon* addon) {
	// 対象物のカテゴリを取得
	CategoryID::Type object_category = CategoryID::Disabled;
	
	Array<CategoryID::Type> object_categories = addon->getCategories();
	bool isConnectable = false;
	for (auto object_category_single : object_categories) {
		if (object_category_single == CategoryID::Connectable) {
			isConnectable = true;
		}
		
		if (object_category_single == CategoryID::Road || 
			object_category_single == CategoryID::Train ||
			object_category_single == CategoryID::Waterway || 
			object_category_single == CategoryID::Airport) {
			
			object_category = object_category_single;
			break;
		}
	}
	
	if (!isConnectable) {
		return CategoryID::Disabled;
	}
	
	return object_category;
}

CategoryID::Type CityMap::m_get_connectable_CategoryID_explicitly(CBAddon* addon) {
	// 対象物のカテゴリを取得
	CategoryID::Type object_category = CategoryID::Disabled;
	
	Array<CategoryID::Type> object_categories = addon->getCategories();
	bool isConnectable = false;
	for (auto object_category_single : object_categories) {
		if (object_category_single == CategoryID::Connectable) {
			isConnectable = true;
		}
		
		if (object_category_single == CategoryID::Road ||
			object_category_single == CategoryID::Railroad ||
			object_category_single == CategoryID::Station ||
			object_category_single == CategoryID::Waterway ||
			object_category_single == CategoryID::Taxiway ||
			object_category_single == CategoryID::Runway) {
			
			object_category = object_category_single;
			break;
		}
	}
	
	if (!isConnectable) {
		return CategoryID::Disabled;
	}
	
	return object_category;
}

Array<CBAddon*> CityMap::getFitAddons(Array<CategoryID::Type> selectedCategories) {
	Array<CBAddon*> retAddons;

	for (auto addon = m_addons.begin(); addon != m_addons.end(); addon++) {

		Array<CategoryID::Type> anAddonCategories = addon->second->getCategories();
		vector<bool> fit = vector<bool>(selectedCategories.size(), false);

		for (auto categoryName = anAddonCategories.begin(); categoryName != anAddonCategories.end(); categoryName++) {
			for (int i = 0; i < (int)selectedCategories.size(); i++) {
				if (selectedCategories[i] == *categoryName) {
					fit[i] = true;
					break;
				}
			}
		}

		bool fitted = true;
		for (int i = 0; i < (int)fit.size(); i++) {
			if (!fit[i]) {
				fitted = false;
			}
		}
		if (fitted) {
			retAddons << addon->second;
		}
	}

	return retAddons;
}

map<String, CBAddon*> CityMap::getAllAddons() {
	return m_addons;
}

// いずれかのアドオンがカテゴリに含まれているか
bool CityMap::hasCategory(CategoryID::Type searchCategory, CoordinateStruct coordinate) {
	Tile* currentTile = &m_tiles[coordinate.y][coordinate.x];

	for (int i = 0; i < (int)currentTile->addons.size(); i++) {
		if (currentTile->addons[i]->isInCategories(searchCategory)) {
			return true;
		}
	}

	return false;
}

int CityMap::m_get_next_objectID() {
	m_max_object_id++;
	int next_objectID = m_max_object_id;
	return next_objectID;
}
