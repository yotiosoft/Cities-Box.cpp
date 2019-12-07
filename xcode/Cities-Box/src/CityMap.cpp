//
//  CityMap.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "CityMap.hpp"


bool CityMap::getElement(String str, String search_element_name, String& ret) {
	string str_utf8 = str.toUTF8();
	string search_element_name_utf8 = search_element_name.toUTF8();
	
	if (str_utf8.find(search_element_name_utf8) != string::npos && str_utf8.find("=") != string::npos) {
		ret = Unicode::Widen(str_utf8.substr(str_utf8.find("\"") + 1, str_utf8.find(";") - (str_utf8.find("\"") + 2)));
		return true;
	}
	return false;
}

bool CityMap::getElement(String str, String search_element_name, int& ret) {
	string str_utf8 = str.toUTF8();
	string search_element_name_utf8 = search_element_name.toUTF8();
	
	if (str_utf8.find(search_element_name_utf8) != string::npos && str_utf8.find("=") != string::npos) {
		ret = stoi(str_utf8.substr(str_utf8.find("= ") + 2, str_utf8.find(";") - (str_utf8.find("= ") + 2)));
		return true;
	}
	return false;
}

bool CityMap::getElement(String str, String search_element_name, bool& ret) {
	string str_utf8 = str.toUTF8();
	string search_element_name_utf8 = search_element_name.toUTF8();
	
	if (str_utf8.find(search_element_name_utf8) != string::npos && str_utf8.find("=") != string::npos) {
		int int_temp = stoi(str_utf8.substr(str_utf8.find("= ") + 2, str_utf8.find(";") - (str_utf8.find("= ") + 2)));
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

bool CityMap::getTypes(String str, String search_element_name, Array<String>& ret) {
	String a_ret;
	if (getElement(str, search_element_name, a_ret)) {
		ret = split(a_ret, U", ");
		return true;
	}
	return false;
}


void CityMap::load(String new_map_file_path) {
	if (FileSystem::Extension(new_map_file_path) == U"cbd") {
		loadCBD(new_map_file_path);
	}
	else if (FileSystem::Extension(new_map_file_path) == U"cbj") {
		loadCBJ(new_map_file_path);
	}
}

void CityMap::loadCBD(String new_map_file_path) {
	// マップファイルの読み込み
	map_file_path = new_map_file_path;
	
	ifstream ifs(map_file_path.toUTF8().c_str());
	string str_temp_utf8;
	String str_temp;
	
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
	
	while (getline(ifs, str_temp_utf8)) {
		str_temp = Unicode::Widen(str_temp_utf8);
		str_temp = str_temp.substr(0, str_temp.length()-LINE_FEED_CODE);				// 改行コードは除く
		
		getElement(str_temp, U"Version", saved_version);
		getElement(str_temp, U"Addons_Set", addon_set);
		
		// アドオン読み込み
		if (!addon_loaded && addon_set.length() > 0) {
			loadAddons(addon_set.toUTF8());
			addon_loaded = true;
		}
		
		getElement(str_temp, U"City_Name", city_name);
		getElement(str_temp, U"Mayor_Name", mayor_name);
		getElement(str_temp, U"Total_Population", total_population);
		getElement(str_temp, U"Set_Change_Weather", change_weather);
		getElement(str_temp, U"Temperature", temperature);
		getElement(str_temp, U"Set_Dark_on_Night", dark_on_night);
		
		getElement(str_temp, U"mapsize_x", mapsize.width);
		getElement(str_temp, U"mapsize_y", mapsize.height);
		
		getElement(str_temp, U"time_Year", time.year);
		getElement(str_temp, U"time_Month", time.month);
		getElement(str_temp, U"time_Date", time.date);
		getElement(str_temp, U"time_Hour", time.hour);
		getElement(str_temp, U"time_Minutes", time.minutes);
		
		getElement(str_temp, U"demand_Residential", demand.residential);
		getElement(str_temp, U"demand_Commercial", demand.commercial);
		getElement(str_temp, U"demand_Office", demand.office);
		getElement(str_temp, U"demand_Industrial", demand.industrial);
		getElement(str_temp, U"demand_Farm", demand.farm);
		
		getElement(str_temp, U"money", money);
		
		getElement(str_temp, U"budget_of_Police", budget.police);
		getElement(str_temp, U"budget_of_Fire_Depertment", budget.fire_depertment);
		getElement(str_temp, U"budget_of_Post_Office", budget.post_office);
		getElement(str_temp, U"budget_of_Education", budget.education);
		
		getElement(str_temp, U"tax_of_Residential", tax.residential);
		getElement(str_temp, U"tax_of_Commercial", tax.commercial);
		getElement(str_temp, U"tax_of_Office", tax.office);
		getElement(str_temp, U"tax_of_Industrial", tax.industrial);
		getElement(str_temp, U"tax_of_Farm", tax.farm);
		
		if (!map_cleared && mapsize.width > 0 && mapsize.height > 0) {
			for (int y=0; y<mapsize.height; y++) {
				SquareStruct new_ss;
				squares.push_back(Array<SquareStruct>(mapsize.width, new_ss));
			}
			map_cleared = true;
		}
		
		for (int i=0; i<array_names.size(); i++) {
			if (str_temp_utf8.find(array_names[i]+"(x,y) {") == 0) {
				current_array_name = array_names[i];
				array_count = -1;
			}
		}
		if (str_temp_utf8.find("}") == 0) {
			current_array_name = "";
		}
		
		if (current_array_name == "name" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].addon_name.push_back(temp[x]);
				
				// マップにAddon_Setが定義されていない場合はNormalとみなしアドオン読み込み
				if (!addon_loaded) {
					loadAddons("");
					addon_loaded = true;
					addon_set = U"Normal";
				}
				
				// アドオンのポインタを登録
				if (addons.find(squares[array_count][x].addon_name[0].toUTF8()) != addons.end()) {
					squares[array_count][x].addons.push_back(addons[squares[array_count][x].addon_name[0].toUTF8()]);
				}
			}
		}
		
		if (current_array_name == "name2" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (temp[x].length() == 0) {
					continue;
				}
				
				squares[array_count][x].addon_name.push_back(temp[x]);
				
				// アドオンのポインタを登録
				if (addons.find(squares[array_count][x].addon_name[1].toUTF8()) != addons.end()) {
					squares[array_count][x].addons.push_back(addons[squares[array_count][x].addon_name[1].toUTF8()]);
				}
			}
		}
		
		if (current_array_name == "category" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].category.push_back(temp[x]);
			}
		}
		
		if (current_array_name == "category_2" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].category.push_back(temp[x]);
			}
		}
		
		if (current_array_name == "category_3" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].category.push_back(temp[x]);
			}
		}
		
		if (current_array_name == "obj_type" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].type_number.push_back(stoi(temp[x].toUTF8()));
			}
		}
		
		if (current_array_name == "obj_type2" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].type_number.push_back(stoi(temp[x].toUTF8()));
			}
		}
		
		if (current_array_name == "obj_dire" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].direction_number.push_back(stoi(temp[x].toUTF8()));
			}
		}
		
		if (current_array_name == "obj_dire2" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].direction_number.push_back(stoi(temp[x].toUTF8()));
			}
		}
		
		if (current_array_name == "obj_serial_num" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].serial_number = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "obj_use_tiles_x" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].use_tiles.x = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "obj_use_tiles_y" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].use_tiles.y = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "obj_tiles_x" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].tiles_count.x = stoi(temp[x].toUTF8());
				
				// obj_tiles_xを修正(r140以前のバージョンで保存した場合)
				if (saved_version <= 140) {
					if (squares[array_count][x].use_tiles.x > 0) {
						// 左向き
						if (squares[array_count][x].direction_number[0] == 0) {
							
						}
						
						// 上向き
						if (squares[array_count][x].direction_number[0] == 1) {
							
						}
						
						// 下向き
						if (squares[array_count][x].direction_number[0] == 2) {
							
						}
						
						// 右向き
						if (squares[array_count][x].direction_number[0] == 3) {
							squares[array_count][x].tiles_count.x += squares[array_count][x].use_tiles.x - 1;
						}
					}
				}
			}
		}
		
		if (current_array_name == "obj_tiles_y" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].tiles_count.y = stoi(temp[x].toUTF8());
				
				// obj_tiles_yを修正(r140以前のバージョンで保存した場合)
				if (saved_version <= 140) {
					if (squares[array_count][x].use_tiles.y > 0) {
						// 左向き
						if (squares[array_count][x].direction_number[0] == 0) {
							
						}
						
						// 上向き
						if (squares[array_count][x].direction_number[0] == 1) {
							squares[array_count][x].tiles_count.y = squares[array_count][x].use_tiles.y - 1 - squares[array_count][x].tiles_count.y;
						}
						
						// 下向き
						if (squares[array_count][x].direction_number[0] == 2) {
							squares[array_count][x].tiles_count.y = abs(squares[array_count][x].tiles_count.y);
						}
						
						// 右向き
						if (squares[array_count][x].direction_number[0] == 3) {
							squares[array_count][x].tiles_count.y = squares[array_count][x].use_tiles.y - 1 - squares[array_count][x].tiles_count.y;
						}
					}
				}
			}
		}
		
		if (current_array_name == "residents" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].residents = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "workers_Commercial" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].workers.commercial = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "workers_Office" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].workers.office = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "workers_Industrial" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].workers.industrial = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "workers_Farm" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].workers.farm = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "workers_Public" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].workers.public_facility = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "students" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].students = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "land_price" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].land_price = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "crime_rate" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].crime_rate = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "education_rate" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].education_rate = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "happiness" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].happiness_rate = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "noise" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].noise = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "crop" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].crop.name = temp[x];
			}
		}
		
		if (current_array_name == "crop_amount" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].crop.amount = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "age" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				Array<String> ages_str = split(temp[x], U"]");
				
				for (int i=0; i<ages_str.size(); i++) {
					if (ages_str[i].length() <= 1 || ages_str[i] == U" ") {
						continue;
					}
					squares[array_count][x].age.push_back(stoi(ages_str[i].substr(1, ages_str[i].length()-1).toUTF8()));
				}
			}
		}
		
		if (current_array_name == "gender" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				Array<String> gender_str = split(temp[x], U"]");
				
				for (int i=0; i<gender_str.size(); i++) {
					if (gender_str[i].length() <= 1 || gender_str[i] == U" ") {
						continue;
					}
					squares[array_count][x].gender.push_back(gender_str[i].substr(1, gender_str[i].length()-1));
				}
			}
		}
		
		if (current_array_name == "workplace" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				Array<String> workplace_str = split(temp[x], U"]");
				
				for (int i=0; i<workplace_str.size(); i++) {
					if (workplace_str[i].length() <= 1 || workplace_str[i] == U" ") {
						continue;
					}
					String workplace_and_serial = workplace_str[i].substr(1, workplace_str[i].length()-1);
					
					WorkPlaceStruct workplace;
					if (workplace_and_serial.substr(0, 1) == U"c") {
						workplace.work_place = RCOIFP::Commercial;
					}
					else if (workplace_and_serial.substr(0, 1) == U"o") {
						workplace.work_place = RCOIFP::Office;
					}
					else if (workplace_and_serial.substr(0, 1) == U"i") {
						workplace.work_place = RCOIFP::Industrial;
					}
					else if (workplace_and_serial.substr(0, 1) == U"f") {
						workplace.work_place = RCOIFP::Farm;
					}
					else if (workplace_and_serial.substr(0, 1) == U"p") {
						workplace.work_place = RCOIFP::Public;
					}
					
					workplace.work_places_serial_number = stoi(workplace_and_serial.substr(1, workplace_and_serial.length()-1).toUTF8());
					
					squares[array_count][x].work_places.push_back(workplace);
				}
			}
		}
		
		if (current_array_name == "school" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				Array<String> school_str = split(temp[x], U"]");
				
				for (int i=0; i<school_str.size(); i++) {
					if (school_str[i].length() <= 1 || school_str[i] == U" ") {
						continue;
					}
					String school_and_serial = school_str[i].substr(1, school_str[i].length()-1);
					
					SchoolStruct school;
					if (school_and_serial.substr(0, 1) == U"e") {
						school.school = School::ElementarySchool;
					}
					else if (school_and_serial.substr(0, 1) == U"j") {
						school.school = School::JuniorHighSchool;
					}
					else if (school_and_serial.substr(0, 1) == U"h") {
						school.school = School::HighSchool;
					}
					else if (school_and_serial.substr(0, 1) == U"U") {
						school.school = School::University;
					}
					
					school.school_serial_number = stoi(school_and_serial.substr(1, school_and_serial.length()-1).toUTF8());
					
					squares[array_count][x].schools.push_back(school);				}
			}
		}
		
		if (current_array_name == "reservation" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (temp[x] == U"none") {
					squares[array_count][x].reservation = RCOIFP::None;
				}
				else if (temp[x] == U"residential") {
					squares[array_count][x].reservation = RCOIFP::Residential;
				}
				else if (temp[x] == U"commercial") {
					squares[array_count][x].reservation = RCOIFP::Commercial;
				}
				else if (temp[x] == U"office") {
					squares[array_count][x].reservation = RCOIFP::Office;
				}
				else if (temp[x] == U"industrial") {
					squares[array_count][x].reservation = RCOIFP::Industrial;
				}
				else if (temp[x] == U"farm") {
					squares[array_count][x].reservation = RCOIFP::Farm;
				}
				else if (temp[x] == U"public") {
					squares[array_count][x].reservation = RCOIFP::Public;
				}
			}
		}
		
		if (current_array_name == "original_name" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].original_name = temp[x];
			}
		}
		
		array_count ++;
	}
	
	System::ShowMessageBox(U"旧形式のマップデータ(*.cbd)が読み込まれました。\n保存時は新形式(*.cbj)で保存されます。", MessageBoxStyle::Warning, MessageBoxButtons::OK);
}

void CityMap::loadCBJ(String new_map_file_path) {
	map_file_path = new_map_file_path;
	
	ifstream ifs(map_file_path.toUTF8().c_str());
	
	std::stringstream strstream;
	strstream << ifs.rdbuf();
	ifs.close();
	
	string map_xor(strstream.str());
	string map_data = str_xor(map_xor, "citiesboxmapdatafilexor");
	
	saveTextFile("./data/map_temp.cbj_temp", map_data);
	
	JSONReader map_file(U"./data/map_temp.cbj_temp");
	remove("./data/map_temp.cbj_temp");
	
	saved_version = map_file[U"Version"].get<int>();
	
	addon_set = map_file[U"Addon_Set"].getString();
	// -> アドオン読み込み
	loadAddons(addon_set.toUTF8());
	
	city_name = map_file[U"City_Name"].getString();
	
	mayor_name = map_file[U"Mayor_Name"].getString();
	
	total_population = map_file[U"Total_Population"].get<int>();
	
	change_weather = map_file[U"Change_Weather"].get<bool>();
	
	temperature = map_file[U"Temperature"].get<int>();
	
	dark_on_night = map_file[U"Dark_on_Night"].get<bool>();
	
	mapsize.width = map_file[U"Map_size.width"].get<int>();
	mapsize.height = map_file[U"Map_size.height"].get<int>();
	
	time.year = map_file[U"Time.year"].get<int>();
	time.month = map_file[U"Time.month"].get<int>();
	time.date = map_file[U"Time.date"].get<int>();
	time.hour = map_file[U"Time.hour"].get<int>();
	time.minutes = map_file[U"Time.minutes"].get<int>();
	
	demand.residential = map_file[U"Demand.residential"].get<int>();
	demand.commercial = map_file[U"Demand.commercial"].get<int>();
	demand.office = map_file[U"Demand.office"].get<int>();
	demand.industrial = map_file[U"Demand.industrial"].get<int>();
	demand.farm = map_file[U"Demand.farm"].get<int>();
	
	money = map_file[U"Money"].get<int>();
	
	budget.police = map_file[U"Budget.police"].get<int>();
	budget.fire_depertment = map_file[U"Budget.fire_depertment"].get<int>();
	budget.post_office = map_file[U"Budget.post_office"].get<int>();
	budget.education = map_file[U"Budget.education"].get<int>();
	
	tax.residential = map_file[U"Tax.residential"].get<int>();
	tax.commercial = map_file[U"Tax.commercial"].get<int>();
	tax.office = map_file[U"Tax.office"].get<int>();
	tax.industrial = map_file[U"Tax.industrial"].get<int>();
	tax.farm = map_file[U"Tax.farm"].get<int>();
	
	int y = 0;
	for (const auto& map_squares : map_file[U"Map"].arrayView()) {
		squares.push_back(Array<SquareStruct>());
		int x = 0;
		for (const auto& square : map_squares.arrayView()) {
			squares[y].push_back(SquareStruct());
			
			for (const auto& j_addons : square[U"addons"].arrayView()) {
				squares[y][x].addon_name.push_back(j_addons[U"name"].getString());
				squares[y][x].category.push_back(j_addons[U"category"].getString());
				squares[y][x].type_number.push_back(j_addons[U"type_number"].get<int>());
				squares[y][x].direction_number.push_back(j_addons[U"direction_number"].get<int>());
				
				// アドオンのポインタを登録
				if (addons.find(squares[y][x].addon_name.back().toUTF8()) != addons.end()) {
					squares[y][x].addons.push_back(addons[squares[y][x].addon_name.back().toUTF8()]);
				}
			}
			
			squares[y][x].use_tiles.x = square[U"use_tiles.x"].get<int>();
			squares[y][x].use_tiles.y = square[U"use_tiles.y"].get<int>();
			
			squares[y][x].tiles_count.x = square[U"tiles_count.x"].get<int>();
			squares[y][x].tiles_count.y = square[U"tiles_count.y"].get<int>();
			
			squares[y][x].serial_number = square[U"serial_number"].get<int>();
			
			squares[y][x].residents = square[U"residents"].get<int>();
			
			squares[y][x].workers.commercial = square[U"workers.commercial"].get<int>();
			squares[y][x].workers.office = square[U"workers.office"].get<int>();
			squares[y][x].workers.industrial = square[U"workers.industrial"].get<int>();
			squares[y][x].workers.farm = square[U"workers.farm"].get<int>();
			squares[y][x].workers.public_facility = square[U"workers.public"].get<int>();
			
			squares[y][x].students = square[U"students"].get<int>();
			
			squares[y][x].land_price = square[U"land_price"].get<int>();
			
			squares[y][x].crime_rate = square[U"crime_rate"].get<int>();
			
			squares[y][x].education_rate = square[U"education_rate"].get<int>();
			
			squares[y][x].happiness_rate = square[U"happiness_rate"].get<int>();
			
			squares[y][x].crop.name = square[U"crop.name"].getString();
			squares[y][x].crop.amount = square[U"crop.amount"].get<int>();
			
			squares[y][x].age = square[U"age"].getArray<int>();
			
			squares[y][x].gender = square[U"gender"].getArray<String>();
			
			for (const auto& work_places : square[U"work_places"].arrayView()) {
				squares[y][x].work_places.push_back(WorkPlaceStruct());
				
				squares[y][x].work_places.back().work_place = getRCOIFP(work_places[U"work_kind"].get<int>());
				squares[y][x].work_places.back().work_places_serial_number = work_places[U"serial_number"].get<int>();
			}
			
			for (const auto& school : square[U"school"].arrayView()) {
				squares[y][x].schools.push_back(SchoolStruct());
				
				squares[y][x].schools.back().school = getSchool(school[U"school_kind"].get<int>());
				squares[y][x].schools.back().school_serial_number = school[U"serial_number"].get<int>();
			}
			
			squares[y][x].reservation = getRCOIFP(square[U"reservation"].get<int>());
			
			squares[y][x].original_name = square[U"original_name"].getString();
			
 			x++;
		}
		y++;
	}
}

void CityMap::loadAddons(string addon_set_name) {
	Array<FileStruct> addons_path = specific::getAllFilesName("./addons", "adat");
	
	for (int i=0; i<addons_path.size(); i++) {
		FileStruct file_temp = addons_path[i];
		
		Addon* new_addon = new Addon();
		if (new_addon->load(addons_path[i], addon_set_name)) {
			addons[splitUTF8(file_temp.file_name, ".")[0]] = new_addon;
		}
		
		System::Sleep(100);
	}
}

void CityMap::drawSquare(CoordinateStruct coordinate, CameraStruct camera) {
	// 描画する座標を算出
	for (int i=(int)squares[coordinate.y][coordinate.x].addons.size()-1; i>=0; i--) {
		squares[coordinate.y][coordinate.x].addons[i]->draw(squares[coordinate.y][coordinate.x].addons[i]->getTypeName(squares[coordinate.y][coordinate.x].type_number[i]), squares[coordinate.y][coordinate.x].addons[i]->getDirectionName(squares[coordinate.y][coordinate.x].type_number[i], squares[coordinate.y][coordinate.x].direction_number[i]), coordinateToPosition(coordinate, camera), squares[coordinate.y][coordinate.x].use_tiles, squares[coordinate.y][coordinate.x].tiles_count);
	}
}

void CityMap::draw(CameraStruct camera, CursorStruct& cursor) {
	// マップを描画
	for (short int y=getDrawArea(camera).first.y; y<getDrawArea(camera).second.y; y++) {
		for (short int x=getDrawArea(camera).first.x; x<getDrawArea(camera).second.x; x++) {
			PositionStruct draw_pos = coordinateToPosition(CoordinateStruct{x, y}, camera);
			
			// 一マス分描画
			if (draw_pos.x >= -CHIP_SIZE && draw_pos.y >= -CHIP_SIZE/2 && draw_pos.x <= Scene::Width() && draw_pos.y <= Scene::Height() + CHIP_SIZE*2) {
				drawSquare(CoordinateStruct{x, y}, camera);
			}
			
			// カーソルの描画
			if (x == cursor.coordinate.x && y == cursor.coordinate.y) {
				cursor.texture->draw(cursor.position.x, cursor.position.y, Alpha(128));
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
	ret.x = (temp_my + temp_mx/2) / (CHIP_SIZE/2);
	ret.y = (-temp_mx + temp_my*2) / CHIP_SIZE;
	
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

bool CityMap::save() {
	// JsonWriterを宣言
	JSONWriter map_file;
	
	map_file.startObject();
	{
		map_file.key(U"Version").write(RELEASE_NUMBER);
		map_file.key(U"Addon_Set").write(addon_set);
		map_file.key(U"City_Name").write(city_name);
		map_file.key(U"Mayor_Name").write(mayor_name);
		map_file.key(U"Total_Population").write(total_population);
		map_file.key(U"Change_Weather").write(change_weather);
		map_file.key(U"Temperature").write(temperature);
		map_file.key(U"Dark_on_Night").write(dark_on_night);
		
		map_file.key(U"Map_size").startObject();
		{
			map_file.key(U"width").write(mapsize.width);
			map_file.key(U"height").write(mapsize.height);
		}
		map_file.endObject();
		
		map_file.key(U"Time").startObject();
		{
			map_file.key(U"year").write(time.year);
			map_file.key(U"month").write(time.month);
			map_file.key(U"date").write(time.date);
			map_file.key(U"hour").write(time.hour);
			map_file.key(U"minutes").write(time.minutes);
		}
		map_file.endObject();
		
		map_file.key(U"Demand").startObject();
		{
			map_file.key(U"residential").write(demand.residential);
			map_file.key(U"commercial").write(demand.commercial);
			map_file.key(U"office").write(demand.office);
			map_file.key(U"industrial").write(demand.industrial);
			map_file.key(U"farm").write(demand.farm);
		}
		map_file.endObject();
		
		map_file.key(U"Money").write(money);
		
		map_file.key(U"Budget").startObject();
		{
			map_file.key(U"police").write(budget.police);
			map_file.key(U"fire_depertment").write(budget.fire_depertment);
			map_file.key(U"post_office").write(budget.post_office);
			map_file.key(U"education").write(budget.education);
		}
		map_file.endObject();
		
		map_file.key(U"Tax").startObject();
		{
			map_file.key(U"residential").write(tax.residential);
			map_file.key(U"commercial").write(tax.commercial);
			map_file.key(U"office").write(tax.office);
			map_file.key(U"industrial").write(tax.industrial);
			map_file.key(U"farm").write(tax.farm);
		}
		map_file.endObject();
		
		map_file.key(U"Map").startArray();
		{
			for (int y=0; y<mapsize.height; y++) {
				map_file.startArray();
				{
					for (int x=0; x<mapsize.width; x++) {
						map_file.startObject();
						{
							map_file.key(U"addons").startArray();
							{
								for (int i=0; i<squares[y][x].addons.size(); i++) {
									map_file.startObject();
									{
										map_file.key(U"name").write(squares[y][x].addon_name[i]);
										map_file.key(U"category").write(squares[y][x].category[i]);
										map_file.key(U"type_number").write(squares[y][x].type_number[i]);
										map_file.key(U"direction_number").write(squares[y][x].direction_number[i]);
									}
									map_file.endObject();
								}
							}
							map_file.endArray();
							
							map_file.key(U"use_tiles").startObject();
							{
								map_file.key(U"x").write(squares[y][x].use_tiles.x);
								map_file.key(U"y").write(squares[y][x].use_tiles.y);
							}
							map_file.endObject();
							
							map_file.key(U"tiles_count").startObject();
							{
								map_file.key(U"x").write(squares[y][x].tiles_count.x);
								map_file.key(U"y").write(squares[y][x].tiles_count.y);
							}
							map_file.endObject();
							
							map_file.key(U"serial_number").write(squares[y][x].serial_number);
							
							map_file.key(U"residents").write(squares[y][x].residents);
							
							map_file.key(U"workers").startObject();
							{
								map_file.key(U"commercial").write(squares[y][x].workers.commercial);
								map_file.key(U"office").write(squares[y][x].workers.office);
								map_file.key(U"industrial").write(squares[y][x].workers.industrial);
								map_file.key(U"farm").write(squares[y][x].workers.farm);
								map_file.key(U"public").write(squares[y][x].workers.public_facility);
							}
							map_file.endObject();
							
							map_file.key(U"students").write(squares[y][x].students);
							
							map_file.key(U"land_price").write(squares[y][x].land_price);
							
							map_file.key(U"crime_rate").write(squares[y][x].crime_rate);
							
							map_file.key(U"education_rate").write(squares[y][x].education_rate);
							
							map_file.key(U"happiness_rate").write(squares[y][x].happiness_rate);
							
							map_file.key(U"crop").startObject();
							{
								map_file.key(U"name").write(squares[y][x].crop.name);
								map_file.key(U"amount").write(squares[y][x].crop.amount);
							}
							map_file.endObject();
							
							map_file.key(U"age").startArray();
							{
								for (int i=0; i<squares[y][x].age.size(); i++) {
									map_file.write(squares[y][x].age[i]);
								}
							}
							map_file.endArray();
							
							map_file.key(U"gender").startArray();
							{
								for (int i=0; i<squares[y][x].gender.size(); i++) {
									map_file.write(squares[y][x].gender[i]);
								}
							}
							map_file.endArray();
							
							map_file.key(U"work_places").startArray();
							{
								for (int i=0; i<squares[y][x].work_places.size(); i++) {
									map_file.startObject();
									{
										map_file.key(U"work_kind").write(squares[y][x].work_places[i].work_place);
										map_file.key(U"serial_number").write(squares[y][x].work_places[i].work_places_serial_number);
									}
									map_file.endObject();
								}
							}
							map_file.endArray();
							
							map_file.key(U"school").startArray();
							{
								for (int i=0; i<squares[y][x].schools.size(); i++) {
									map_file.startObject();
									{
										map_file.key(U"school_kind").write(squares[y][x].schools[i].school);
										map_file.key(U"serial_number").write(squares[y][x].schools[i].school_serial_number);
									}
									map_file.endObject();
								}
							}
							map_file.endArray();
							
							map_file.key(U"reservation").write(squares[y][x].reservation);
							
							map_file.key(U"original_name").write(squares[y][x].original_name);
						}
						map_file.endObject();
					}
				}
				map_file.endArray();
			}
		}
		map_file.endArray();
	}
	map_file.endObject();
	
	saveTextFile(map_file_path.toUTF8()+".cbj", str_xor(map_file.get().toUTF8(), "citiesboxmapdatafilexor"));
	//saveTextFile(map_file_path.toUTF8()+".cbj", map_file.get().toUTF8());
	//map_file.save(map_file_path+U".cbj");
	
	return true;
}

void CityMap::freeMapAndAddons() {
	for (auto i = addons.begin(); i != addons.end() ; i++) {
		free(i->second);
	}
	
	Array<Array<SquareStruct>>().swap(squares);
}
