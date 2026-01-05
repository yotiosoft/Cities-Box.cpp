//
//  CityMap_GetData.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/26.
//

#include "CityMap.hpp"

Array<CBAddon> CityMap::getAddon(CoordinateStruct coordinate) {
	Array<CBAddon> retAddons;

	for (int i = 0; i < (int)m_tiles[coordinate.y][coordinate.x].addons.size(); i++) {
		retAddons << *(m_tiles[coordinate.y][coordinate.x].addons[i]);
	}

	return retAddons;
}

RCOIFstruct CityMap::getDemand() {
	return m_demand;
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

bool CityMap::m_get_element(String str, String searchElementName, String& ret) {
	string strUTF8 = str.toUTF8();
	string searchElementNameUTF8 = searchElementName.toUTF8();

	if (strUTF8.find(searchElementNameUTF8) != string::npos && strUTF8.find("=") != string::npos) {
		ret = Unicode::Widen(strUTF8.substr(strUTF8.find("\"") + 1, strUTF8.find(";") - (strUTF8.find("\"") + 2)));
		return true;
	}
	return false;
}

bool CityMap::m_get_element(String str, String searchElementName, int& ret) {
	string strUTF8 = str.toUTF8();
	string searchElementNameUTF8 = searchElementName.toUTF8();

	if (strUTF8.find(searchElementNameUTF8) != string::npos && strUTF8.find("=") != string::npos) {
		ret = stoi(strUTF8.substr(strUTF8.find("= ") + 2, strUTF8.find(";") - (strUTF8.find("= ") + 2)));
		return true;
	}
	return false;
}

bool CityMap::m_get_element(String str, String searchElementName, bool& ret) {
	string strUTF8 = str.toUTF8();
	string searchElementNameUTF8 = searchElementName.toUTF8();

	if (strUTF8.find(searchElementNameUTF8) != string::npos && strUTF8.find("=") != string::npos) {
		int intTemp = stoi(strUTF8.substr(strUTF8.find("= ") + 2, strUTF8.find(";") - (strUTF8.find("= ") + 2)));
		if (intTemp == 1) {
			ret = true;
			return true;
		}
		else if (intTemp == 0) {
			ret = false;
			return true;
		}
	}
	return false;
}

bool CityMap::m_get_types(String str, String searchElementName, Array<String>& ret) {
	String aRet;
	if (m_get_element(str, searchElementName, aRet)) {
		ret = UnitaryTools::split(aRet, U", ");
		return true;
	}
	return false;
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
