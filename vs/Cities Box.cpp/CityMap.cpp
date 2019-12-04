//
//  CityMap.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "CityMap.hpp"


bool CityMap::getElement(string str, string search_element_name, string& ret) {
	if (str.find(search_element_name) != string::npos && str.find("=") != string::npos) {
		ret = str.substr(str.find("\"") + 1, str.find(";") - (str.find("\"") + 2) - LINE_FEED_CODE);
		return true;
	}
	return false;
}

bool CityMap::getElement(string str, string search_element_name, int& ret) {
	if (str.find(search_element_name) != string::npos && str.find("=") != string::npos) {
		ret = stoi(str.substr(str.find("= ") + 2, str.find(";") - (str.find("= ") + 2) - LINE_FEED_CODE));
		return true;
	}
	return false;
}

bool CityMap::getElement(string str, string search_element_name, bool& ret) {
	if (str.find(search_element_name) != string::npos && str.find("=") != string::npos) {
		int int_temp = stoi(str.substr(str.find("= ") + 2, str.find(";") - (str.find("= ") + 2) - LINE_FEED_CODE));
		if (int_temp == 1) {
			ret = true;
			return true;
		}
		else if (int_temp == 0) {
			ret = false;
			return true;
		}
	}
	return false;
}

bool CityMap::getTypes(string str, string search_element_name, Array<string>& ret) {
	string a_ret;
	if (getElement(str, search_element_name, a_ret)) {
		ret = split(a_ret, ", ");
		return true;
	}
	return false;
}


void CityMap::load(FileStruct map_file) {
	// マップファイルの読み込み
	ifstream ifs(map_file.file_path.c_str());
	string str_temp;
	
	if (ifs.fail()) {
		cerr << "Failed to open file." << endl;
	}
	
	// 各要素の読み出し
	string current_array_name = "";
	Array<string> array_names =
	{"name", "name2", "category", "category_2", "category_3", "obj_type", "obj_type2", "obj_dire", "obj_dire2",
		"obj_serial_num", "obj_use_tiles_x", "obj_use_tiles_y", "obj_tiles_x", "obj_tiles_y", "residents",
		"workers_Commercial", "workers_Office", "workers_Industrial", "workers_Farm", "workers_Public",
		"students", "land_price", "crime_rate", "education_rate", "happiness", "noise", "crop", "crop_amount",
		"age", "gender", "workplace", "school", "reservation", "original_name"};
	int array_count = 0;
	bool map_cleared = false;

	mapsize.width = -1;
	mapsize.height = -1;
	
	bool addon_loaded = false;
	
	while (getline(ifs, str_temp)) {
		str_temp = str_temp.substr(0, str_temp.length()-LINE_FEED_CODE);				// 改行コードは除く
		
		getElement(str_temp, "Version", saved_version);
		getElement(str_temp, "Addons_Set", addon_set);
		// アドオン読み込み
		if (!addon_loaded && addon_set.length() > 0) {
			loadAddons(addon_set);
			addon_loaded = true;
		}
		
		getElement(str_temp, "City_Name", city_name);
		getElement(str_temp, "Mayor_Name", mayor_name);
		getElement(str_temp, "Total_Population", total_population);
		getElement(str_temp, "Set_Change_Weather", change_weather);
		getElement(str_temp, "Temperature", temperature);
		getElement(str_temp, "Set_Dark_on_Night", dark_on_night);
		
		getElement(str_temp, "mapsize_x", mapsize.width);
		getElement(str_temp, "mapsize_y", mapsize.height);
		
		getElement(str_temp, "time_Year", time.year);
		getElement(str_temp, "time_Month", time.month);
		getElement(str_temp, "time_Date", time.date);
		getElement(str_temp, "time_Hour", time.hour);
		getElement(str_temp, "time_Minutes", time.minutes);
		
		getElement(str_temp, "demand_Residential", demand.residential);
		getElement(str_temp, "demand_Commercial", demand.commercial);
		getElement(str_temp, "demand_Office", demand.office);
		getElement(str_temp, "demand_Industrial", demand.industrial);
		getElement(str_temp, "demand_Farm", demand.farm);
		
		getElement(str_temp, "money", money);
		
		getElement(str_temp, "budget_of_Police", budget.police);
		getElement(str_temp, "budget_of_Fire_Depertment", budget.fire_depertment);
		getElement(str_temp, "budget_of_Post_Office", budget.post_office);
		getElement(str_temp, "budget_of_Education", budget.education);
		
		getElement(str_temp, "tax_of_Residential", tax.residential);
		getElement(str_temp, "tax_of_Commercial", tax.commercial);
		getElement(str_temp, "tax_of_Office", tax.office);
		getElement(str_temp, "tax_of_Industrial", tax.industrial);
		getElement(str_temp, "tax_of_Farm", tax.farm);
		
		if (!map_cleared && mapsize.width > 0 && mapsize.height > 0) {
			for (int y=0; y<mapsize.height; y++) {
				SquareStruct new_ss;
				squares.push_back(Array<SquareStruct>(mapsize.width, new_ss));
			}
			map_cleared = true;
		}
		
		for (int i=0; i<array_names.size(); i++) {
			if (str_temp.find(array_names[i]+"(x,y) {") == 0) {
				current_array_name = array_names[i];
				array_count = -1;
			}
		}
		if (str_temp.find("}") == 0) {
			current_array_name = "";
		}
		
		if (current_array_name == "name" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");

			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].addon_name.push_back(temp[x]);
				
				// マップにAddon_Setが定義されていない場合はNormalとみなしアドオン読み込み
				if (!addon_loaded) {
					loadAddons("");
					addon_loaded = true;
					addon_set = "Normal";
				}
				
				// アドオンのポインタを登録
				if (addons.find(squares[array_count][x].addon_name[0]) != addons.end()) {
					squares[array_count][x].addons.push_back(addons[squares[array_count][x].addon_name[0]]);
				}
			}
		}
		
		if (current_array_name == "name2" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].addon_name.push_back(temp[x]);
			}
		}
		
		if (current_array_name == "category" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].category.push_back(temp[x]);
			}
		}
		
		if (current_array_name == "category_2" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].category.push_back(temp[x]);
			}
		}
		
		if (current_array_name == "category_3" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].category.push_back(temp[x]);
			}
		}
		
		if (current_array_name == "obj_type" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].type_number.push_back(stoi(temp[x]));
			}
		}
		
		if (current_array_name == "obj_type2" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].type_number.push_back(stoi(temp[x]));
			}
		}
		
		if (current_array_name == "obj_dire" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].direction_number.push_back(stoi(temp[x]));
			}
		}
		
		if (current_array_name == "obj_dire2" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].direction_number.push_back(stoi(temp[x]));
			}
		}
		
		if (current_array_name == "obj_serial_num" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].serial_number = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "obj_use_tiles_x" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].use_tiles.x = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "obj_use_tiles_y" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].use_tiles.y = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "obj_tiles_x" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].tiles_count.x = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "obj_tiles_y" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].tiles_count.y = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "residents" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].residents = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "wokers_Commercial" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].workers.commercial = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "wokers_Office" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].workers.office = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "wokers_Industrial" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].workers.industrial = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "wokers_Farm" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].workers.farm = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "wokers_Public" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].workers.public_facility = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "students" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].students = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "land_price" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].land_price = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "crime_rate" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].crime_rate = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "education_rate" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].education_rate = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "happiness" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].happiness_rate = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "noise" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].noise = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "crop" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].crop.name = temp[x];
			}
		}
		
		if (current_array_name == "crop_amount" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].crop.amount = stoi(temp[x]);
			}
		}
		
		if (current_array_name == "age" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				Array<string> ages_str = split(temp[x], "]");
				
				for (int i=0; i<ages_str.size(); i++) {
					if (ages_str[i].length() <= 1 || ages_str[i] == " ") {
						continue;
					}
					squares[array_count][x].age.push_back(stoi(ages_str[i].substr(1, ages_str[i].length()-1)));
				}
			}
		}
		
		if (current_array_name == "gender" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				Array<string> gender_str = split(temp[x], "]");
				
				for (int i=0; i<gender_str.size(); i++) {
					if (gender_str[i].length() <= 1 || gender_str[i] == " ") {
						continue;
					}
					squares[array_count][x].gender.push_back(gender_str[i].substr(1, gender_str[i].length()-1));
				}
			}
		}
		
		if (current_array_name == "workplace" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				Array<string> workplace_str = split(temp[x], "]");
				
				for (int i=0; i<workplace_str.size(); i++) {
					if (workplace_str[i].length() <= 1 || workplace_str[i] == " ") {
						continue;
					}
					string workplace_and_serial = workplace_str[i].substr(1, workplace_str[i].length()-1);
					
					WorkPlaceStruct workplace;
					if (workplace_and_serial.substr(0, 1) == "c") {
						workplace.work_place = RCOIFP::Commercial;
					}
					else if (workplace_and_serial.substr(0, 1) == "o") {
						workplace.work_place = RCOIFP::Office;
					}
					else if (workplace_and_serial.substr(0, 1) == "i") {
						workplace.work_place = RCOIFP::Industrial;
					}
					else if (workplace_and_serial.substr(0, 1) == "f") {
						workplace.work_place = RCOIFP::Farm;
					}
					else if (workplace_and_serial.substr(0, 1) == "p") {
						workplace.work_place = RCOIFP::Public;
					}
					
					workplace.work_places_serial_number = stoi(workplace_and_serial.substr(1, workplace_and_serial.length()-1));
					
					squares[array_count][x].work_places.push_back(workplace);
				}
			}
		}
		
		if (current_array_name == "school" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				Array<string> school_str = split(temp[x], "]");
				
				for (int i=0; i<school_str.size(); i++) {
					if (school_str[i].length() <= 1 || school_str[i] == " ") {
						continue;
					}
					string school_and_serial = school_str[i].substr(1, school_str[i].length()-1);
					
					SchoolStruct school;
					if (school_and_serial.substr(0, 1) == "e") {
						school.school = School::ElementarySchool;
					}
					else if (school_and_serial.substr(0, 1) == "j") {
						school.school = School::JuniorHighSchool;
					}
					else if (school_and_serial.substr(0, 1) == "h") {
						school.school = School::HighSchool;
					}
					else if (school_and_serial.substr(0, 1) == "U") {
						school.school = School::University;
					}
					
					school.school_serial_number = stoi(school_and_serial.substr(1, school_and_serial.length()-1));
					
					squares[array_count][x].schools.push_back(school);				}
			}
		}
		
		if (current_array_name == "reservation" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (temp[x] == "none") {
					squares[array_count][x].reservation = RCOIFP::None;
				}
				else if (temp[x] == "residential") {
					squares[array_count][x].reservation = RCOIFP::Residential;
				}
				else if (temp[x] == "commercial") {
					squares[array_count][x].reservation = RCOIFP::Commercial;
				}
				else if (temp[x] == "office") {
					squares[array_count][x].reservation = RCOIFP::Office;
				}
				else if (temp[x] == "industrial") {
					squares[array_count][x].reservation = RCOIFP::Industrial;
				}
				else if (temp[x] == "farm") {
					squares[array_count][x].reservation = RCOIFP::Farm;
				}
				else if (temp[x] == "public") {
					squares[array_count][x].reservation = RCOIFP::Public;
				}
			}
		}
		
		if (current_array_name == "original_name" && array_count >= 0) {
			Array<string> temp = split(str_temp, ", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].original_name = temp[x];
			}
		}
		
		array_count ++;
	}
}

void CityMap::loadAddons(string addon_set_name) {
	Array<FileStruct> addons_path = specific::getAllFilesName("./addons", "adat");
	
	for (int i=0; i<addons_path.size(); i++) {
		FileStruct file_temp = addons_path[i];
		
		Addon* new_addon = new Addon();
		if (new_addon->load(addons_path[i], addon_set_name)) {
			addons[split(file_temp.file_name, ".")[0]] = new_addon;
		}
		
		System::Sleep(100);
	}
}

void CityMap::drawSquare(CoordinateStruct coordinate, CameraStruct camera) {
	// 描画する座標を算出
	squares[coordinate.y][coordinate.x].addons[0]->draw(squares[coordinate.y][coordinate.x].addons[0]->getTypeName(squares[coordinate.y][coordinate.x].type_number[0]), squares[coordinate.y][coordinate.x].addons[0]->getDirectionName(squares[coordinate.y][coordinate.x].type_number[0], squares[coordinate.y][coordinate.x].direction_number[0]), coordinateToPosition(coordinate, camera), squares[coordinate.y][coordinate.x].use_tiles, squares[coordinate.y][coordinate.x].tiles_count);
}

void CityMap::draw(CameraStruct camera) {
	for (short int y=getDrawArea(camera).first.y; y<getDrawArea(camera).second.y; y++) {
		for (short int x=getDrawArea(camera).first.x; x<getDrawArea(camera).second.x; x++) {
			PositionStruct draw_pos = coordinateToPosition(CoordinateStruct{x, y}, camera);
			
			if (draw_pos.x >= -CHIP_SIZE && draw_pos.y >= -CHIP_SIZE/2 && draw_pos.x <= Scene::Width() && draw_pos.y <= Scene::Height() + CHIP_SIZE*2) {
				drawSquare(CoordinateStruct{x, y}, camera);
			}
		}
	}
}

SizeStruct CityMap::getMapSize() {
	return mapsize;
}

PositionStruct CityMap::coordinateToPosition(CoordinateStruct coordinate, CameraStruct camera) {
	// カメラの座標がデフォルト値（64*mapsize.width/2-Scene::Width()/2, 0）のときの描画位置を算出
	CameraStruct default_camera;
	default_camera.position = PositionStruct{0, 0};
	default_camera.center = {0, 0};
	
	// 基準点となるx:0, y:0のマスの表示位置を算出する
	PositionStruct square_0x0_position = PositionStruct{default_camera.position.x - camera.position.x,
		default_camera.position.y - camera.position.y};
	
	PositionStruct square_position = PositionStruct{square_0x0_position.x+coordinate.x*32-coordinate.y*32, square_0x0_position.y+coordinate.y*16+coordinate.x*16};
	
	return square_position;
}

CoordinateStruct CityMap::positionToCoordinate(PositionStruct position, CameraStruct camera) {
	int temp_mx = position.x+camera.position.x-CHIP_SIZE/2;
	int temp_my = position.y+camera.position.y;
	
	CoordinateStruct ret;
	ret.x = (temp_my + temp_mx/2) / (CHIP_SIZE/2) - 1;
	ret.y = (-temp_mx + temp_my*2) / CHIP_SIZE - 1;
	
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
	if (camera.position.x == camera_before.position.x && camera.position.y == camera_before.position.y) {
		return range;
	}
	camera_before = camera;
	
	// 描画できる範囲
	int range_one_direction = (sqrt(powf(Scene::Width(), 2)+powf(Scene::Height(), 2))/sqrt(powf(CHIP_SIZE/2, 2)+powf(CHIP_SIZE/4, 2)));
	
	// 画面中央の座標
	CoordinateStruct center_coordinate = positionToCoordinate(PositionStruct{Scene::Width()/2, Scene::Height()/2}, camera);
	
	pair<CoordinateStruct, CoordinateStruct> ret;
	// 左上の座標
	ret.first = CoordinateStruct{center_coordinate.x-range_one_direction, center_coordinate.y-range_one_direction};
	
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
	ret.second = CoordinateStruct{center_coordinate.x+range_one_direction, center_coordinate.y+range_one_direction};
	
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

void CityMap::freeMapAndAddons() {
	for (auto i = addons.begin(); i != addons.end() ; i++) {
		free(i->second);
	}
	
	Array<Array<SquareStruct>>().swap(squares);
}
