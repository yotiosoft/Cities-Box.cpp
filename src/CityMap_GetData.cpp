//
//  CityMap_GetData.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/26.
//

#include "CityMap.hpp"

SimulationSnapshot CityMap::updateWorld(int minutesDelta) {
	rust::Vec<rust::citymap::ResidentialTileState> residentialTiles;
	rust::Vec<rust::citymap::WorkPlaceTileState> workPlaceTiles;
	rust::Vec<rust::citymap::SchoolTileState> schoolTiles;
	rust::Vec<rust::citymap::DemandTileState> demandTiles;
	rust::citymap::SimulationMapStats mapStats;
	const bool runsDailyUpdate = m_rust_core->will_run_daily_update(minutesDelta);

	// 日付をまたぐ場合だけ、C++が所有する最新の住宅状態を値として渡す。
	if (runsDailyUpdate) {
		for (int y = 0; y < m_map_size.y; ++y) {
			for (int x = 0; x < m_map_size.x; ++x) {
				Tile& tile = m_tiles[y][x];
				rust::citymap::DemandTileState demandState;
				demandState.land_price = tile.getRate(RateID::LandPrice);
				demandState.crime_rate = tile.getRate(RateID::CrimeRate);
				demandState.education_rate = tile.getRate(RateID::EducationRate);
				demandTiles.push_back(std::move(demandState));
				Object* residentialObject = tile.hasCategory(CategoryID::Residential);
				if (residentialObject != nullptr && residentialObject->getAddonP() != nullptr) {
					++mapStats.residential_tiles;

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
					for (const WorkPlaceStruct& workPlace : tile.workPlaces) {
						state.work_place_kinds.push_back(static_cast<int32_t>(workPlace.workPlace));
						state.work_place_serial_numbers.push_back(workPlace.workPlacesSerialNumber);
					}
					for (const SchoolStruct& school : tile.schools) {
						state.school_kinds.push_back(static_cast<int32_t>(school.school));
						state.school_serial_numbers.push_back(school.schoolSerialNumber);
					}
					residentialTiles.push_back(std::move(state));
				}

				auto addWorkPlace = [&](CategoryID::Type category, RCOIFP::Type kind, int workers) {
					Object* object = tile.hasCategory(category);
					if (object == nullptr || object->getAddonP() == nullptr) {
						return false;
					}
					rust::citymap::WorkPlaceTileState state;
					state.x = x;
					state.y = y;
					state.kind = static_cast<int32_t>(kind);
					state.serial_number = object->getObjectID();
					state.maximum_capacity = object->getAddonP()->getMaximumCapacity();
					state.workers = workers;
					workPlaceTiles.push_back(std::move(state));
					return true;
				};
				if (!addWorkPlace(CategoryID::Commecial, RCOIFP::Commercial, tile.workers.commercial)
					&& !addWorkPlace(CategoryID::Office, RCOIFP::Office, tile.workers.office)
					&& !addWorkPlace(CategoryID::Industrial, RCOIFP::Industrial, tile.workers.industrial)
					&& !addWorkPlace(CategoryID::Farm, RCOIFP::Farm, tile.workers.farm)) {
					addWorkPlace(CategoryID::Public, RCOIFP::Public, tile.workers.publicFacility);
				}

				auto addSchool = [&](CategoryID::Type category, School::Type kind) {
					Object* object = tile.hasCategory(category);
					if (object == nullptr || object->getAddonP() == nullptr) {
						return false;
					}
					rust::citymap::SchoolTileState state;
					state.x = x;
					state.y = y;
					state.kind = static_cast<int32_t>(kind);
					state.serial_number = object->getObjectID();
					state.maximum_capacity = object->getAddonP()->getMaximumCapacity();
					state.students = tile.students;
					schoolTiles.push_back(std::move(state));
					return true;
				};
				if (!addSchool(CategoryID::ElementarySchool, School::ElementarySchool)
					&& !addSchool(CategoryID::JuniorHighSchool, School::JuniorHighSchool)
					&& !addSchool(CategoryID::HighSchool, School::HighSchool)) {
					addSchool(CategoryID::University, School::University);
				}

				mapStats.commercial_tiles += tile.hasCategory(CategoryID::Commecial) != nullptr;
				mapStats.office_tiles += tile.hasCategory(CategoryID::Office) != nullptr;
				mapStats.industrial_tiles += tile.hasCategory(CategoryID::Industrial) != nullptr;
				mapStats.farm_tiles += tile.hasCategory(CategoryID::Farm) != nullptr;
				mapStats.police_stations += tile.hasCategory(CategoryID::Police) != nullptr;
				mapStats.fire_departments += tile.hasCategory(CategoryID::FireDepartment) != nullptr;
				mapStats.post_offices += tile.hasCategory(CategoryID::PostOffice) != nullptr;
				mapStats.education_facilities += tile.hasCategory(CategoryID::Education) != nullptr;
			}
		}
	}

	auto update = m_rust_core->update_world(
		minutesDelta,
		std::move(residentialTiles),
		std::move(workPlaceTiles),
		std::move(schoolTiles),
		std::move(demandTiles),
		mapStats
	);
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
		tile.workPlaces.clear();
		const size_t workPlaceCount = Min(state.work_place_kinds.size(), state.work_place_serial_numbers.size());
		for (size_t i = 0; i < workPlaceCount; ++i) {
			tile.workPlaces << WorkPlaceStruct{
				UnitaryTools::getRCOIFP(state.work_place_kinds[i]),
				state.work_place_serial_numbers[i]
			};
		}
		tile.schools.clear();
		const size_t schoolCount = Min(state.school_kinds.size(), state.school_serial_numbers.size());
		for (size_t i = 0; i < schoolCount; ++i) {
			tile.schools << SchoolStruct{
				UnitaryTools::getSchool(state.school_kinds[i]),
				state.school_serial_numbers[i]
			};
		}

		if (runsDailyUpdate) {
			auto workKindName = [](int32_t kind) {
				switch (UnitaryTools::getRCOIFP(kind)) {
				case RCOIFP::Commercial: return U"commercial";
				case RCOIFP::Office: return U"office";
				case RCOIFP::Industrial: return U"industrial";
				case RCOIFP::Farm: return U"farm";
				case RCOIFP::Public: return U"public";
				default: return U"unknown";
				}
			};
			auto schoolKindName = [](int32_t kind) {
				switch (UnitaryTools::getSchool(kind)) {
				case School::ElementarySchool: return U"elementary";
				case School::JuniorHighSchool: return U"junior-high";
				case School::HighSchool: return U"high-school";
				case School::University: return U"university";
				default: return U"unknown";
				}
			};
			String destinations;
			for (size_t i = 0; i < workPlaceCount; ++i) {
				if (!destinations.isEmpty()) {
					destinations += U", ";
				}
				destinations += U"{}#{}"_fmt(workKindName(state.work_place_kinds[i]), state.work_place_serial_numbers[i]);
			}
			String schools;
			for (size_t i = 0; i < schoolCount; ++i) {
				if (!schools.isEmpty()) {
					schools += U", ";
				}
				schools += U"{}#{}"_fmt(schoolKindName(state.school_kinds[i]), state.school_serial_numbers[i]);
			}
			UnitaryTools::debugLog(
				U"employment",
				CoordinateStruct{ state.x, state.y },
				U"residents={}, workers={}, workplaces=[{}], students={}, schools=[{}]"_fmt(
					state.residents, workPlaceCount, destinations, schoolCount, schools
				)
			);
		}
	}

	for (const auto& state : update.work_place_tiles) {
		if (state.x < 0 || state.y < 0 || state.x >= m_map_size.x || state.y >= m_map_size.y) {
			continue;
		}
		Tile& tile = m_tiles[state.y][state.x];
		switch (UnitaryTools::getRCOIFP(state.kind)) {
		case RCOIFP::Commercial: tile.workers.commercial = state.workers; break;
		case RCOIFP::Office: tile.workers.office = state.workers; break;
		case RCOIFP::Industrial: tile.workers.industrial = state.workers; break;
		case RCOIFP::Farm: tile.workers.farm = state.workers; break;
		case RCOIFP::Public: tile.workers.publicFacility = state.workers; break;
		default: break;
		}
	}

	for (const auto& state : update.school_tiles) {
		if (state.x >= 0 && state.y >= 0 && state.x < m_map_size.x && state.y < m_map_size.y) {
			m_tiles[state.y][state.x].students = state.students;
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
