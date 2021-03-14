//
//  CityMap.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "CityMap.hpp"


bool CityMap::getElement(String str, String searchElementName, String& ret) {
	string strUTF8 = str.toUTF8();
	string searchElementNameUTF8 = searchElementName.toUTF8();
	
	if (strUTF8.find(searchElementNameUTF8) != string::npos && strUTF8.find("=") != string::npos) {
		ret = Unicode::Widen(strUTF8.substr(strUTF8.find("\"") + 1, strUTF8.find(";") - (strUTF8.find("\"") + 2)));
		return true;
	}
	return false;
}

bool CityMap::getElement(String str, String searchElementName, int& ret) {
	string strUTF8 = str.toUTF8();
	string searchElementNameUTF8 = searchElementName.toUTF8();
	
	if (strUTF8.find(searchElementNameUTF8) != string::npos && strUTF8.find("=") != string::npos) {
		ret = stoi(strUTF8.substr(strUTF8.find("= ") + 2, strUTF8.find(";") - (strUTF8.find("= ") + 2)));
		return true;
	}
	return false;
}

bool CityMap::getElement(String str, String searchElementName, bool& ret) {
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

bool CityMap::getTypes(String str, String searchElementName, Array<String>& ret) {
	String aRet;
	if (getElement(str, searchElementName, aRet)) {
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
	mapFilePath = loadMapFilePath;
	
	TextReader mapData(mapFilePath);
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
	
	mapsize.width = -1;
	mapsize.height = -1;
	
	bool addonLoaded = false;
	
	while (mapData.readLine(strTemp)) {
		strTemp = strTemp.substr(0, strTemp.length()-LINE_FEED_CODE);				// 改行コードは除く
		strTempUTF8 = strTemp.toUTF8();
		
		getElement(strTemp, U"Version", savedVersion);
		getElement(strTemp, U"Addons_Set", addonSet);
		
		// アドオン読み込み
		if (!addonLoaded && addonSet.length() > 0) {
			loadAddons(addonSet);
			addonLoaded = true;
		}
		
		getElement(strTemp, U"City_Name", cityName);
		getElement(strTemp, U"Mayor_Name", mayorName);
		getElement(strTemp, U"Total_Population", totalPopulation);
		getElement(strTemp, U"Set_Change_Weather", changeWeather);
		getElement(strTemp, U"Temperature", temperature);
		getElement(strTemp, U"Set_Dark_on_Night", darkOnNight);
		
		getElement(strTemp, U"mapsize_x", mapsize.width);
		getElement(strTemp, U"mapsize_y", mapsize.height);
		
		getElement(strTemp, U"time_Year", time.year);
		getElement(strTemp, U"time_Month", time.month);
		getElement(strTemp, U"time_Date", time.date);
		getElement(strTemp, U"time_Hour", time.hour);
		getElement(strTemp, U"time_Minutes", time.minutes);
		
		getElement(strTemp, U"demand_Residential", demand.residential);
		getElement(strTemp, U"demand_Commercial", demand.commercial);
		getElement(strTemp, U"demand_Office", demand.office);
		getElement(strTemp, U"demand_Industrial", demand.industrial);
		getElement(strTemp, U"demand_Farm", demand.farm);
		
		getElement(strTemp, U"Money", money);
		
		getElement(strTemp, U"budget_of_Police", budget.police);
		getElement(strTemp, U"budget_of_Fire_Depertment", budget.fireDepertment);
		getElement(strTemp, U"budget_of_Post_Office", budget.postOffice);
		getElement(strTemp, U"budget_of_Education", budget.education);
		
		getElement(strTemp, U"tax_of_Residential", tax.residential);
		getElement(strTemp, U"tax_of_Commercial", tax.commercial);
		getElement(strTemp, U"tax_of_Office", tax.office);
		getElement(strTemp, U"tax_of_Industrial", tax.industrial);
		getElement(strTemp, U"tax_of_Farm", tax.farm);
		
		if (!mapCleared && mapsize.width > 0 && mapsize.height > 0) {
			for (int y=0; y<mapsize.height; y++) {
				Tile new_tile;
				tiles.push_back(Array<Tile>(mapsize.width, new_tile));
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
			
			for (int x=0; x<mapsize.width; x++) {
				// マップにAddon_Setが定義されていない場合はNormalとみなしアドオン読み込み
				if (!addonLoaded) {
					addonSet = U"Normal";
					loadAddons(addonSet);
					addonLoaded = true;
				}
				
				// アドオンのポインタを登録
				if (addons.find(temp[x]) != addons.end()) {
					tiles[arrayCount][x].addons << addons[temp[x]];
					
					//tiles[array_count][x].category = tiles[array_count][x].addons.back()->getCategories();
				}
			}
		}
		
		if (currentArrayName == "name2" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (temp[x].length() == 0) {
					continue;
				}
				
				// アドオンのポインタを登録
				if (addons.find(temp[x]) != addons.end()) {
					Addon* addon_temp = tiles[arrayCount][x].addons[0];
					tiles[arrayCount][x].addons.back() = addons[temp[x]];
					tiles[arrayCount][x].addons << addon_temp;
					
					cout << tiles[arrayCount][x].addons[1]->getName(NameMode::English) << endl;
					
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
			
			for (int x=0; x<mapsize.width; x++) {
				if (tiles[arrayCount][x].addons.size() == 2) {
					tiles[arrayCount][x].addType(tiles[arrayCount][x].addons[1]->getTypeName(stoi(temp[x].toUTF8())));	// とりあえず同じものを[0]にも入れておく
					tiles[arrayCount][x].addType(tiles[arrayCount][x].addons[1]->getTypeName(stoi(temp[x].toUTF8())));
				}
				else {
					tiles[arrayCount][x].addType(tiles[arrayCount][x].addons[0]->getTypeName(stoi(temp[x].toUTF8())));
				}
			}
		}
		
		if (currentArrayName == "obj_type2" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (tiles[arrayCount][x].addons.size() == 2) {
					tiles[arrayCount][x].getType(0) = tiles[arrayCount][x].addons[0]->getTypeName(stoi(temp[x].toUTF8()));
				}
			}
		}
		
		if (currentArrayName == "obj_dire" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (tiles[arrayCount][x].addons.size() == 2) {
					tiles[arrayCount][x].addDirection(tiles[arrayCount][x].addons[1]->getDirectionName(tiles[arrayCount][x].getType(1), stoi(temp[x].toUTF8())));	// とりあえず同じものを[0]にも入れておく
					tiles[arrayCount][x].addDirection(tiles[arrayCount][x].addons[1]->getDirectionName(tiles[arrayCount][x].getType(1), stoi(temp[x].toUTF8())));
				}
				else {
					tiles[arrayCount][x].addDirection(tiles[arrayCount][x].addons[0]->getDirectionName(tiles[arrayCount][x].getType(0), stoi(temp[x].toUTF8())));
				}
			}
		}
		
		if (currentArrayName == "obj_dire2" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (tiles[arrayCount][x].addons.size() == 2) {
					tiles[arrayCount][x].getDirection(0) = tiles[arrayCount][x].addons[0]->getDirectionName(tiles[arrayCount][x].getType(0), stoi(temp[x].toUTF8()));
				}
			}
		}
		
		if (currentArrayName == "obj_serial_num" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].serialNumber = stoi(temp[x].toUTF8());
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
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].tilesCount.x = stoi(temp[x].toUTF8());
				
				// obj_tiles_xを修正(r140以前のバージョンで保存した場合)
				if (savedVersion <= 140) {
					if (tiles[arrayCount][x].addons[0]->getUseTiles(tiles[arrayCount][x].getType(0), tiles[arrayCount][x].getDirection(0)).x > 0) {
						// 左向き
						if (tiles[arrayCount][x].getDirection(0) == U"left") {
							
						}
						
						// 上向き
						if (tiles[arrayCount][x].getDirection(0) == U"top") {
							
						}
						
						// 下向き
						if (tiles[arrayCount][x].getDirection(0) == U"bottom") {
							
						}
						
						// 右向き
						if (tiles[arrayCount][x].getDirection(0) == U"right") {
							tiles[arrayCount][x].tilesCount.x += tiles[arrayCount][x].addons[0]->getUseTiles(tiles[arrayCount][x].getType(0), tiles[arrayCount][x].getDirection(0)).x - 1;
						}
					}
				}
			}
		}
		
		if (currentArrayName == "obj_tiles_y" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].tilesCount.y = stoi(temp[x].toUTF8());
				
				// obj_tiles_yを修正(r140以前のバージョンで保存した場合)
				if (savedVersion <= 140) {
					if (tiles[arrayCount][x].addons[0]->getUseTiles(tiles[arrayCount][x].getType(0), tiles[arrayCount][x].getDirection(0)).y > 0) {
						// 左向き
						if (tiles[arrayCount][x].getDirection(0) == U"left") {
							
						}
						
						// 上向き
						if (tiles[arrayCount][x].getDirection(0) == U"top") {
							tiles[arrayCount][x].tilesCount.y = tiles[arrayCount][x].addons[0]->getUseTiles(tiles[arrayCount][x].getType(0), tiles[arrayCount][x].getDirection(0)).y - 1 - tiles[arrayCount][x].tilesCount.y;
						}
						
						// 下向き
						if (tiles[arrayCount][x].getDirection(0) == U"bottom") {
							tiles[arrayCount][x].tilesCount.y = abs(tiles[arrayCount][x].tilesCount.y);
						}
						
						// 右向き
						if (tiles[arrayCount][x].getDirection(0) == U"right") {
							tiles[arrayCount][x].tilesCount.y = tiles[arrayCount][x].addons[0]->getUseTiles(tiles[arrayCount][x].getType(0), tiles[arrayCount][x].getDirection(0)).y - 1 - tiles[arrayCount][x].tilesCount.y;
						}
					}
				}
			}
		}
		
		if (currentArrayName == "residents" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].residents = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "workers_Commercial" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].workers.commercial = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "workers_Office" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].workers.office = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "workers_Industrial" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].workers.industrial = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "workers_Farm" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].workers.farm = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "workers_Public" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].workers.publicFacility = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "students" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].students = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "land_price" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].rate[U"land_price"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "happiness" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].rate[U"happiness_rate"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "crime_rate" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].rate[U"crime_rate"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "happiness" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].rate[U"happiness_rate"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (currentArrayName == "education_rate" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].rate[U"education_rate"] = stoi(temp[x].toUTF8());
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
			
			for (int x=0; x<mapsize.width; x++) {
				Array<String> agesStr = split(temp[x], U"]");
				
				for (int i=0; i<agesStr.size(); i++) {
					if (agesStr[i].length() <= 1 || agesStr[i] == U" ") {
						continue;
					}
					tiles[arrayCount][x].age.push_back(stoi(agesStr[i].substr(1, agesStr[i].length()-1).toUTF8()));
				}
			}
		}
		
		if (currentArrayName == "gender" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				Array<String> genderStr = split(temp[x], U"]");
				
				for (int i=0; i<genderStr.size(); i++) {
					if (genderStr[i].length() <= 1 || genderStr[i] == U" ") {
						continue;
					}
					tiles[arrayCount][x].gender.push_back(genderStr[i].substr(1, genderStr[i].length()-1));
				}
			}
		}
		
		if (currentArrayName == "workplace" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
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
					
					tiles[arrayCount][x].workPlaces.push_back(workPlace);
				}
			}
		}
		
		if (currentArrayName == "school" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
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
					
					tiles[arrayCount][x].schools.push_back(school);				}
			}
		}
		
		if (currentArrayName == "reservation" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (temp[x] == U"none") {
					tiles[arrayCount][x].reservation = RCOIFP::None;
				}
				else if (temp[x] == U"residential") {
					tiles[arrayCount][x].reservation = RCOIFP::Residential;
				}
				else if (temp[x] == U"commercial") {
					tiles[arrayCount][x].reservation = RCOIFP::Commercial;
				}
				else if (temp[x] == U"office") {
					tiles[arrayCount][x].reservation = RCOIFP::Office;
				}
				else if (temp[x] == U"industrial") {
					tiles[arrayCount][x].reservation = RCOIFP::Industrial;
				}
				else if (temp[x] == U"farm") {
					tiles[arrayCount][x].reservation = RCOIFP::Farm;
				}
				else if (temp[x] == U"public") {
					tiles[arrayCount][x].reservation = RCOIFP::Public;
				}
			}
		}
		
		if (currentArrayName == "original_name" && arrayCount >= 0) {
			Array<String> temp = split(strTemp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				tiles[arrayCount][x].setOriginalName(temp[x]);
			}
		}
		
		arrayCount ++;
	}
	
	System::ShowMessageBox(U"旧形式のマップデータ(*.cbd)が読み込まれました。\n保存時は新形式(*.cbj)で保存されます。", MessageBoxStyle::Warning, MessageBoxButtons::OK);
}

void CityMap::loadCBJ(String loadMapFilePath) {
	mapFilePath = loadMapFilePath;
	
	ifstream ifs(mapFilePath.toUTF8().c_str(), ios::in | ios::binary);
	
	std::string mapXOR((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	string mapDataStr = stringXOR(mapXOR, "citiesboxmapdatafilexor");
	
	saveTextFile("./data/map_temp.cbj_temp", mapDataStr);
	
	JSONReader mapData(U"./data/map_temp.cbj_temp");
	//remove("./data/map_temp.cbj_temp");
	
	savedVersion = mapData[U"Version"].get<int>();
	
	addonSet = mapData[U"Addon_Set"].getString();
	// -> アドオン読み込み
	loadAddons(addonSet);
	
	cityName = mapData[U"City_Name"].getString();
	
	mayorName = mapData[U"Mayor_Name"].getString();
	
	totalPopulation = mapData[U"Total_Population"].get<int>();
	
	changeWeather = mapData[U"Change_Weather"].get<bool>();
	
	temperature = mapData[U"Temperature"].get<int>();
	
	darkOnNight = mapData[U"Dark_on_Night"].get<bool>();
	
	mapsize.width = mapData[U"Map_size.width"].get<int>();
	mapsize.height = mapData[U"Map_size.height"].get<int>();
	
	time.year = mapData[U"Time.year"].get<int>();
	time.month = mapData[U"Time.month"].get<int>();
	time.date = mapData[U"Time.date"].get<int>();
	time.hour = mapData[U"Time.hour"].get<int>();
	time.minutes = mapData[U"Time.minutes"].get<int>();
	
	demand.residential = mapData[U"Demand.residential"].get<int>();
	demand.commercial = mapData[U"Demand.commercial"].get<int>();
	demand.office = mapData[U"Demand.office"].get<int>();
	demand.industrial = mapData[U"Demand.industrial"].get<int>();
	demand.farm = mapData[U"Demand.farm"].get<int>();
	
	money = mapData[U"Money"].get<int>();
	
	budget.police = mapData[U"Budget.police"].get<int>();
	budget.fireDepertment = mapData[U"Budget.fire_depertment"].get<int>();
	budget.postOffice = mapData[U"Budget.post_office"].get<int>();
	budget.education = mapData[U"Budget.education"].get<int>();
	
	tax.residential = mapData[U"Tax.residential"].get<int>();
	tax.commercial = mapData[U"Tax.commercial"].get<int>();
	tax.office = mapData[U"Tax.office"].get<int>();
	tax.industrial = mapData[U"Tax.industrial"].get<int>();
	tax.farm = mapData[U"Tax.farm"].get<int>();
	
	int y = 0;
	for (const auto& mapTiles : mapData[U"Map"].arrayView()) {
		tiles.push_back(Array<Tile>());
		int x = 0;
		for (const auto& tile : mapTiles.arrayView()) {
			tiles[y].push_back(Tile());
			
			for (const auto& jAddons : tile[U"addons"].arrayView()) {
				//tiles[y][x].category.push_back(j_addons[U"category"].getString());
				tiles[y][x].addType(jAddons[U"type_number"].getString());
				tiles[y][x].addDirection(jAddons[U"direction_number"].getString());
				
				// アドオンのポインタを登録
				if (addons.find(jAddons[U"name"].getString()) != addons.end()) {
					tiles[y][x].addons.push_back(addons[jAddons[U"name"].getString()]);
				}
				else {
					cout << "Cant't find " << jAddons[U"name"].getString() << endl;
				}
			}
			
			//tiles[y][x].use_tiles.x = square[U"use_tiles.x"].get<int>();
			//tiles[y][x].use_tiles.y = square[U"use_tiles.y"].get<int>();
			
			tiles[y][x].tilesCount.x = tile[U"tiles_count.x"].get<int>();
			tiles[y][x].tilesCount.y = tile[U"tiles_count.y"].get<int>();
			
			tiles[y][x].serialNumber = tile[U"serial_number"].get<int>();
			
			tiles[y][x].residents = tile[U"residents"].get<int>();
			
			tiles[y][x].workers.commercial = tile[U"workers.commercial"].get<int>();
			tiles[y][x].workers.office = tile[U"workers.office"].get<int>();
			tiles[y][x].workers.industrial = tile[U"workers.industrial"].get<int>();
			tiles[y][x].workers.farm = tile[U"workers.farm"].get<int>();
			tiles[y][x].workers.publicFacility = tile[U"workers.public"].get<int>();
			
			tiles[y][x].students = tile[U"students"].get<int>();
			
			tiles[y][x].happinessRate = tile[U"happiness_rate"].get<int>();
			
			// 各率の読み込み
			for (const auto& rate : tile[U"rate"].objectView()) {
				tiles[y][x].rate[rate.name] = rate.value.get<int>();
			}
			
			/*
			tiles[y][x].crop.name = square[U"crop.name"].getString();
			tiles[y][x].crop.amount = square[U"crop.amount"].get<int>();
			*/
			
			tiles[y][x].age = tile[U"age"].getArray<int>();
			
			tiles[y][x].gender = tile[U"gender"].getArray<String>();
			
			for (const auto& workPlaces : tile[U"work_places"].arrayView()) {
				tiles[y][x].workPlaces.push_back(WorkPlaceStruct());
				
				tiles[y][x].workPlaces.back().workPlace = getRCOIFP(workPlaces[U"work_kind"].get<int>());
				tiles[y][x].workPlaces.back().workPlacesSerialNumber = workPlaces[U"serial_number"].get<int>();
			}
			
			for (const auto& schools : tile[U"school"].arrayView()) {
				tiles[y][x].schools.push_back(SchoolStruct());
				
				tiles[y][x].schools.back().school = getSchool(schools[U"school_kind"].get<int>());
				tiles[y][x].schools.back().schoolSerialNumber = schools[U"serial_number"].get<int>();
			}
			
			tiles[y][x].reservation = getRCOIFP(tile[U"reservation"].get<int>());
			
			tiles[y][x].setOriginalName(tile[U"original_name"].getString());
			
			x++;
		}
		y++;
	}
}

void CityMap::loadAddons(String addonSetName) {
	//Array<FileStruct> addons_path = specific::getAllFilesName("./addons", "adat");
	Array<FileStruct> addonsPath = specific::getAllFilesName("./addons", "adj");
	
	for (int i=0; i<addonsPath.size(); i++) {
		FileStruct fileTemp = addonsPath[i];
		
		Addon* loadingAddon = new Addon();
		if (loadingAddon->load(addonsPath[i], addonSetName)) {
			addons[loadingAddon->getName(NameMode::English)] = loadingAddon;
		}
		else {
			delete(loadingAddon);
		}
		
		System::Sleep(20);
	}
}

Array<Addon*> CityMap::getFitAddons(Array<String> selectedCategories) {
	Array<Addon*> retAddons;
	
	for (auto addon = addons.begin(); addon != addons.end(); addon++) {
		
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
	return addons;
}

void CityMap::drawTile(CoordinateStruct coordinate, CameraStruct camera) {
	// 描画する座標を算出
	for (int i=0; i<(int)tiles[coordinate.y][coordinate.x].addons.size(); i++) {
		int rate;
		Color rateColor;
		if (showRate.length() > 0) {
			rate = getRate(coordinate, showRate);
			if (showRate == U"crime_rate") {
				rateColor = getRateColor(rate, false, 0);
			}
			else {
				rateColor = getRateColor(rate, true, 50);
			}
		}
		
		tiles[coordinate.y][coordinate.x].addons[i]->draw(tiles[coordinate.y][coordinate.x].getType(i), tiles[coordinate.y][coordinate.x].getDirection(i), coordinateToPosition(coordinate, camera), tiles[coordinate.y][coordinate.x].addons[i]->getUseTiles(tiles[coordinate.y][coordinate.x].getType(i), tiles[coordinate.y][coordinate.x].getDirection(i)), tiles[coordinate.y][coordinate.x].tilesCount, &rateColor);
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
	
	for (int i=0; i<tiles[coordinate.y][coordinate.x].addons.size(); i++) {
		retAddons << *(tiles[coordinate.y][coordinate.x].addons[i]);
	}
	
	return retAddons;
}

RCOIFstruct CityMap::getDemand() {
	return demand;
}

int CityMap::getPopulation() {
	return totalPopulation;
}

int CityMap::getMoney() {
	return money;
}

int CityMap::getTemperature() {
	return temperature;
}

SizeStruct CityMap::getMapSize() {
	return mapsize;
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
	
	if (ret.x >= mapsize.width) {
		ret.x = mapsize.width-1;
	}
	if (ret.y >= mapsize.height) {
		ret.y = mapsize.height-1;
	}
	
	return ret;
}

pair<CoordinateStruct, CoordinateStruct> CityMap::getDrawArea(CameraStruct camera) {
	// カメラの座標が変わっていなければrangeを返す
	if (camera.position.x == cameraBefore.position.x && camera.position.y == cameraBefore.position.y) {
		return range;
	}
	cameraBefore = camera;
	
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
	if (ret.first.x >= mapsize.width) {
		ret.first.x = mapsize.width-1;
	}
	if (ret.first.y >= mapsize.height) {
		ret.first.y = mapsize.height-1;
	}
	
	// 右下の座標
	ret.second = CoordinateStruct{centerCoordinate.x+rangeOneDirection, centerCoordinate.y+rangeOneDirection};
	
	if (ret.second.x < 0) {
		ret.second.x = 0;
	}
	if (ret.second.y < 0) {
		ret.second.y = 0;
	}
	if (ret.second.x >= mapsize.width) {
		ret.second.x = mapsize.width-1;
	}
	if (ret.second.y >= mapsize.height) {
		ret.second.y = mapsize.height-1;
	}
	
	range = ret;
	
	return ret;
}

// いずれかのアドオンがカテゴリに含まれているか
bool CityMap::isInCategories(String searchCategory, CoordinateStruct coordinate) {
	Tile* currentTile = &tiles[coordinate.y][coordinate.x];
	
	for (int i=0; i<currentTile->addons.size(); i++) {
		if (currentTile->addons[i]->isInCategories(searchCategory)) {
			return true;
		}
	}
	
	return false;
}

// アドオンの設置
bool CityMap::build(CoordinateStruct position, Addon* selectedAddon, bool needToBreak) {
	Tile* currentTile = &tiles[position.y][position.x];
	String type, direction;
	Array<CoordinateStruct> needUpdate;
	if (getBuildTypeAndDirection(position, selectedAddon, type, direction, needUpdate)) {
		CoordinateStruct useTiles = selectedAddon->getUseTiles(type, direction);
		
		if (direction == U"left") {
			position.y += useTiles.y-1;
		}
		if (direction == U"right") {
			position.x -= useTiles.x-1;
			position.y += useTiles.y-1;
		}
		else if (direction == U"top") {
			position.y += useTiles.y-1;
		}
		
		// 効果を取得
		map<String, EffectStruct> effects = selectedAddon->getEffects();
		
		// 中央となる座標を取得
		int centerX = useTiles.x/2;
		int centerY = useTiles.y/2;
		
		for (int y=0; abs(y)<useTiles.y; y--) {
			for (int x=0; abs(x)<useTiles.x; x++) {
				if (needToBreak && type != U"train_crossing" && type != U"bridge") {
					breaking(CoordinateStruct{position.x+x, position.y+y});
				}
				
				currentTile = &tiles[position.y+y][position.x+x];
				
				if (type != U"train_crossing" && type != U"bridge") {
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
									tiles[position.y+y+ey][position.x+x+ex].rate[effect->first] += effectPerGrid*max(abs(effect->second.grid-1-ey), abs(effect->second.grid-1-ex));
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
				
				for (int j=0; j<tiles[needUpdate[i].y][needUpdate[i].x].addons.size(); j++) {
					if (tiles[needUpdate[i].y][needUpdate[i].x].addons[j]->isInCategories(searchCategories)) {
						if (!(needUpdate[j].x == -1 && needUpdate[j].y == -1)) {
							cout << "update for " << needUpdate[i].x << "," << needUpdate[i].y << " " << needUpdate.size() << endl;
							update(needUpdate[i], tiles[needUpdate[i].y][needUpdate[i].x].addons[j], needUpdate);
						}
					}
				}
			}
		}
	}
	
	return true;
}

void CityMap::update(CoordinateStruct position, Addon* selectedAddon, Array<CoordinateStruct>& needUpdate) {
	Tile* currentTile = &tiles[position.y][position.x];
	
	// 踏切と橋の場合は更新不要
	for (int i=0; i<currentTile->getTypes().size(); i++) {
		if (currentTile->getType(i) == U"train_crossing" || currentTile->getType(i) == U"bridge") {
			return;
		}
	}
	
	String type, direction;
	if (getBuildTypeAndDirection(position, selectedAddon, type, direction, needUpdate)) {
		currentTile->clearAddons();						// 一旦クリアしてもう一度設置
		
		currentTile->addType(type);
		currentTile->addDirection(direction);
		currentTile->addons << selectedAddon;
	}
}

void CityMap::breaking(CoordinateStruct coordinate) {
	Tile* currentTile = &tiles[coordinate.y][coordinate.x];
	Array<Addon*> breakAddons = currentTile->addons;
	
	for (int i=0; i<breakAddons.size(); i++) {
		currentTile = &tiles[coordinate.y][coordinate.x];
		CoordinateStruct useTiles = breakAddons[i]->getUseTiles(currentTile->getType(i), currentTile->getDirection(i));
		
		CoordinateStruct startPoint = moveToAddonStartTile(coordinate, i);
		currentTile = &tiles[startPoint.y][startPoint.x];
		
		// 効果を取得
		map<String, EffectStruct> effects = breakAddons[i]->getEffects();
		
		// 中央となる座標を取得
		int centerX = useTiles.x/2;
		int centerY = useTiles.y/2;
		
		for (int y=0; abs(y)<useTiles.y; y--) {
			for (int x=0; x<useTiles.x; x++) {
				Tile beforeBreak = tiles[startPoint.y+y][startPoint.x+x];
				cout << "from breaking: " << endl;
				build(CoordinateStruct{startPoint.x+x, startPoint.y+y}, addons[U"tile_greenfield"], false);
				
				// 効果を除去
				if (abs(x) == centerX && abs(y) == centerY) {
					for (auto effect = effects.begin(); effect != effects.end(); effect++) {
						double effectPerGrid = effect->second.influence / effect->second.grid;
						for (int ey=-effect->second.grid; ey<=effect->second.grid; ey++) {
							for (int ex=-effect->second.grid; ex<=effect->second.grid; ex++) {
								if (isPositionAvailable(CoordinateStruct{startPoint.x+x+ex, startPoint.y+y+ey})) {
									tiles[startPoint.y+y+ey][startPoint.x+x+ex].rate[effect->first] -= effectPerGrid*max(abs(effect->second.grid-1-ey), abs(effect->second.grid-1-ex));
								}
							}
						}
					}
				}
				
				if (beforeBreak.addons[i]->isInCategories(U"connectable_type")) {
					Array<CoordinateStruct> needUpdate;
					
					for (int j=0; j<AROUND_TILES; j++) {
						for (int k=0; k<tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons.size(); k++) {
							if ((beforeBreak.addons[i]->isInCategories(U"road") && tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"road")) ||
								(beforeBreak.addons[i]->isInCategories(U"train") && tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"train")) ||
								(beforeBreak.addons[i]->isInCategories(U"waterway") && tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"waterway")) ||
								(beforeBreak.addons[i]->isInCategories(U"airport") && tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"airport"))) {
								needUpdate << CoordinateStruct{startPoint.x+x+AroundTiles[j].second.x, startPoint.y+y+AroundTiles[j].second.y};
							}
						}
					}
					
					if (breakAddons[i]->isInCategories(U"road")) {
						for (int j=0; j<needUpdate.size(); j++) {
							if (tiles[needUpdate[j].y][needUpdate[j].x].addons[i]->isInCategories(U"railroad")) {
								needUpdate[j] = {-1, -1};
							}
						}
					}
					
					for (int j=0; j<needUpdate.size(); j++) {
						for (int k=0; k<tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons.size(); k++) {
							if (!(needUpdate[j].x == -1 && needUpdate[j].y == -1)) {
								cout << "breaking update: " << needUpdate[j].x << "," << needUpdate[j].y << endl;
								update(needUpdate[j], tiles[needUpdate[j].y][needUpdate[j].x].addons[k], needUpdate);
							}
						}
					}
					
				}
			}
		}
	}
}

CoordinateStruct CityMap::moveToAddonStartTile(CoordinateStruct searchCoordinate, int addonNumber) {
	Tile* searchTile = &tiles[searchCoordinate.y][searchCoordinate.x];
	
	searchCoordinate.x -= searchTile->tilesCount.x;
	searchCoordinate.y += searchTile->tilesCount.y;
	
	return CoordinateStruct{searchCoordinate.x, searchCoordinate.y};
}

// 設置する場所に合うTypeとDirectionを取得
bool CityMap::getBuildTypeAndDirection(CoordinateStruct coordinate, Addon* selectedAddon, String& retType, String& retDirection, Array<CoordinateStruct>& needUpdate) {
	// 接続タイプ（道路など）アドオンの場合
	if (selectedAddon->isInCategories(U"connectable_type")) {
		// 周囲に道路があるか（建設可能か）確認する
		int totalAroundRoad = 0;
		Array<pair<String, CoordinateStruct>> aroundRoadCoordinate;
		
		bool needUpdateMore = true;
		if (needUpdate.size() > 0) {
			needUpdateMore = false;
		}
		
		for (int i=0; i<AROUND_TILES; i++) {
			CoordinateStruct currentTile = {coordinate.x+AroundTiles[i].second.x, coordinate.y+AroundTiles[i].second.y};
			
			if (currentTile.x < 0 || currentTile.y < 0 || currentTile.x >= mapsize.width || currentTile.y >= mapsize.height) {
				continue;
			}
			
			for (int j=0; j<tiles[currentTile.y][currentTile.x].addons.size(); j++) {
				// 道路の場合
				if ((selectedAddon->isInCategories(U"road") && tiles[currentTile.y][currentTile.x].addons[j]->isInCategories(U"road")) ||
					(selectedAddon->isInCategories(U"train") && tiles[currentTile.y][currentTile.x].addons[j]->isInCategories(U"train")) ||
					(selectedAddon->isInCategories(U"waterway") && tiles[currentTile.y][currentTile.x].addons[j]->isInCategories(U"waterway")) ||
					(selectedAddon->isInCategories(U"airport") && tiles[currentTile.y][currentTile.x].addons[j]->isInCategories(U"airport"))) {
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
			for (int i=0; i<tiles[coordinate.y][coordinate.x].addons.size(); i++) {
				if (tiles[coordinate.y][coordinate.x].addons[i]->isInCategories(U"railroad")) {
					retType = U"train_crossing";
					
					if (tiles[coordinate.y][coordinate.x].getDirection(i) == U"width") {
						retDirection = U"width";
					}
					else {
						retDirection = U"depth";
					}
					
					for (int j=0; j<needUpdate.size(); j++) {
						if (tiles[needUpdate[j].y][needUpdate[j].x].addons[i]->isInCategories(U"railroad")) {
							needUpdate[j] = {-1, -1};
						}
					}
					
					return true;
				}
			}
		}
		
		// 橋を設置する必要がある場合
		if (selectedAddon->isInCategories(U"road")) {
			for (int i=0; i<tiles[coordinate.y][coordinate.x].addons.size(); i++) {
				if (tiles[coordinate.y][coordinate.x].addons[i]->isInCategories(U"waterway")) {
					retType = U"bridge";
					
					if (tiles[coordinate.y][coordinate.x].getDirection(i) == U"width") {
						retDirection = U"depth";
					}
					else {
						retDirection = U"width";
					}
					
					for (int j=0; j<needUpdate.size(); j++) {
						if (tiles[needUpdate[j].y][needUpdate[j].x].addons[i]->isInCategories(U"waterway")) {
							needUpdate[j] = {-1, -1};
						}
					}
					
					return true;
				}
			}
		}
		
		// その他
		if (totalAroundRoad == 0) {
			retType = U"intersection_cross";
			retDirection = U"normal";
			return true;
		}
		if (totalAroundRoad == 1) {
			retType = U"dead_end";
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
				retType = U"default";
				
				if (aroundRoadCoordinate[0].second.x != 0) {
					retDirection = U"depth";
				}
				else {
					retDirection = U"width";
				}
				
				return true;
			}
			// 曲がり角
			else {
				retType = U"turn";
				
				if (findStringArray(aroundRoadCoordinate, Array<String>{U"left", U"top"})){
					retDirection = U"left-top";
					return true;
				}
				if (findStringArray(aroundRoadCoordinate, Array<String>{U"right", U"top"})){
					retDirection = U"right-top";
					return true;
				}
				if (findStringArray(aroundRoadCoordinate, Array<String>{U"left", U"bottom"})){
					retDirection = U"left-bottom";
					return true;
				}
				if (findStringArray(aroundRoadCoordinate, Array<String>{U"right", U"bottom"})){
					retDirection = U"right-bottom";
					return true;
				}
				
				return false;
			}
		}
		if (totalAroundRoad == 3) {
			retType = U"intersection_T";
			
			if (findStringArray(aroundRoadCoordinate, Array<String>{U"left", U"top", U"bottom"})){
				retDirection = U"left-top-bottom";
				return true;
			}
			if (findStringArray(aroundRoadCoordinate, Array<String>{U"right", U"top", U"bottom"})){
				retDirection = U"right-top-bottom";
				return true;
			}
			if (findStringArray(aroundRoadCoordinate, Array<String>{U"left", U"right", U"top"})){
				retDirection = U"left-right-top";
				return true;
			}
			if (findStringArray(aroundRoadCoordinate, Array<String>{U"left", U"right", U"bottom"})){
				retDirection = U"left-right-bottom";
				return true;
			}
			
			return false;
		}
		if (totalAroundRoad == 4) {
			retType = U"intersection_cross";
			retDirection = U"normal";
			return true;
		}
	}
	
	// オブジェクトタイプの場合
	if (selectedAddon->isInCategories(U"object_type")) {
		// 周囲に道路があるか確認する
		for (int i=0; i<AROUND_TILES; i++) {
			CoordinateStruct currentTile = {coordinate.x+AroundTiles[i].second.x, coordinate.y+AroundTiles[i].second.y};
			
			for (int j=0; j<tiles[currentTile.y][currentTile.x].addons.size(); j++) {
				if (tiles[currentTile.y][currentTile.x].addons[j]->isInCategories(U"road")) {
					String retTypeTemp = U"normal";
					String retDirectionTemp = AroundTiles[i].first;
					
					// 複数のタイルを使う場合、建てる方向に障害物などがないか確認する
					bool cannotBuild = false;
					
					int addX = 1, addY = 1;
					if (retTypeTemp == U"right") {
						addX = -1;
					}
					else if (retTypeTemp == U"bottom") {
						addY = -1;
					}
					
					for (int y=0; y<selectedAddon->getUseTiles(retType, retDirection).y; y+=addY) {
						for (int x=0; y<selectedAddon->getUseTiles(retType, retDirection).x; x+=addX) {
							for (int k=0; k<tiles[y][x].addons.size(); k++) {
								if (tiles[y][x].addons[k]->isInCategories(U"connectable_type")) {
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
		retType = U"normal";
		retDirection = U"normal";
		return true;
	}
	
	return false;					// 存在しない or 設置不可能な場合
}

// アドオンを削除
void CityMap::clear(CoordinateStruct position) {
	Tile* currentTile = &tiles[position.y][position.x];
	Addon* selectedAddon = addons[U"tile_greenfield"];
	
	currentTile->clearAll();
	
	currentTile->addType(U"normal");
	currentTile->addDirection(U"normal");
	currentTile->addons << selectedAddon;
	
	// 幸福度を戻す
}

bool CityMap::isPositionAvailable(CoordinateStruct coordinate) {
	if (coordinate.x >= 0 && coordinate.x <= mapsize.width-1 && coordinate.y >= 0 && coordinate.y <= mapsize.height-1) {
		return true;
	}
	else {
		return false;
	}
}

TimeStruct CityMap::cityTime(int minutesDelta) {
	time.minutes += minutesDelta;
	
	if (time.minutes >= 60) {
		time.hour ++;
		time.minutes -= 60;
		
		if (time.hour >= 24) {
			time.date ++;
			time.hour -= 24;
			
			if (time.month == 4 || time.month == 6 || time.month == 9 || time.month == 11) {
				if (time.date >= 31) {
					time.month ++;
					time.date -= 30;
				}
			}
			else if (time.month == 2) {
				if (time.year % 4 == 0 && time.date >= 30) {
					time.month ++;
					time.date -= 29;
				}
				else if (time.date >= 29) {
					time.month ++;
					time.date -= 28;
				}
			}
			else if (time.date >= 32) {
				time.month ++;
				time.date -= 31;
				
				if (time.month >= 13) {
					time.year ++;
					time.month -= 12;
				}
			}
		}
	}
	
	
	return time;
}

map<String, int> CityMap::getRate(CoordinateStruct coordinate) {
	return tiles[coordinate.y][coordinate.x].rate;
}

int CityMap::getRate(CoordinateStruct coordinate, String rateName) {
	if (tiles[coordinate.y][coordinate.x].rate.find(rateName) != tiles[coordinate.y][coordinate.x].rate.end()) {
		return tiles[coordinate.y][coordinate.x].rate[rateName];
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
	showRate = rateName;
}

bool CityMap::save() {
	// JsonWriterを宣言
	JSONWriter mapData;
	
	mapData.startObject();
	{
		mapData.key(U"Version").write(RELEASE_NUMBER);
		mapData.key(U"Addon_Set").write(addonSet);
		mapData.key(U"City_Name").write(cityName);
		mapData.key(U"Mayor_Name").write(mayorName);
		mapData.key(U"Total_Population").write(totalPopulation);
		mapData.key(U"Change_Weather").write(changeWeather);
		mapData.key(U"Temperature").write(temperature);
		mapData.key(U"Dark_on_Night").write(darkOnNight);
		
		mapData.key(U"Map_size").startObject();
		{
			mapData.key(U"width").write(mapsize.width);
			mapData.key(U"height").write(mapsize.height);
		}
		mapData.endObject();
		
		mapData.key(U"Time").startObject();
		{
			mapData.key(U"year").write(time.year);
			mapData.key(U"month").write(time.month);
			mapData.key(U"date").write(time.date);
			mapData.key(U"hour").write(time.hour);
			mapData.key(U"minutes").write(time.minutes);
		}
		mapData.endObject();
		
		mapData.key(U"Demand").startObject();
		{
			mapData.key(U"residential").write(demand.residential);
			mapData.key(U"commercial").write(demand.commercial);
			mapData.key(U"office").write(demand.office);
			mapData.key(U"industrial").write(demand.industrial);
			mapData.key(U"farm").write(demand.farm);
		}
		mapData.endObject();
		
		mapData.key(U"Money").write(money);
		
		mapData.key(U"Budget").startObject();
		{
			mapData.key(U"police").write(budget.police);
			mapData.key(U"fire_depertment").write(budget.fireDepertment);
			mapData.key(U"post_office").write(budget.postOffice);
			mapData.key(U"education").write(budget.education);
		}
		mapData.endObject();
		
		mapData.key(U"Tax").startObject();
		{
			mapData.key(U"residential").write(tax.residential);
			mapData.key(U"commercial").write(tax.commercial);
			mapData.key(U"office").write(tax.office);
			mapData.key(U"industrial").write(tax.industrial);
			mapData.key(U"farm").write(tax.farm);
		}
		mapData.endObject();
		
		mapData.key(U"Map").startArray();
		{
			for (int y=0; y<mapsize.height; y++) {
				mapData.startArray();
				{
					for (int x=0; x<mapsize.width; x++) {
						mapData.startObject();
						{
							mapData.key(U"addons").startArray();
							{
								for (int i=0; i<tiles[y][x].addons.size(); i++) {
									mapData.startObject();
									{
										mapData.key(U"name").write(tiles[y][x].addons[i]->getName(NameMode::English));
										//map_file.key(U"category").write(tiles[y][x].category[i]);
										mapData.key(U"type_number").write(tiles[y][x].getType(i));
										mapData.key(U"direction_number").write(tiles[y][x].getDirection(i));
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
								mapData.key(U"x").write(tiles[y][x].tilesCount.x);
								mapData.key(U"y").write(tiles[y][x].tilesCount.y);
							}
							mapData.endObject();
							
							mapData.key(U"serial_number").write(tiles[y][x].serialNumber);
							
							mapData.key(U"residents").write(tiles[y][x].residents);
							
							mapData.key(U"workers").startObject();
							{
								mapData.key(U"commercial").write(tiles[y][x].workers.commercial);
								mapData.key(U"office").write(tiles[y][x].workers.office);
								mapData.key(U"industrial").write(tiles[y][x].workers.industrial);
								mapData.key(U"farm").write(tiles[y][x].workers.farm);
								mapData.key(U"public").write(tiles[y][x].workers.publicFacility);
							}
							mapData.endObject();
							
							mapData.key(U"students").write(tiles[y][x].students);
							
							mapData.key(U"rate").startObject();
							{
								for (auto rate = tiles[y][x].rate.begin(); rate != tiles[y][x].rate.end() ; rate++) {
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
								for (int i=0; i<tiles[y][x].age.size(); i++) {
									mapData.write(tiles[y][x].age[i]);
								}
							}
							mapData.endArray();
							
							mapData.key(U"gender").startArray();
							{
								for (int i=0; i<tiles[y][x].gender.size(); i++) {
									mapData.write(tiles[y][x].gender[i]);
								}
							}
							mapData.endArray();
							
							mapData.key(U"work_places").startArray();
							{
								for (int i=0; i<tiles[y][x].workPlaces.size(); i++) {
									mapData.startObject();
									{
										mapData.key(U"work_kind").write(tiles[y][x].workPlaces[i].workPlace);
										mapData.key(U"serial_number").write(tiles[y][x].workPlaces[i].workPlacesSerialNumber);
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							
							mapData.key(U"school").startArray();
							{
								for (int i=0; i<tiles[y][x].schools.size(); i++) {
									mapData.startObject();
									{
										mapData.key(U"school_kind").write(tiles[y][x].schools[i].school);
										mapData.key(U"serial_number").write(tiles[y][x].schools[i].schoolSerialNumber);
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							
							mapData.key(U"reservation").write(tiles[y][x].reservation);
							
							mapData.key(U"original_name").write(tiles[y][x].getOriginalName());
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
	
	saveTextFile(mapFilePath.toUTF8(), stringXOR(mapData.get().toUTF8(), "citiesboxmapdatafilexor"));
	//saveTextFile(map_file_path.toUTF8()+".cbj", map_file.get().toUTF8());
	//map_file.save(map_file_path+U".cbj");
	
	return true;
}

void CityMap::freeMapAndAddons() {
	for (auto i = addons.begin(); i != addons.end() ; i++) {
		delete(i->second);
	}
	
	Array<Array<Tile>>().swap(tiles);
}
