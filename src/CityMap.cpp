//
//  CityMap.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "CityMap.hpp"


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
		ret = split(aRet, U", ");
		return true;
	}
	return false;
}


void CityMap::load(String loadMapFilePath) {
	if (FileSystem::Extension(loadMapFilePath) == U"cbd") {
		loadCBD(loadMapFilePath);
	}
	else if (FileSystem::Extension(loadMapFilePath) == U"cbj") {
		loadCBJ(loadMapFilePath);
	}
}

static s3d::String extracted(int i, Array<s3d::String> &workplaceStr) {
	String workplaceAndSerial = workplaceStr[i].substr(1, workplaceStr[i].length()-1);
	return workplaceAndSerial;
}

void CityMap::loadCBD(String loadMapFilePath) {
	// マップファイルの読み込み
	m_map_file_path = loadMapFilePath;
	
	TextReader mapData(m_map_file_path);
	string strTempUTF8;
	String strTemp;
	
	// 各要素の読み出し
	string currentArrayName = "";
	Array<string> arrayNames =
	{"name", "name2", "category", "category_2", "category_3", "obj_type", "obj_type2", "obj_dire", "obj_dire2",
		"obj_serial_num", "obj_use_tiles_x", "obj_use_tiles_y", "obj_tiles_x", "obj_tiles_y", "residents",
		"workers_Commercial", "workers_Office", "workers_Industrial", "workers_Farm", "workers_Public",
		"students", "land_price", "crime_rate", "education_rate", "happiness", "noise", "crop", "crop_amount",
		"age", "gender", "workplace", "school", "reservation", "original_name"};
	int arrayCount = 0;
	bool mapCleared = false;
	
	m_map_size.x = -1;
	m_map_size.y = -1;
	
	bool addonLoaded = false;
	
	while (mapData.readLine(strTemp)) {
		strTemp = strTemp.substr(0, strTemp.length()-LINE_FEED_CODE);				// 改行コードは除く
		strTempUTF8 = strTemp.toUTF8();
		
		m_get_element(strTemp, U"Version", m_saved_version);
		m_get_element(strTemp, U"Addons_Set", m_addon_set_name);
		
		// アドオン読み込み
		if (!addonLoaded && m_addon_set_name.length() > 0) {
			loadAddons(m_addon_set_name);
			addonLoaded = true;
		}
		
		m_get_element(strTemp, U"City_Name", m_city_name);
		m_get_element(strTemp, U"Mayor_Name", m_mayor_name);
		m_get_element(strTemp, U"Total_Population", m_total_population);
		m_get_element(strTemp, U"Set_Change_Weather", m_change_weather);
		m_get_element(strTemp, U"Temperature", m_temperature);
		m_get_element(strTemp, U"Set_Dark_on_Night", m_dark_on_night);
		
		m_get_element(strTemp, U"mapsize_x", m_map_size.x);
		m_get_element(strTemp, U"mapsize_y", m_map_size.y);
		
		m_get_element(strTemp, U"time_Year", m_time_now.year);
		m_get_element(strTemp, U"time_Month", m_time_now.month);
		m_get_element(strTemp, U"time_Date", m_time_now.date);
		m_get_element(strTemp, U"time_Hour", m_time_now.hour);
		m_get_element(strTemp, U"time_Minutes", m_time_now.minutes);
		
		m_get_element(strTemp, U"demand_Residential", m_demand.residential);
		m_get_element(strTemp, U"demand_Commercial", m_demand.commercial);
		m_get_element(strTemp, U"demand_Office", m_demand.office);
		m_get_element(strTemp, U"demand_Industrial", m_demand.industrial);
		m_get_element(strTemp, U"demand_Farm", m_demand.farm);
		
		m_get_element(strTemp, U"Money", m_money);
		
		m_get_element(strTemp, U"budget_of_Police", m_budget.police);
		m_get_element(strTemp, U"budget_of_Fire_Depertment", m_budget.fireDepertment);
		m_get_element(strTemp, U"budget_of_Post_Office", m_budget.postOffice);
		m_get_element(strTemp, U"budget_of_Education", m_budget.education);
		
		m_get_element(strTemp, U"tax_of_Residential", m_tax.residential);
		m_get_element(strTemp, U"tax_of_Commercial", m_tax.commercial);
		m_get_element(strTemp, U"tax_of_Office", m_tax.office);
		m_get_element(strTemp, U"tax_of_Industrial", m_tax.industrial);
		m_get_element(strTemp, U"tax_of_Farm", m_tax.farm);
		
		if (!mapCleared && m_map_size.x > 0 && m_map_size.y > 0) {
			for (int y=0; y<m_map_size.y; y++) {
				Tile new_tile;
				m_tiles.push_back(Array<Tile>(m_map_size.x, new_tile));
			}
			mapCleared = true;
		}
		
		for (int i=0; i<arrayNames.size(); i++) {
			if (strTempUTF8.find(arrayNames[i]+"(x,y) {") == 0) {
				currentArrayName = arrayNames[i];
				arrayCount = -1;
			}
		}
		if (strTempUTF8.find("}") == 0) {
			currentArrayName = "";
		}
		
		if (currentArrayName == "name" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				// マップにAddon_Setが定義されていない場合はNormalとみなしアドオン読み込み
				if (!addonLoaded) {
					m_addon_set_name = U"Normal";
					loadAddons(m_addon_set_name);
					addonLoaded = true;
				}
				
				// アドオンのポインタを登録
				if (m_addons.find(temp[x]) != m_addons.end()) {
					m_tiles[arrayCount][x].addons << m_addons[temp[x]];
					
					//tiles[array_count][x].category = tiles[array_count][x].addons.back()->getCategories();
				}
			}
		}
		
		if (currentArrayName == "name2" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				if (temp[x].length() == 0) {
					continue;
				}
				
				// アドオンのポインタを登録
				if (m_addons.find(temp[x]) != m_addons.end()) {
					Addon* addon_temp = m_tiles[arrayCount][x].addons[0];
					m_tiles[arrayCount][x].addons.back() = m_addons[temp[x]];
					m_tiles[arrayCount][x].addons << addon_temp;
					
					cout << m_tiles[arrayCount][x].addons[1]->getName(NameMode::English) << endl;
					
					/*Array<String> categories = tiles[array_count][x].addons.back()->getCategories();
					for (int i=0; i<categories.size(); i++) {
						tiles[array_count][x].category.push_back(categories[i]);
					}*/
				}
			}
		}
		/*
		if (current_array_name == "category" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[array_count][x].category.push_back(temp[x]);
			}
		}
		
		if (current_array_name == "category_2" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[array_count][x].category.push_back(temp[x]);
			}
		}
		
		if (current_array_name == "category_3" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[array_count][x].category.push_back(temp[x]);
			}
		}*/
		
		if (currentArrayName == "obj_type" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				if (m_tiles[arrayCount][x].addons.size() == 2) {
					m_tiles[arrayCount][x].addType(m_tiles[arrayCount][x].addons[1]->getTypeID(stoi(temp[x].toUTF8())));	// とりあえず同じものを[0]にも入れておく
					m_tiles[arrayCount][x].addType(m_tiles[arrayCount][x].addons[1]->getTypeID(stoi(temp[x].toUTF8())));
				}
				else {
					m_tiles[arrayCount][x].addType(m_tiles[arrayCount][x].addons[0]->getTypeID(stoi(temp[x].toUTF8())));
				}
			}
		}
		
		if (currentArrayName == "obj_type2" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				if (m_tiles[arrayCount][x].addons.size() == 2) {
					m_tiles[arrayCount][x].addType(m_tiles[arrayCount][x].addons[0]->getTypeID(stoi(temp[x].toUTF8())));
				}
			}
		}
		
		if (currentArrayName == "obj_dire" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				if (m_tiles[arrayCount][x].addons.size() == 2) {
					m_tiles[arrayCount][x].addDirection(m_tiles[arrayCount][x].addons[1]->getDirectionID(m_tiles[arrayCount][x].getType(1), stoi(temp[x].toUTF8())));	// とりあえず同じものを[0]にも入れておく
					m_tiles[arrayCount][x].addDirection(m_tiles[arrayCount][x].addons[1]->getDirectionID(m_tiles[arrayCount][x].getType(1), stoi(temp[x].toUTF8())));
				}
				else {
					m_tiles[arrayCount][x].addDirection(m_tiles[arrayCount][x].addons[0]->getDirectionID(m_tiles[arrayCount][x].getType(0), stoi(temp[x].toUTF8())));
				}
			}
		}
		
		if (currentArrayName == "obj_dire2" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				if (m_tiles[arrayCount][x].addons.size() == 2) {
					m_tiles[arrayCount][x].addDirection(m_tiles[arrayCount][x].addons[0]->getDirectionID(m_tiles[arrayCount][x].getType(0), stoi(temp[x].toUTF8())));
				}
			}
		}
		
		if (currentArrayName == "obj_serial_num" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].serialNumber = stoi(temp[x].toUTF8());
			}
		}
		/*
		if (current_array_name == "obj_use_tiles_x" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[array_count][x].use_tiles.x = stoi(temp[x].toUTF8());
				
				if (tiles[array_count][x].use_tiles.x == 0) {
					tiles[array_count][x].use_tiles.x = 1;
				}
			}
		}
		
		if (current_array_name == "obj_use_tiles_y" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[array_count][x].use_tiles.y = stoi(temp[x].toUTF8());
				
				if (tiles[array_count][x].use_tiles.y == 0) {
					tiles[array_count][x].use_tiles.y = 1;
				}
			}
		}
		*/
		if (currentArrayName == "obj_tiles_x" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].tilesCount.x = stoi(temp[x].toUTF8());
				
				// obj_tiles_xを修正(r140以前のバージョンで保存した場合)
				if (m_saved_version <= 140) {
					if (m_tiles[arrayCount][x].addons[0]->getUseTiles(m_tiles[arrayCount][x].getType(0), m_tiles[arrayCount][x].getDirection(0)).x > 0) {
						// 左向き
						if (m_tiles[arrayCount][x].getDirection(0) == DirectionID::Left) {
							
						}
						
						// 上向き
						if (m_tiles[arrayCount][x].getDirection(0) == DirectionID::Top) {
							
						}
						
						// 下向き
						if (m_tiles[arrayCount][x].getDirection(0) == DirectionID::Bottom) {
							
						}
						
						// 右向き
						if (m_tiles[arrayCount][x].getDirection(0) == DirectionID::Right) {
							m_tiles[arrayCount][x].tilesCount.x += m_tiles[arrayCount][x].addons[0]->getUseTiles(m_tiles[arrayCount][x].getType(0), m_tiles[arrayCount][x].getDirection(0)).x - 1;
						}
					}
				}
			}
		}
		
		if (currentArrayName == "obj_tiles_y" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].tilesCount.y = stoi(temp[x].toUTF8());
				
				// obj_tiles_yを修正(r140以前のバージョンで保存した場合)
				if (m_saved_version <= 140) {
					if (m_tiles[arrayCount][x].addons[0]->getUseTiles(m_tiles[arrayCount][x].getType(0), m_tiles[arrayCount][x].getDirection(0)).y > 0) {
						// 左向き
						if (m_tiles[arrayCount][x].getDirection(0) == DirectionID::Left) {
							
						}
						
						// 上向き
						if (m_tiles[arrayCount][x].getDirection(0) == DirectionID::Top) {
							m_tiles[arrayCount][x].tilesCount.y = m_tiles[arrayCount][x].addons[0]->getUseTiles(m_tiles[arrayCount][x].getType(0), m_tiles[arrayCount][x].getDirection(0)).y - 1 - m_tiles[arrayCount][x].tilesCount.y;
						}
						
						// 下向き
						if (m_tiles[arrayCount][x].getDirection(0) == DirectionID::Bottom) {
							m_tiles[arrayCount][x].tilesCount.y = abs(m_tiles[arrayCount][x].tilesCount.y);
						}
						
						// 右向き
						if (m_tiles[arrayCount][x].getDirection(0) == DirectionID::Right) {
							m_tiles[arrayCount][x].tilesCount.y = m_tiles[arrayCount][x].addons[0]->getUseTiles(m_tiles[arrayCount][x].getType(0), m_tiles[arrayCount][x].getDirection(0)).y - 1 - m_tiles[arrayCount][x].tilesCount.y;
						}
					}
				}
			}
		}
		
		if (currentArrayName == "residents" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].residents = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "workers_Commercial" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].workers.commercial = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "workers_Office" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].workers.office = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "workers_Industrial" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].workers.industrial = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "workers_Farm" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].workers.farm = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "workers_Public" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].workers.publicFacility = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "students" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].students = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "land_price" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].rate[U"land_price"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "happiness" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].rate[U"happiness_rate"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "crime_rate" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].rate[U"crime_rate"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "happiness" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].rate[U"happiness_rate"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "education_rate" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].rate[U"education_rate"] = stoi(temp[x].toUTF8());
			}
		}
		/*
		if (current_array_name == "crop" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[array_count][x].crop.name = temp[x];
			}
		}
		
		if (current_array_name == "crop_amount" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[array_count][x].crop.amount = stoi(temp[x].toUTF8());
			}
		}
		*/
		if (currentArrayName == "age" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				Array<String> agesStr = split(temp[x], U"]");
				
				for (int i=0; i<agesStr.size(); i++) {
					if (agesStr[i].length() <= 1 || agesStr[i] == U" ") {
						continue;
					}
					m_tiles[arrayCount][x].age.push_back(stoi(agesStr[i].substr(1, agesStr[i].length()-1).toUTF8()));
				}
			}
		}
		
		if (currentArrayName == "gender" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				Array<String> genderStr = split(temp[x], U"]");
				
				for (int i=0; i<genderStr.size(); i++) {
					if (genderStr[i].length() <= 1 || genderStr[i] == U" ") {
						continue;
					}
					m_tiles[arrayCount][x].gender.push_back(genderStr[i].substr(1, genderStr[i].length()-1));
				}
			}
		}
		
		if (currentArrayName == "workplace" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				Array<String> workPlaceStr = split(temp[x], U"]");
				
				for (int i=0; i<workPlaceStr.size(); i++) {
					if (workPlaceStr[i].length() <= 1 || workPlaceStr[i] == U" ") {
						continue;
					}
					s3d::String workPlaceAndSerial = extracted(i, workPlaceStr);
					
					WorkPlaceStruct workPlace;
					if (workPlaceAndSerial.substr(0, 1) == U"c") {
						workPlace.workPlace = RCOIFP::Commercial;
					}
					else if (workPlaceAndSerial.substr(0, 1) == U"o") {
						workPlace.workPlace = RCOIFP::Office;
					}
					else if (workPlaceAndSerial.substr(0, 1) == U"i") {
						workPlace.workPlace = RCOIFP::Industrial;
					}
					else if (workPlaceAndSerial.substr(0, 1) == U"f") {
						workPlace.workPlace = RCOIFP::Farm;
					}
					else if (workPlaceAndSerial.substr(0, 1) == U"p") {
						workPlace.workPlace = RCOIFP::Public;
					}
					
					workPlace.workPlacesSerialNumber = stoi(workPlaceAndSerial.substr(1, workPlaceAndSerial.length()-1).toUTF8());
					
					m_tiles[arrayCount][x].workPlaces.push_back(workPlace);
				}
			}
		}
		
		if (currentArrayName == "school" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				Array<String> schoolStr = split(temp[x], U"]");
				
				for (int i=0; i<schoolStr.size(); i++) {
					if (schoolStr[i].length() <= 1 || schoolStr[i] == U" ") {
						continue;
					}
					String schoolAndSerial = schoolStr[i].substr(1, schoolStr[i].length()-1);
					
					SchoolStruct school;
					if (schoolAndSerial.substr(0, 1) == U"e") {
						school.school = School::ElementarySchool;
					}
					else if (schoolAndSerial.substr(0, 1) == U"j") {
						school.school = School::JuniorHighSchool;
					}
					else if (schoolAndSerial.substr(0, 1) == U"h") {
						school.school = School::HighSchool;
					}
					else if (schoolAndSerial.substr(0, 1) == U"U") {
						school.school = School::University;
					}
					
					school.schoolSerialNumber = stoi(schoolAndSerial.substr(1, schoolAndSerial.length()-1).toUTF8());
					
					m_tiles[arrayCount][x].schools.push_back(school);				}
			}
		}
		
		if (currentArrayName == "reservation" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				if (temp[x] == U"none") {
					m_tiles[arrayCount][x].reservation = RCOIFP::None;
				}
				else if (temp[x] == U"residential") {
					m_tiles[arrayCount][x].reservation = RCOIFP::Residential;
				}
				else if (temp[x] == U"commercial") {
					m_tiles[arrayCount][x].reservation = RCOIFP::Commercial;
				}
				else if (temp[x] == U"office") {
					m_tiles[arrayCount][x].reservation = RCOIFP::Office;
				}
				else if (temp[x] == U"industrial") {
					m_tiles[arrayCount][x].reservation = RCOIFP::Industrial;
				}
				else if (temp[x] == U"farm") {
					m_tiles[arrayCount][x].reservation = RCOIFP::Farm;
				}
				else if (temp[x] == U"public") {
					m_tiles[arrayCount][x].reservation = RCOIFP::Public;
				}
			}
		}
		
		if (currentArrayName == "original_name" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<m_map_size.x; x++) {
				m_tiles[arrayCount][x].setOriginalName(temp[x]);
			}
		}
		
		arrayCount ++;
	}
	
	System::ShowMessageBox(U"旧形式のマップデータ(*.cbd)が読み込まれました。\n保存時は新形式(*.cbj)で保存されます。", MessageBoxStyle::Warning, MessageBoxButtons::OK);
}

void CityMap::loadCBJ(String loadMapFilePath) {
	m_map_file_path = loadMapFilePath;
	
	ifstream ifs(m_map_file_path.toUTF8().c_str(), ios::in | ios::binary);
	
	std::string mapXOR((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	string mapDataStr = stringXOR(mapXOR, "citiesboxmapdatafilexor");
	saveTextFile("./data/map_temp.cbj_unxor", mapDataStr);
	saveTextFile("./data/map_temp.cbj_temp", mapDataStr);
	
	JSONReader mapData(U"./data/map_temp.cbj_temp");
	//remove("./data/map_temp.cbj_temp");
	
	m_saved_version = mapData[U"Version"].get<int>();
	
	m_addon_set_name = mapData[U"Addon_Set"].getString();
	// -> アドオン読み込み
	loadAddons(m_addon_set_name);
	
	m_city_name = mapData[U"City_Name"].getString();
	
	m_mayor_name = mapData[U"Mayor_Name"].getString();
	
	m_total_population = mapData[U"Total_Population"].get<int>();
	
	m_change_weather = mapData[U"Change_Weather"].get<bool>();
	
	m_temperature = mapData[U"Temperature"].get<int>();
	
	m_dark_on_night = mapData[U"Dark_on_Night"].get<bool>();
	
	m_map_size.x = mapData[U"Map_size.width"].get<int>();
	m_map_size.y = mapData[U"Map_size.height"].get<int>();
	
	m_time_now.year = mapData[U"Time.year"].get<int>();
	m_time_now.month = mapData[U"Time.month"].get<int>();
	m_time_now.date = mapData[U"Time.date"].get<int>();
	m_time_now.hour = mapData[U"Time.hour"].get<int>();
	m_time_now.minutes = mapData[U"Time.minutes"].get<int>();
	
	m_demand.residential = mapData[U"Demand.residential"].get<int>();
	m_demand.commercial = mapData[U"Demand.commercial"].get<int>();
	m_demand.office = mapData[U"Demand.office"].get<int>();
	m_demand.industrial = mapData[U"Demand.industrial"].get<int>();
	m_demand.farm = mapData[U"Demand.farm"].get<int>();
	
	m_money = mapData[U"Money"].get<int>();
	
	m_budget.police = mapData[U"Budget.police"].get<int>();
	m_budget.fireDepertment = mapData[U"Budget.fire_depertment"].get<int>();
	m_budget.postOffice = mapData[U"Budget.post_office"].get<int>();
	m_budget.education = mapData[U"Budget.education"].get<int>();
	
	m_tax.residential = mapData[U"Tax.residential"].get<int>();
	m_tax.commercial = mapData[U"Tax.commercial"].get<int>();
	m_tax.office = mapData[U"Tax.office"].get<int>();
	m_tax.industrial = mapData[U"Tax.industrial"].get<int>();
	m_tax.farm = mapData[U"Tax.farm"].get<int>();
	
	int y = 0;
	for (const auto& mapTiles : mapData[U"Map"].arrayView()) {
		m_tiles.push_back(Array<Tile>());
		int x = 0;
		for (const auto& tile : mapTiles.arrayView()) {
			m_tiles[y].push_back(Tile());
			
			m_tiles[y][x].tilesCount.x = tile[U"tiles_count.x"].get<int>();
			m_tiles[y][x].tilesCount.y = tile[U"tiles_count.y"].get<int>();
			
			for (const auto& jAddons : tile[U"addons"].arrayView()) {
				TypeID::Type type_id = typeNameToTypeID(jAddons[U"type_number"].getString());
				DirectionID::Type direction_id = directionNameToDirectionID(jAddons[U"direction_number"].getString());
				
				//tiles[y][x].category.push_back(j_addons[U"category"].getString());
				m_tiles[y][x].addType(type_id);
				m_tiles[y][x].addDirection(direction_id);
				
				// アドオンのポインタを登録
				if (m_addons.find(jAddons[U"name"].getString()) != m_addons.end()) {
					m_tiles[y][x].addons.push_back(m_addons[jAddons[U"name"].getString()]);
					
					// 0x0の位置でオブジェクトを生成しオブジェクトリストに追加
					if (tile[U"tiles_count.x"].get<int>() == 0 && tile[U"tiles_count.y"].get<int>() == 0) {
						m_objects << Object(m_addons[jAddons[U"name"].getString()], type_id, direction_id, CoordinateStruct{x, y});
					}
				}
				else {
					cout << "Cant't find " << jAddons[U"name"].getString() << endl;
				}
			}
			
			m_tiles[y][x].tilesCount.x = tile[U"tiles_count.x"].get<int>();
			m_tiles[y][x].tilesCount.y = tile[U"tiles_count.y"].get<int>();
			
			m_tiles[y][x].serialNumber = tile[U"serial_number"].get<int>();
			
			m_tiles[y][x].residents = tile[U"residents"].get<int>();
			
			m_tiles[y][x].workers.commercial = tile[U"workers.commercial"].get<int>();
			m_tiles[y][x].workers.office = tile[U"workers.office"].get<int>();
			m_tiles[y][x].workers.industrial = tile[U"workers.industrial"].get<int>();
			m_tiles[y][x].workers.farm = tile[U"workers.farm"].get<int>();
			m_tiles[y][x].workers.publicFacility = tile[U"workers.public"].get<int>();
			
			m_tiles[y][x].students = tile[U"students"].get<int>();
			
			m_tiles[y][x].happinessRate = tile[U"happiness_rate"].get<int>();
			
			// 各率の読み込み
			for (const auto& rate : tile[U"rate"].objectView()) {
				m_tiles[y][x].rate[rate.name] = rate.value.get<int>();
			}
			
			/*
			tiles[y][x].crop.name = square[U"crop.name"].getString();
			tiles[y][x].crop.amount = square[U"crop.amount"].get<int>();
			*/
			
			m_tiles[y][x].age = tile[U"age"].getArray<int>();
			
			m_tiles[y][x].gender = tile[U"gender"].getArray<String>();
			
			for (const auto& workPlaces : tile[U"work_places"].arrayView()) {
				m_tiles[y][x].workPlaces.push_back(WorkPlaceStruct());
				
				m_tiles[y][x].workPlaces.back().workPlace = getRCOIFP(workPlaces[U"work_kind"].get<int>());
				m_tiles[y][x].workPlaces.back().workPlacesSerialNumber = workPlaces[U"serial_number"].get<int>();
			}
			
			for (const auto& schools : tile[U"school"].arrayView()) {
				m_tiles[y][x].schools.push_back(SchoolStruct());
				
				m_tiles[y][x].schools.back().school = getSchool(schools[U"school_kind"].get<int>());
				m_tiles[y][x].schools.back().schoolSerialNumber = schools[U"serial_number"].get<int>();
			}
			
			m_tiles[y][x].reservation = getRCOIFP(tile[U"reservation"].get<int>());
			
			m_tiles[y][x].setOriginalName(tile[U"original_name"].getString());
			
			x++;
		}
		y++;
	}
}

void CityMap::loadAddons(String addonSetName) {
	//Array<FileStruct> addons_path = specific::getAllFilesName("./addons", "adat");
	Array<FileStruct> addonsPath = specific::getAllFilesName("./addons", "adj");
	
	for (int i=0; i<addonsPath.size(); i++) {
		cout << "from: " << addonsPath[i].file_path << endl;
		FileStruct fileTemp = addonsPath[i];
		
		Addon* loadingAddon = new Addon();
		if (loadingAddon->load(addonsPath[i], addonSetName)) {
			m_addons[loadingAddon->getName(NameMode::English)] = loadingAddon;
		}
		else {
			delete(loadingAddon);
		}
		
		System::Sleep(20);
	}
}

Array<Addon*> CityMap::getFitAddons(Array<String> selectedCategories) {
	Array<Addon*> retAddons;
	
	for (auto addon = m_addons.begin(); addon != m_addons.end(); addon++) {
		
		Array<String> anAddonCategories = addon->second->getCategories();
		vector<bool> fit = vector<bool>(selectedCategories.size(), false);
		
		for (auto categoryName = anAddonCategories.begin(); categoryName != anAddonCategories.end(); categoryName++) {
			for (int i=0; i<selectedCategories.size(); i++) {
				if (selectedCategories[i] == *categoryName) {
					fit[i] = true;
					break;
				}
			}
		}
		
		bool fitted = true;
		for (int i=0; i<fit.size(); i++) {
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

map<String, Addon*> CityMap::getAllAddons() {
	return m_addons;
}

void CityMap::drawTile(CoordinateStruct coordinate, CameraStruct camera) {
	// 描画する座標を算出
	for (int i=0; i<(int)m_tiles[coordinate.y][coordinate.x].addons.size(); i++) {
		int rate;
		Color rateColor = Color(0, 0, 0, 0);
		if (m_show_rate.length() > 0) {
			rate = getRate(coordinate, m_show_rate);
			if (m_show_rate == U"crime_rate") {
				rateColor = getRateColor(rate, false, 0);
			}
			else {
				rateColor = getRateColor(rate, true, 50);
			}
		}
		
		m_tiles[coordinate.y][coordinate.x].addons[i]->draw(m_tiles[coordinate.y][coordinate.x].getType(i), m_tiles[coordinate.y][coordinate.x].getDirection(i), coordinateToPosition(coordinate, camera), m_tiles[coordinate.y][coordinate.x].addons[i]->getUseTiles(m_tiles[coordinate.y][coordinate.x].getType(i), m_tiles[coordinate.y][coordinate.x].getDirection(i)), m_tiles[coordinate.y][coordinate.x].tilesCount, rateColor, m_time_now);
	}
}

void CityMap::draw(CameraStruct camera, CursorStruct& cursor) {
	// マップを描画
	for (short int y=getDrawArea(camera).first.y; y<getDrawArea(camera).second.y; y++) {
		for (short int x=getDrawArea(camera).first.x; x<getDrawArea(camera).second.x; x++) {
			PositionStruct drawPos = coordinateToPosition(CoordinateStruct{x, y}, camera);
			
			// 一マス分描画
			if (drawPos.x >= -CHIP_SIZE && drawPos.y >= -CHIP_SIZE/2 && drawPos.x <= Scene::Width() && drawPos.y <= Scene::Height() + CHIP_SIZE*2) {
				drawTile(CoordinateStruct{x, y}, camera);
			}
			
			// カーソルの描画
			if (x == cursor.coordinate.x && y == cursor.coordinate.y) {
				cursor.texture->draw(cursor.position_per_tiles.x, cursor.position_per_tiles.y, Alpha(128));
			}
		}
	}
}

Array<Addon> CityMap::getAddon(CoordinateStruct coordinate) {
	Array<Addon> retAddons;
	
	for (int i=0; i<m_tiles[coordinate.y][coordinate.x].addons.size(); i++) {
		retAddons << *(m_tiles[coordinate.y][coordinate.x].addons[i]);
	}
	
	return retAddons;
}

RCOIFstruct CityMap::getDemand() {
	return m_demand;
}

int CityMap::getPopulation() {
	return m_total_population;
}

int CityMap::getMoney() {
	return m_money;
}

int CityMap::getTemperature() {
	return m_temperature;
}

Size CityMap::getMapSize() {
	return m_map_size;
}

PositionStruct CityMap::coordinateToPosition(CoordinateStruct coordinate, CameraStruct camera) {
	// カメラの座標がデフォルト値（64*mapsize.width/2-Scene::Width()/2, 0）のときの描画位置を算出
	CameraStruct defaultCamera;
	defaultCamera.position = PositionStruct{0, 0};
	defaultCamera.center = {0, 0};
	
	// 基準点となるx:0, y:0のマスの表示位置を算出する
	PositionStruct tile0x0Position = PositionStruct{defaultCamera.position.x - camera.position.x,
		defaultCamera.position.y - camera.position.y};
	
	PositionStruct tilePosition = PositionStruct{tile0x0Position.x+coordinate.x*32-coordinate.y*32, tile0x0Position.y+coordinate.y*16+coordinate.x*16};
	
	return tilePosition;
}

CoordinateStruct CityMap::positionToCoordinate(PositionStruct position, CameraStruct camera) {
	int tempMX = position.x+camera.position.x-CHIP_SIZE/2;
	int tempMY = position.y+camera.position.y;
	
	CoordinateStruct ret;
	ret.x = (tempMY + tempMX/2) / (CHIP_SIZE/2);
	ret.y = (-tempMX + tempMY*2) / CHIP_SIZE;
	
	if (ret.x < 0) {
		ret.x = 0;
	}
	if (ret.y < 0) {
		ret.y = 0;
	}
	
	if (ret.x >= m_map_size.x) {
		ret.x = m_map_size.x-1;
	}
	if (ret.y >= m_map_size.y) {
		ret.y = m_map_size.y-1;
	}
	
	return ret;
}

pair<CoordinateStruct, CoordinateStruct> CityMap::getDrawArea(CameraStruct camera) {
	// カメラの座標が変わっていなければrangeを返す
	if (camera.position.x == m_camera_before.position.x && camera.position.y == m_camera_before.position.y) {
		return m_range;
	}
	m_camera_before = camera;
	
	// 描画できる範囲
	int rangeOneDirection = (sqrt(powf(Scene::Width(), 2)+powf(Scene::Height(), 2))/sqrt(powf(CHIP_SIZE/2, 2)+powf(CHIP_SIZE/4, 2)));
	
	// 画面中央の座標
	CoordinateStruct centerCoordinate = positionToCoordinate(PositionStruct{Scene::Width()/2, Scene::Height()/2}, camera);
	
	pair<CoordinateStruct, CoordinateStruct> ret;
	// 左上の座標
	ret.first = CoordinateStruct{centerCoordinate.x-rangeOneDirection, centerCoordinate.y-rangeOneDirection};
	
	if (ret.first.x < 0) {
		ret.first.x = 0;
	}
	if (ret.first.y < 0) {
		ret.first.y = 0;
	}
	if (ret.first.x >= m_map_size.x) {
		ret.first.x = m_map_size.x-1;
	}
	if (ret.first.y >= m_map_size.y) {
		ret.first.y = m_map_size.y-1;
	}
	
	// 右下の座標
	ret.second = CoordinateStruct{centerCoordinate.x+rangeOneDirection, centerCoordinate.y+rangeOneDirection};
	
	if (ret.second.x < 0) {
		ret.second.x = 0;
	}
	if (ret.second.y < 0) {
		ret.second.y = 0;
	}
	if (ret.second.x >= m_map_size.x) {
		ret.second.x = m_map_size.x-1;
	}
	if (ret.second.y >= m_map_size.y) {
		ret.second.y = m_map_size.y-1;
	}
	
	m_range = ret;
	
	return ret;
}

// いずれかのアドオンがカテゴリに含まれているか
bool CityMap::isInCategories(String searchCategory, CoordinateStruct coordinate) {
	Tile* currentTile = &m_tiles[coordinate.y][coordinate.x];
	
	for (int i=0; i<currentTile->addons.size(); i++) {
		if (currentTile->addons[i]->isInCategories(searchCategory)) {
			return true;
		}
	}
	
	return false;
}

// アドオンの設置
bool CityMap::build(CoordinateStruct position, Addon* selectedAddon, bool needToBreak) {
	Tile* currentTile = &m_tiles[position.y][position.x];
	
	TypeID::Type type;
	DirectionID::Type direction;
	
	Array<CoordinateStruct> needUpdate;
	if (getBuildTypeAndDirection(position, selectedAddon, type, direction, needUpdate)) {
		CoordinateStruct useTiles = selectedAddon->getUseTiles(type, direction);
		
		if (direction == DirectionID::Left) {
			position.y += useTiles.y-1;
		}
		if (direction == DirectionID::Right) {
			position.x -= useTiles.x-1;
			position.y += useTiles.y-1;
		}
		else if (direction == DirectionID::Top) {
			position.y += useTiles.y-1;
		}
		
		// 効果を取得
		map<String, EffectStruct> effects = selectedAddon->getEffects();
		
		// 中央となる座標を取得
		int centerX = useTiles.x/2;
		int centerY = useTiles.y/2;
		
		for (int y=0; abs(y)<useTiles.y; y--) {
			for (int x=0; abs(x)<useTiles.x; x++) {
				if (needToBreak && type != TypeID::UnderConstruction && type != TypeID::Bridge) {
					breaking(CoordinateStruct{position.x+x, position.y+y});
				}
				
				currentTile = &m_tiles[position.y+y][position.x+x];
				
				if (type != TypeID::TrainCrossing && type != TypeID::Bridge) {
					currentTile->clearAll();
				}
				
				currentTile->addType(type);
				currentTile->addDirection(direction);
				
				currentTile->tilesCount = {abs(x), abs(y)};
				
				currentTile->addons << selectedAddon;
				
				// 効果を地図へ反映
				if (abs(x) == centerX && abs(y) == centerY) {
					for (auto effect = effects.begin(); effect != effects.end(); effect++) {
						double effectPerGrid = effect->second.influence / effect->second.grid;
						for (int ey=-effect->second.grid; ey<=effect->second.grid; ey++) {
							for (int ex=-effect->second.grid; ex<=effect->second.grid; ex++) {
								if (isPositionAvailable(CoordinateStruct{position.x+x+ex, position.y+y+ey})) {
									m_tiles[position.y+y+ey][position.x+x+ex].rate[effect->first] += effectPerGrid*max(abs(effect->second.grid-1-ey), abs(effect->second.grid-1-ex));
								}
							}
						}
					}
				}
			}
		}
		
		// (道路などで)周囲のアドオンの修正が必要な場合は修正する
		if (needUpdate.size() > 0) {
			Array<String> searchCategories = {U"road", U"train", U"waterway"};
			
			for (int i=0; i<needUpdate.size(); i++) {
				
				if (needUpdate[i].x < 0 || needUpdate[i].y < 0) {
					continue;
				}
				
				for (int j=0; j<m_tiles[needUpdate[i].y][needUpdate[i].x].addons.size(); j++) {
					if (m_tiles[needUpdate[i].y][needUpdate[i].x].addons[j]->isInCategories(searchCategories)) {
						if (!(needUpdate[j].x == -1 && needUpdate[j].y == -1)) {
							cout << "update for " << needUpdate[i].x << "," << needUpdate[i].y << " " << needUpdate.size() << endl;
							update(needUpdate[i], m_tiles[needUpdate[i].y][needUpdate[i].x].addons[j], needUpdate);
						}
					}
				}
			}
		}
	}
	
	return true;
}

void CityMap::update(CoordinateStruct position, Addon* selectedAddon, Array<CoordinateStruct>& needUpdate) {
	Tile* currentTile = &m_tiles[position.y][position.x];
	
	// 踏切と橋の場合は更新不要
	for (int i=0; i<currentTile->getTypes().size(); i++) {
		if (currentTile->getType(i) == TypeID::UnderConstruction || currentTile->getType(i) == TypeID::Bridge) {
			return;
		}
	}
	
	TypeID::Type type;
	DirectionID::Type direction;
	if (getBuildTypeAndDirection(position, selectedAddon, type, direction, needUpdate)) {
		currentTile->clearAddons();						// 一旦クリアしてもう一度設置
		
		currentTile->addType(type);
		currentTile->addDirection(direction);
		currentTile->addons << selectedAddon;
	}
}

void CityMap::breaking(CoordinateStruct coordinate) {
	Tile* currentTile = &m_tiles[coordinate.y][coordinate.x];
	Array<Addon*> breakAddons = currentTile->addons;
	
	for (int i=0; i<breakAddons.size(); i++) {
		currentTile = &m_tiles[coordinate.y][coordinate.x];
		CoordinateStruct useTiles = breakAddons[i]->getUseTiles(currentTile->getType(i), currentTile->getDirection(i));
		
		CoordinateStruct startPoint = moveToAddonStartTile(coordinate, i);
		currentTile = &m_tiles[startPoint.y][startPoint.x];
		
		// 効果を取得
		map<String, EffectStruct> effects = breakAddons[i]->getEffects();
		
		// 中央となる座標を取得
		int centerX = useTiles.x/2;
		int centerY = useTiles.y/2;
		
		for (int y=0; abs(y)<useTiles.y; y--) {
			for (int x=0; x<useTiles.x; x++) {
				Tile beforeBreak = m_tiles[startPoint.y+y][startPoint.x+x];
				cout << "from breaking: " << endl;
				build(CoordinateStruct{startPoint.x+x, startPoint.y+y}, m_addons[U"tile_greenfield"], false);
				
				// 効果を除去
				if (abs(x) == centerX && abs(y) == centerY) {
					for (auto effect = effects.begin(); effect != effects.end(); effect++) {
						double effectPerGrid = effect->second.influence / effect->second.grid;
						for (int ey=-effect->second.grid; ey<=effect->second.grid; ey++) {
							for (int ex=-effect->second.grid; ex<=effect->second.grid; ex++) {
								if (isPositionAvailable(CoordinateStruct{startPoint.x+x+ex, startPoint.y+y+ey})) {
									m_tiles[startPoint.y+y+ey][startPoint.x+x+ex].rate[effect->first] -= effectPerGrid*max(abs(effect->second.grid-1-ey), abs(effect->second.grid-1-ex));
								}
							}
						}
					}
				}
				
				if (beforeBreak.addons[i]->isInCategories(U"connectable_type")) {
					Array<CoordinateStruct> needUpdate;
					
					for (int j=0; j<AROUND_TILES; j++) {
						for (int k=0; k<m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons.size(); k++) {
							if ((beforeBreak.addons[i]->isInCategories(U"road") && m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"road")) ||
								(beforeBreak.addons[i]->isInCategories(U"train") && m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"train")) ||
								(beforeBreak.addons[i]->isInCategories(U"waterway") && m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"waterway")) ||
								(beforeBreak.addons[i]->isInCategories(U"airport") && m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"airport"))) {
								needUpdate << CoordinateStruct{startPoint.x+x+AroundTiles[j].second.x, startPoint.y+y+AroundTiles[j].second.y};
							}
						}
					}
					
					if (breakAddons[i]->isInCategories(U"road")) {
						for (int j=0; j<needUpdate.size(); j++) {
							if (m_tiles[needUpdate[j].y][needUpdate[j].x].addons[i]->isInCategories(U"railroad")) {
								needUpdate[j] = {-1, -1};
							}
						}
					}
					
					for (int j=0; j<needUpdate.size(); j++) {
						for (int k=0; k<m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons.size(); k++) {
							if (!(needUpdate[j].x == -1 && needUpdate[j].y == -1)) {
								cout << "breaking update: " << needUpdate[j].x << "," << needUpdate[j].y << endl;
								update(needUpdate[j], m_tiles[needUpdate[j].y][needUpdate[j].x].addons[k], needUpdate);
							}
						}
					}
					
				}
			}
		}
	}
}

CoordinateStruct CityMap::moveToAddonStartTile(CoordinateStruct searchCoordinate, int addonNumber) {
	Tile* searchTile = &m_tiles[searchCoordinate.y][searchCoordinate.x];
	
	searchCoordinate.x -= searchTile->tilesCount.x;
	searchCoordinate.y += searchTile->tilesCount.y;
	
	return CoordinateStruct{searchCoordinate.x, searchCoordinate.y};
}

// 設置する場所に合うTypeとDirectionを取得
bool CityMap::getBuildTypeAndDirection(CoordinateStruct coordinate, Addon* selectedAddon, TypeID::Type& retType, DirectionID::Type& retDirection,
									   Array<CoordinateStruct>& needUpdate) {
	// 接続タイプ（道路など）アドオンの場合
	if (selectedAddon->isInCategories(U"connectable_type")) {
		// 周囲に道路があるか（建設可能か）確認する
		int totalAroundRoad = 0;
		Array<pair<DirectionID::Type, CoordinateStruct>> aroundRoadCoordinate;
		
		bool needUpdateMore = true;
		if (needUpdate.size() > 0) {
			needUpdateMore = false;
		}
		
		for (int i=0; i<AROUND_TILES; i++) {
			CoordinateStruct currentTile = {coordinate.x+AroundTiles[i].second.x, coordinate.y+AroundTiles[i].second.y};
			
			if (currentTile.x < 0 || currentTile.y < 0 || currentTile.x >= m_map_size.x || currentTile.y >= m_map_size.y) {
				continue;
			}
			
			for (int j=0; j<m_tiles[currentTile.y][currentTile.x].addons.size(); j++) {
				// 道路の場合
				if ((selectedAddon->isInCategories(U"road") && m_tiles[currentTile.y][currentTile.x].addons[j]->isInCategories(U"road")) ||
					(selectedAddon->isInCategories(U"train") && m_tiles[currentTile.y][currentTile.x].addons[j]->isInCategories(U"train")) ||
					(selectedAddon->isInCategories(U"waterway") && m_tiles[currentTile.y][currentTile.x].addons[j]->isInCategories(U"waterway")) ||
					(selectedAddon->isInCategories(U"airport") && m_tiles[currentTile.y][currentTile.x].addons[j]->isInCategories(U"airport"))) {
					totalAroundRoad ++;
					aroundRoadCoordinate << AroundTiles[i];
					
					if (needUpdateMore) {
						needUpdate << currentTile;
					}
					
					break;
				}
			}
		}
		
		// 踏切を設置する必要がある場合
		if (selectedAddon->isInCategories(U"road")) {
			for (int i=0; i<m_tiles[coordinate.y][coordinate.x].addons.size(); i++) {
				if (m_tiles[coordinate.y][coordinate.x].addons[i]->isInCategories(U"railroad")) {
					retType = TypeID::TrainCrossing;
					
					if (m_tiles[coordinate.y][coordinate.x].getDirection(i) == DirectionID::Width) {
						retDirection = DirectionID::Width;
					}
					else {
						retDirection = DirectionID::Depth;
					}
					
					for (int j=0; j<needUpdate.size(); j++) {
						if (m_tiles[needUpdate[j].y][needUpdate[j].x].addons[i]->isInCategories(U"railroad")) {
							needUpdate[j] = {-1, -1};
						}
					}
					
					return true;
				}
			}
		}
		
		// 橋を設置する必要がある場合
		if (selectedAddon->isInCategories(U"road")) {
			for (int i=0; i<m_tiles[coordinate.y][coordinate.x].addons.size(); i++) {
				if (m_tiles[coordinate.y][coordinate.x].addons[i]->isInCategories(U"waterway")) {
					retType = TypeID::Bridge;
					
					if (m_tiles[coordinate.y][coordinate.x].getDirection(i) == DirectionID::Width) {
						retDirection = DirectionID::Depth;
					}
					else {
						retDirection = DirectionID::Width;
					}
					
					for (int j=0; j<needUpdate.size(); j++) {
						if (m_tiles[needUpdate[j].y][needUpdate[j].x].addons[i]->isInCategories(U"waterway")) {
							needUpdate[j] = {-1, -1};
						}
					}
					
					return true;
				}
			}
		}
		
		// その他
		if (totalAroundRoad == 0) {
			retType = TypeID::IntersectionCross;
			retDirection = DirectionID::Normal;
			return true;
		}
		if (totalAroundRoad == 1) {
			retType = TypeID::DeadEnd;
			retDirection = aroundRoadCoordinate[0].first;
			return true;
		}
		if (totalAroundRoad == 2) {
			CoordinateStruct roadDelta = {0, 0};
			
			for (int i=0; i<2; i++) {
				roadDelta.x += aroundRoadCoordinate[i].second.x;
				roadDelta.y += aroundRoadCoordinate[i].second.y;
			}
			
			// 縦横方向
			if (roadDelta.x == 0 && roadDelta.y == 0) {
				retType = TypeID::Default;
				
				if (aroundRoadCoordinate[0].second.x != 0) {
					retDirection = DirectionID::Depth;
				}
				else {
					retDirection = DirectionID::Width;
				}
				
				return true;
			}
			// 曲がり角
			else {
				retType = TypeID::Turn;
				
				// DirectionIDを取得して設定
				int direction_id_temp = 0;
				for(int i=0; i<aroundRoadCoordinate.size(); i++) {
					direction_id_temp += (int)aroundRoadCoordinate[i].first;
				}
				retDirection = (DirectionID::Type)direction_id_temp;
				
				return true;
			}
		}
		if (totalAroundRoad == 3) {
			retType = TypeID::IntersectionT;
			
			// DirectionIDを取得して設定
			int direction_id_temp = 0;
			for(int i=0; i<aroundRoadCoordinate.size(); i++) {
				direction_id_temp += (int)aroundRoadCoordinate[i].first;
			}
			retDirection = (DirectionID::Type)direction_id_temp;
			
			return true;
		}
		if (totalAroundRoad == 4) {
			retType = TypeID::IntersectionCross;
			retDirection = DirectionID::Normal;
			return true;
		}
	}
	
	// オブジェクトタイプの場合
	if (selectedAddon->isInCategories(U"object_type")) {
		// 周囲に道路があるか確認する
		for (int i=0; i<AROUND_TILES; i++) {
			CoordinateStruct currentTile = {coordinate.x+AroundTiles[i].second.x, coordinate.y+AroundTiles[i].second.y};
			
			for (int j=0; j<m_tiles[currentTile.y][currentTile.x].addons.size(); j++) {
				if (m_tiles[currentTile.y][currentTile.x].addons[j]->isInCategories(U"road")) {
					TypeID::Type retTypeTemp = TypeID::Normal;
					DirectionID::Type retDirectionTemp = AroundTiles[i].first;
					
					// 複数のタイルを使う場合、建てる方向に障害物などがないか確認する
					bool cannotBuild = false;
					
					int addX = 1, addY = 1;
					if (retDirectionTemp == DirectionID::Right) {
						addX = -1;
					}
					else if (retDirectionTemp == DirectionID::Bottom) {
						addY = -1;
					}
					
					for (int y=0; y<selectedAddon->getUseTiles(retType, retDirection).y; y+=addY) {
						for (int x=0; y<selectedAddon->getUseTiles(retType, retDirection).x; x+=addX) {
							for (int k=0; k<m_tiles[y][x].addons.size(); k++) {
								if (m_tiles[y][x].addons[k]->isInCategories(U"connectable_type")) {
									cannotBuild = true;
									break;
								}
							}
							if (cannotBuild) break;
						}
						if (cannotBuild) break;
					}
					
					if (!cannotBuild) {
						retType = retTypeTemp;
						retDirection = retDirectionTemp;
						return true;
					}
				}
			}
		}
	}
	
	// タイルの場合
	if (selectedAddon->isInCategories(U"put_type")) {
		retType = TypeID::Normal;
		retDirection = DirectionID::Normal;
		return true;
	}
	
	return false;					// 存在しない or 設置不可能な場合
}

// アドオンを削除
void CityMap::clear(CoordinateStruct position) {
	Tile* currentTile = &m_tiles[position.y][position.x];
	Addon* selectedAddon = m_addons[U"tile_greenfield"];
	
	currentTile->clearAll();
	
	currentTile->addType(TypeID::Normal);
	currentTile->addDirection(DirectionID::Normal);
	currentTile->addons << selectedAddon;
	
	// 幸福度を戻す
}

bool CityMap::isPositionAvailable(CoordinateStruct coordinate) {
	if (coordinate.x >= 0 && coordinate.x <= m_map_size.x-1 && coordinate.y >= 0 && coordinate.y <= m_map_size.y-1) {
		return true;
	}
	else {
		return false;
	}
}

TimeStruct CityMap::cityTime(int minutesDelta) {
	m_time_now.minutes += minutesDelta;
	
	if (m_time_now.minutes >= 60) {
		m_time_now.hour ++;
		m_time_now.minutes -= 60;
		
		if (m_time_now.hour >= 24) {
			m_time_now.date ++;
			m_time_now.hour -= 24;
			
			if (m_time_now.month == 4 || m_time_now.month == 6 || m_time_now.month == 9 || m_time_now.month == 11) {
				if (m_time_now.date >= 31) {
					m_time_now.month ++;
					m_time_now.date -= 30;
				}
			}
			else if (m_time_now.month == 2) {
				if (m_time_now.year % 4 == 0 && m_time_now.date >= 30) {
					m_time_now.month ++;
					m_time_now.date -= 29;
				}
				else if (m_time_now.date >= 29) {
					m_time_now.month ++;
					m_time_now.date -= 28;
				}
			}
			else if (m_time_now.date >= 32) {
				m_time_now.month ++;
				m_time_now.date -= 31;
				
				if (m_time_now.month >= 13) {
					m_time_now.year ++;
					m_time_now.month -= 12;
				}
			}
		}
	}
	
	
	return m_time_now;
}

map<String, int> CityMap::getRate(CoordinateStruct coordinate) {
	return m_tiles[coordinate.y][coordinate.x].rate;
}

int CityMap::getRate(CoordinateStruct coordinate, String rateName) {
	if (m_tiles[coordinate.y][coordinate.x].rate.find(rateName) != m_tiles[coordinate.y][coordinate.x].rate.end()) {
		return m_tiles[coordinate.y][coordinate.x].rate[rateName];
	}
	else {
		return 0;
	}
}

Color CityMap::getRateColor(int rate, bool upper, int standard) {
	Color ret(50, 50, 50);
	
	if (upper) {
		if (rate > standard) {
			ret.b += (rate-standard) * 1.27;
		}
		else if (rate < standard) {
			ret.r += (standard-rate) * 1.27;
		}
		return ret;
	}
	
	if (rate < standard) {
		ret.b += (standard-rate) * 1.27;
	}
	else if (rate > standard) {
		ret.r += (rate-standard) * 1.27;
	}
	
	return ret;
}

void CityMap::setShowRate(String rateName) {
	m_show_rate = rateName;
}

bool CityMap::save() {
	// JsonWriterを宣言
	JSONWriter mapData;
	
	mapData.startObject();
	{
		mapData.key(U"Version").write(RELEASE_NUMBER);
		mapData.key(U"Addon_Set").write(m_addon_set_name);
		mapData.key(U"City_Name").write(m_city_name);
		mapData.key(U"Mayor_Name").write(m_mayor_name);
		mapData.key(U"Total_Population").write(m_total_population);
		mapData.key(U"Change_Weather").write(m_change_weather);
		mapData.key(U"Temperature").write(m_temperature);
		mapData.key(U"Dark_on_Night").write(m_dark_on_night);
		
		mapData.key(U"Map_size").startObject();
		{
			mapData.key(U"width").write(m_map_size.x);
			mapData.key(U"height").write(m_map_size.y);
		}
		mapData.endObject();
		
		mapData.key(U"Time").startObject();
		{
			mapData.key(U"year").write(m_time_now.year);
			mapData.key(U"month").write(m_time_now.month);
			mapData.key(U"date").write(m_time_now.date);
			mapData.key(U"hour").write(m_time_now.hour);
			mapData.key(U"minutes").write(m_time_now.minutes);
		}
		mapData.endObject();
		
		mapData.key(U"Demand").startObject();
		{
			mapData.key(U"residential").write(m_demand.residential);
			mapData.key(U"commercial").write(m_demand.commercial);
			mapData.key(U"office").write(m_demand.office);
			mapData.key(U"industrial").write(m_demand.industrial);
			mapData.key(U"farm").write(m_demand.farm);
		}
		mapData.endObject();
		
		mapData.key(U"Money").write(m_money);
		
		mapData.key(U"Budget").startObject();
		{
			mapData.key(U"police").write(m_budget.police);
			mapData.key(U"fire_depertment").write(m_budget.fireDepertment);
			mapData.key(U"post_office").write(m_budget.postOffice);
			mapData.key(U"education").write(m_budget.education);
		}
		mapData.endObject();
		
		mapData.key(U"Tax").startObject();
		{
			mapData.key(U"residential").write(m_tax.residential);
			mapData.key(U"commercial").write(m_tax.commercial);
			mapData.key(U"office").write(m_tax.office);
			mapData.key(U"industrial").write(m_tax.industrial);
			mapData.key(U"farm").write(m_tax.farm);
		}
		mapData.endObject();
		
		mapData.key(U"Map").startArray();
		{
			for (int y=0; y<m_map_size.y; y++) {
				mapData.startArray();
				{
					for (int x=0; x<m_map_size.x; x++) {
						mapData.startObject();
						{
							mapData.key(U"addons").startArray();
							{
								for (int i=0; i<m_tiles[y][x].addons.size(); i++) {
									mapData.startObject();
									{
										mapData.key(U"name").write(m_tiles[y][x].addons[i]->getName(NameMode::English));
										//map_file.key(U"category").write(tiles[y][x].category[i]);
										mapData.key(U"type_number").write(m_tiles[y][x].getType(i));
										mapData.key(U"direction_number").write(m_tiles[y][x].getDirection(i));
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							/*
							map_file.key(U"use_tiles").startObject();
							{
								map_file.key(U"x").write(tiles[y][x].use_tiles.x);
								map_file.key(U"y").write(tiles[y][x].use_tiles.y);
							}
							map_file.endObject();
							*/
							mapData.key(U"tiles_count").startObject();
							{
								mapData.key(U"x").write(m_tiles[y][x].tilesCount.x);
								mapData.key(U"y").write(m_tiles[y][x].tilesCount.y);
							}
							mapData.endObject();
							
							mapData.key(U"serial_number").write(m_tiles[y][x].serialNumber);
							
							mapData.key(U"residents").write(m_tiles[y][x].residents);
							
							mapData.key(U"workers").startObject();
							{
								mapData.key(U"commercial").write(m_tiles[y][x].workers.commercial);
								mapData.key(U"office").write(m_tiles[y][x].workers.office);
								mapData.key(U"industrial").write(m_tiles[y][x].workers.industrial);
								mapData.key(U"farm").write(m_tiles[y][x].workers.farm);
								mapData.key(U"public").write(m_tiles[y][x].workers.publicFacility);
							}
							mapData.endObject();
							
							mapData.key(U"students").write(m_tiles[y][x].students);
							
							mapData.key(U"rate").startObject();
							{
								for (auto rate = m_tiles[y][x].rate.begin(); rate != m_tiles[y][x].rate.end() ; rate++) {
									if (rate->second != 0) {
										mapData.key(rate->first).write(rate->second);
									}
								}
							}
							mapData.endObject();
							
							/*
							map_file.key(U"crop").startObject();
							{
								map_file.key(U"name").write(tiles[y][x].crop.name);
								map_file.key(U"amount").write(tiles[y][x].crop.amount);
							}
							map_file.endObject();
							 */
							
							mapData.key(U"age").startArray();
							{
								for (int i=0; i<m_tiles[y][x].age.size(); i++) {
									mapData.write(m_tiles[y][x].age[i]);
								}
							}
							mapData.endArray();
							
							mapData.key(U"gender").startArray();
							{
								for (int i=0; i<m_tiles[y][x].gender.size(); i++) {
									mapData.write(m_tiles[y][x].gender[i]);
								}
							}
							mapData.endArray();
							
							mapData.key(U"work_places").startArray();
							{
								for (int i=0; i<m_tiles[y][x].workPlaces.size(); i++) {
									mapData.startObject();
									{
										mapData.key(U"work_kind").write(m_tiles[y][x].workPlaces[i].workPlace);
										mapData.key(U"serial_number").write(m_tiles[y][x].workPlaces[i].workPlacesSerialNumber);
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							
							mapData.key(U"school").startArray();
							{
								for (int i=0; i<m_tiles[y][x].schools.size(); i++) {
									mapData.startObject();
									{
										mapData.key(U"school_kind").write(m_tiles[y][x].schools[i].school);
										mapData.key(U"serial_number").write(m_tiles[y][x].schools[i].schoolSerialNumber);
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							
							mapData.key(U"reservation").write(m_tiles[y][x].reservation);
							
							mapData.key(U"original_name").write(m_tiles[y][x].getOriginalName());
						}
						mapData.endObject();
					}
				}
				mapData.endArray();
			}
		}
		mapData.endArray();
	}
	mapData.endObject();
	
	saveTextFile(m_map_file_path.toUTF8(), stringXOR(mapData.get().toUTF8(), "citiesboxmapdatafilexor"));
	//saveTextFile(map_file_path.toUTF8()+".cbj", map_file.get().toUTF8());
	//map_file.save(map_file_path+U".cbj");
	
	return true;
}

void CityMap::freeMapAndAddons() {
	for (auto i = m_addons.begin(); i != m_addons.end() ; i++) {
		delete(i->second);
	}
	
	Array<Array<Tile>>().swap(m_tiles);
}
