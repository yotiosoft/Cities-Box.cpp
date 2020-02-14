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
	
	TextReader map_data(map_file_path);
	string str_temp_utf8;
	String str_temp;
	
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
	
	while (map_data.readLine(str_temp)) {
		str_temp = str_temp.substr(0, str_temp.length()-LINE_FEED_CODE);				// 改行コードは除く
		str_temp_utf8 = str_temp.toUTF8();
		
		getElement(str_temp, U"Version", saved_version);
		getElement(str_temp, U"Addons_Set", addon_set);
		
		// アドオン読み込み
		if (!addon_loaded && addon_set.length() > 0) {
			loadAddons(addon_set);
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
		
		getElement(str_temp, U"Money", money);
		
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
				// マップにAddon_Setが定義されていない場合はNormalとみなしアドオン読み込み
				if (!addon_loaded) {
					addon_set = U"Normal";
					loadAddons(addon_set);
					addon_loaded = true;
				}
				
				// アドオンのポインタを登録
				if (addons.find(temp[x]) != addons.end()) {
					squares[array_count][x].addons << addons[temp[x]];
					
					//squares[array_count][x].category = squares[array_count][x].addons.back()->getCategories();
				}
				else {
					cout << "fuck fuck fuck at " << x << "," << array_count << " name: " << temp[x] << endl;
				}
			}
		}
		
		if (current_array_name == "name2" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (temp[x].length() == 0) {
					continue;
				}
				
				// アドオンのポインタを登録
				if (addons.find(temp[x]) != addons.end()) {
					Addon* addon_temp = squares[array_count][x].addons[0];
					squares[array_count][x].addons.back() = addons[temp[x]];
					squares[array_count][x].addons << addon_temp;
					
					cout << squares[array_count][x].addons[1]->getName() << endl;
					
					/*Array<String> categories = squares[array_count][x].addons.back()->getCategories();
					for (int i=0; i<categories.size(); i++) {
						squares[array_count][x].category.push_back(categories[i]);
					}*/
				}
			}
		}
		/*
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
		}*/
		
		if (current_array_name == "obj_type" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (squares[array_count][x].addons.size() == 2) {
					squares[array_count][x].types << squares[array_count][x].addons[1]->getTypeName(stoi(temp[x].toUTF8()));	// とりあえず同じものを[0]にも入れておく
					squares[array_count][x].types << squares[array_count][x].addons[1]->getTypeName(stoi(temp[x].toUTF8()));
				}
				else {
					squares[array_count][x].types << squares[array_count][x].addons[0]->getTypeName(stoi(temp[x].toUTF8()));
				}
			}
		}
		
		if (current_array_name == "obj_type2" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (squares[array_count][x].addons.size() == 2) {
					squares[array_count][x].types[0] = squares[array_count][x].addons[0]->getTypeName(stoi(temp[x].toUTF8()));
				}
			}
		}
		
		if (current_array_name == "obj_dire" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (squares[array_count][x].addons.size() == 2) {
					squares[array_count][x].directions << squares[array_count][x].addons[1]->getDirectionName(squares[array_count][x].types[1], stoi(temp[x].toUTF8()));	// とりあえず同じものを[0]にも入れておく
					squares[array_count][x].directions << squares[array_count][x].addons[1]->getDirectionName(squares[array_count][x].types[1], stoi(temp[x].toUTF8()));
				}
				else {
					squares[array_count][x].directions << squares[array_count][x].addons[0]->getDirectionName(squares[array_count][x].types[0], stoi(temp[x].toUTF8()));
				}
			}
		}
		
		if (current_array_name == "obj_dire2" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				if (squares[array_count][x].addons.size() == 2) {
					squares[array_count][x].directions[0] = squares[array_count][x].addons[0]->getDirectionName(squares[array_count][x].types[0], stoi(temp[x].toUTF8()));
				}
			}
		}
		
		if (current_array_name == "obj_serial_num" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].serial_number = stoi(temp[x].toUTF8());
			}
		}
		/*
		if (current_array_name == "obj_use_tiles_x" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].use_tiles.x = stoi(temp[x].toUTF8());
				
				if (squares[array_count][x].use_tiles.x == 0) {
					squares[array_count][x].use_tiles.x = 1;
				}
			}
		}
		
		if (current_array_name == "obj_use_tiles_y" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].use_tiles.y = stoi(temp[x].toUTF8());
				
				if (squares[array_count][x].use_tiles.y == 0) {
					squares[array_count][x].use_tiles.y = 1;
				}
			}
		}
		*/
		if (current_array_name == "obj_tiles_x" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].tiles_count.x = stoi(temp[x].toUTF8());
				
				// obj_tiles_xを修正(r140以前のバージョンで保存した場合)
				if (saved_version <= 140) {
					if (squares[array_count][x].addons[0]->getUseTiles(squares[array_count][x].types[0], squares[array_count][x].directions[0]).x > 0) {
						// 左向き
						if (squares[array_count][x].directions[0] == U"left") {
							
						}
						
						// 上向き
						if (squares[array_count][x].directions[0] == U"top") {
							
						}
						
						// 下向き
						if (squares[array_count][x].directions[0] == U"bottom") {
							
						}
						
						// 右向き
						if (squares[array_count][x].directions[0] == U"right") {
							squares[array_count][x].tiles_count.x += squares[array_count][x].addons[0]->getUseTiles(squares[array_count][x].types[0], squares[array_count][x].directions[0]).x - 1;
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
					if (squares[array_count][x].addons[0]->getUseTiles(squares[array_count][x].types[0], squares[array_count][x].directions[0]).y > 0) {
						// 左向き
						if (squares[array_count][x].directions[0] == U"left") {
							
						}
						
						// 上向き
						if (squares[array_count][x].directions[0] == U"top") {
							squares[array_count][x].tiles_count.y = squares[array_count][x].addons[0]->getUseTiles(squares[array_count][x].types[0], squares[array_count][x].directions[0]).y - 1 - squares[array_count][x].tiles_count.y;
						}
						
						// 下向き
						if (squares[array_count][x].directions[0] == U"bottom") {
							squares[array_count][x].tiles_count.y = abs(squares[array_count][x].tiles_count.y);
						}
						
						// 右向き
						if (squares[array_count][x].directions[0] == U"right") {
							squares[array_count][x].tiles_count.y = squares[array_count][x].addons[0]->getUseTiles(squares[array_count][x].types[0], squares[array_count][x].directions[0]).y - 1 - squares[array_count][x].tiles_count.y;
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
				squares[array_count][x].rate[U"land_price"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "happiness" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].rate[U"happiness_rate"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "crime_rate" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].rate[U"crime_rate"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "happiness" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].rate[U"happiness_rate"] = stoi(temp[x].toUTF8());
			}
		}
		
		if (current_array_name == "education_rate" && array_count >= 0) {
			Array<String> temp = split(str_temp, U", ");
			
			for (int x=0; x<mapsize.width; x++) {
				squares[array_count][x].rate[U"education_rate"] = stoi(temp[x].toUTF8());
			}
		}
		/*
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
		*/
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
	
	ifstream ifs(map_file_path.toUTF8().c_str(), ios::in | ios::binary);
	
	std::string map_xor((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	string map_data = stringXOR(map_xor, "citiesboxmapdatafilexor");
	
	saveTextFile("./data/map_temp.cbj_temp", map_data);
	
	JSONReader map_file(U"./data/map_temp.cbj_temp");
	//remove("./data/map_temp.cbj_temp");
	
	saved_version = map_file[U"Version"].get<int>();
	
	addon_set = map_file[U"Addon_Set"].getString();
	// -> アドオン読み込み
	loadAddons(addon_set);
	
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
				//squares[y][x].category.push_back(j_addons[U"category"].getString());
				squares[y][x].types.push_back(j_addons[U"type_number"].getString());
				squares[y][x].directions.push_back(j_addons[U"direction_number"].getString());
				
				// アドオンのポインタを登録
				if (addons.find(j_addons[U"name"].getString()) != addons.end()) {
					squares[y][x].addons.push_back(addons[j_addons[U"name"].getString()]);
				}
				else {
					cout << "Cant't find " << j_addons[U"name"].getString() << endl;
				}
			}
			
			//squares[y][x].use_tiles.x = square[U"use_tiles.x"].get<int>();
			//squares[y][x].use_tiles.y = square[U"use_tiles.y"].get<int>();
			
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
			
			squares[y][x].happiness_rate = square[U"happiness_rate"].get<int>();
			
			// 各率の読み込み
			for (const auto& rate : square[U"rate"].objectView()) {
				squares[y][x].rate[rate.name] = rate.value.get<int>();
			}
			
			/*
			squares[y][x].crop.name = square[U"crop.name"].getString();
			squares[y][x].crop.amount = square[U"crop.amount"].get<int>();
			*/
			
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

void CityMap::loadAddons(String addon_set_name) {
	//Array<FileStruct> addons_path = specific::getAllFilesName("./addons", "adat");
	Array<FileStruct> addons_path = specific::getAllFilesName("./addons", "adj");
	
	for (int i=0; i<addons_path.size(); i++) {
		FileStruct file_temp = addons_path[i];
		
		cout << file_temp.file_path << endl;
		
		Addon* new_addon = new Addon();
		if (new_addon->load(addons_path[i], addon_set_name)) {
			addons[new_addon->getName()] = new_addon;
		}
		else {
			free(new_addon);
		}
		
		System::Sleep(20);
	}
}

Array<Addon*> CityMap::getFitAddons(Array<String> selected_categories) {
	Array<Addon*> ret_addons;
	
	for (auto addon = addons.begin(); addon != addons.end(); addon++) {
		
		Array<String> an_addon_categories = addon->second->getCategories();
		vector<bool> fit = vector<bool>(selected_categories.size(), false);
		
		for (auto category_name = an_addon_categories.begin(); category_name != an_addon_categories.end(); category_name++) {
			for (int i=0; i<selected_categories.size(); i++) {
				if (selected_categories[i] == *category_name) {
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
			ret_addons << addon->second;
		}
	}
	
	return ret_addons;
}

map<String, Addon*> CityMap::getAllAddons() {
	return addons;
}

void CityMap::drawSquare(CoordinateStruct coordinate, CameraStruct camera) {
	// 描画する座標を算出
	for (int i=0; i<(int)squares[coordinate.y][coordinate.x].addons.size(); i++) {
		int rate;
		Color rate_color;
		if (show_rate.length() > 0) {
			rate = getRate(coordinate, show_rate);
			if (show_rate == U"crime_rate") {
				rate_color = getRateColor(rate, false, 0);
			}
			else {
				rate_color = getRateColor(rate, true, 50);
			}
		}
		
		squares[coordinate.y][coordinate.x].addons[i]->draw(squares[coordinate.y][coordinate.x].types[i], squares[coordinate.y][coordinate.x].directions[i], coordinateToPosition(coordinate, camera), squares[coordinate.y][coordinate.x].addons[i]->getUseTiles(squares[coordinate.y][coordinate.x].types[i], squares[coordinate.y][coordinate.x].directions[i]), squares[coordinate.y][coordinate.x].tiles_count, &rate_color);
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
				cursor.texture->draw(cursor.position_per_tiles.x, cursor.position_per_tiles.y, Alpha(128));
			}
		}
	}
}

Array<Addon> CityMap::getAddon(CoordinateStruct coordinate) {
	Array<Addon> ret_addons;
	
	for (int i=0; i<squares[coordinate.y][coordinate.x].addons.size(); i++) {
		ret_addons << *(squares[coordinate.y][coordinate.x].addons[i]);
	}
	
	return ret_addons;
}

RCOIFstruct CityMap::getDemand() {
	return demand;
}

int CityMap::getPopulation() {
	return total_population;
}

int CityMap::getMoney() {
	return money;
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

// いずれかのアドオンがカテゴリに含まれているか
bool CityMap::isInCategories(String search_category, CoordinateStruct coordinate) {
	SquareStruct* current_square = &squares[coordinate.y][coordinate.x];
	
	for (int i=0; i<current_square->addons.size(); i++) {
		if (current_square->addons[i]->isInCategories(search_category)) {
			return true;
		}
	}
	
	return false;
}

// アドオンの設置
bool CityMap::build(CoordinateStruct position, Addon* selected_addon, bool need_to_break) {
	SquareStruct* current_square = &squares[position.y][position.x];
	
	String type, direction;
	Array<CoordinateStruct> need_update;
	if (getBuildTypeAndDirection(position, selected_addon, type, direction, need_update)) {
		CoordinateStruct use_tiles = selected_addon->getUseTiles(type, direction);
		
		if (direction == U"left") {
			position.y += use_tiles.y-1;
		}
		if (direction == U"right") {
			position.x -= use_tiles.x-1;
			position.y += use_tiles.y-1;
		}
		else if (direction == U"top") {
			position.y += use_tiles.y-1;
		}
		
		// 効果を取得
		map<String, EffectStruct> effects = selected_addon->getEffects();
		
		// 中央となる座標を取得
		int center_x = use_tiles.x/2;
		int center_y = use_tiles.y/2;
		
		for (int y=0; abs(y)<use_tiles.y; y--) {
			for (int x=0; abs(x)<use_tiles.x; x++) {
				if (need_to_break && type != U"train_crossing" && type != U"bridge") {
					breaking(CoordinateStruct{position.x+x, position.y+y});
				}
				
				current_square = &squares[position.y+y][position.x+x];
				
				if (type != U"train_crossing" && type != U"bridge") {
					current_square->addons.clear();
					current_square->types.clear();
					current_square->directions.clear();
				}
				
				current_square->types << type;
				current_square->directions << direction;
				
				current_square->serial_number = 0;
				
				current_square->tiles_count = {abs(x), abs(y)};
				
				current_square->residents = 0;
				current_square->workers = {0, 0, 0, 0, 0};
				current_square->students = 0;
				current_square->reservation = RCOIFP::None;
				
				current_square->addons << selected_addon;
				
				// 効果を地図へ反映
				if (abs(x) == center_x && abs(y) == center_y) {
					for (auto effect = effects.begin(); effect != effects.end(); effect++) {
						double effect_per_grid = effect->second.influence / effect->second.grid;
						for (int ey=-effect->second.grid; ey<=effect->second.grid; ey++) {
							for (int ex=-effect->second.grid; ex<=effect->second.grid; ex++) {
								if (isPositionAvailable(CoordinateStruct{position.x+x+ex, position.y+y+ey})) {
									squares[position.y+y+ey][position.x+x+ex].rate[effect->first] += effect_per_grid*max(abs(effect->second.grid-1-ey), abs(effect->second.grid-1-ex));
								}
							}
						}
					}
				}
			}
		}
		
		// (道路などで)周囲のアドオンの修正が必要な場合は修正する
		if (need_update.size() > 0) {
			Array<String> search_categories = {U"road", U"train", U"waterway"};
			
			for (int i=0; i<need_update.size(); i++) {
				
				if (need_update[i].x < 0 || need_update[i].y < 0) {
					continue;
				}
				
				for (int j=0; j<squares[need_update[i].y][need_update[i].x].addons.size(); j++) {
					if (squares[need_update[i].y][need_update[i].x].addons[j]->isInCategories(search_categories)) {
						if (!(need_update[j].x == -1 && need_update[j].y == -1)) {
							cout << "update for " << need_update[i].x << "," << need_update[i].y << endl;
							update(need_update[i], squares[need_update[i].y][need_update[i].x].addons[j], need_update);
						}
					}
				}
			}
		}
	}
	
	return true;
}

void CityMap::update(CoordinateStruct position, Addon* selected_addon, Array<CoordinateStruct>& need_update) {
	SquareStruct* current_square = &squares[position.y][position.x];
	
	// 踏切と橋の場合は更新不要
	for (int i=0; i<current_square->types.size(); i++) {
		if (current_square->types[i] == U"train_crossing" || current_square->types[i] == U"bridge") {
			return;
		}
	}
	
	String type, direction;
	if (getBuildTypeAndDirection(position, selected_addon, type, direction, need_update)) {
		current_square->addons.clear();
		current_square->types.clear();
		current_square->directions.clear();
		
		current_square->types << type;
		current_square->directions << direction;
		
		current_square->serial_number = 0;
		current_square->tiles_count = {0, 0};
		current_square->residents = 0;
		current_square->workers = {0, 0, 0, 0, 0};
		current_square->students = 0;
		current_square->reservation = RCOIFP::None;
		
		current_square->addons << selected_addon;
	}
}

void CityMap::breaking(CoordinateStruct coordinate) {
	SquareStruct* current_square = &squares[coordinate.y][coordinate.x];
	Array<Addon*> break_addons = current_square->addons;
	
	for (int i=0; i<break_addons.size(); i++) {
		current_square = &squares[coordinate.y][coordinate.x];
		CoordinateStruct use_tiles = break_addons[i]->getUseTiles(current_square->types[i], current_square->directions[i]);
		
		CoordinateStruct start_point = moveToAddonStartSquare(coordinate, i);
		current_square = &squares[start_point.y][start_point.x];
		
		// 効果を取得
		map<String, EffectStruct> effects = break_addons[i]->getEffects();
		
		// 中央となる座標を取得
		int center_x = use_tiles.x/2;
		int center_y = use_tiles.y/2;
		
		for (int y=0; abs(y)<use_tiles.y; y--) {
			for (int x=0; x<use_tiles.x; x++) {
				SquareStruct before_break = squares[start_point.y+y][start_point.x+x];
				build(CoordinateStruct{start_point.x+x, start_point.y+y}, addons[U"tile_greenfield"], false);
				
				// 効果を除去
				if (abs(x) == center_x && abs(y) == center_y) {
					for (auto effect = effects.begin(); effect != effects.end(); effect++) {
						double effect_per_grid = effect->second.influence / effect->second.grid;
						for (int ey=-effect->second.grid; ey<=effect->second.grid; ey++) {
							for (int ex=-effect->second.grid; ex<=effect->second.grid; ex++) {
								if (isPositionAvailable(CoordinateStruct{start_point.x+x+ex, start_point.y+y+ey})) {
									squares[start_point.y+y+ey][start_point.x+x+ex].rate[effect->first] -= effect_per_grid*max(abs(effect->second.grid-1-ey), abs(effect->second.grid-1-ex));
								}
							}
						}
					}
				}
				
				if (before_break.addons[i]->isInCategories(U"connectable_type")) {
					Array<CoordinateStruct> need_update;
					
					for (int j=0; j<AROUND_TILES; j++) {
						for (int k=0; k<squares[start_point.y+y+AroundTiles[j].second.y][start_point.x+x+AroundTiles[j].second.x].addons.size(); k++) {
							if ((before_break.addons[i]->isInCategories(U"road") && squares[start_point.y+y+AroundTiles[j].second.y][start_point.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"road")) ||
								(before_break.addons[i]->isInCategories(U"train") && squares[start_point.y+y+AroundTiles[j].second.y][start_point.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"train")) ||
								(before_break.addons[i]->isInCategories(U"waterway") && squares[start_point.y+y+AroundTiles[j].second.y][start_point.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"waterway")) ||
								(before_break.addons[i]->isInCategories(U"airport") && squares[start_point.y+y+AroundTiles[j].second.y][start_point.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"airport"))) {
								need_update << CoordinateStruct{start_point.x+x+AroundTiles[j].second.x, start_point.y+y+AroundTiles[j].second.y};
							}
						}
					}
					
					if (break_addons[i]->isInCategories(U"road")) {
						for (int j=0; j<need_update.size(); j++) {
							if (squares[need_update[j].y][need_update[j].x].addons[i]->isInCategories(U"railroad")) {
								need_update[j] = {-1, -1};
							}
						}
					}
					
					for (int j=0; j<need_update.size(); j++) {
						for (int k=0; k<squares[start_point.y+y+AroundTiles[j].second.y][start_point.x+x+AroundTiles[j].second.x].addons.size(); k++) {
							if (!(need_update[j].x == -1 && need_update[j].y == -1)) {
								cout << "breaking update: " << need_update[j].x << "," << need_update[j].y << endl;
								update(need_update[j], squares[need_update[j].y][need_update[j].x].addons[k], need_update);
							}
						}
					}
					
				}
			}
		}
	}
}

CoordinateStruct CityMap::moveToAddonStartSquare(CoordinateStruct search_coordinate, int addon_number) {
	SquareStruct* search_square = &squares[search_coordinate.y][search_coordinate.x];
	
	search_coordinate.x -= search_square->tiles_count.x;
	search_coordinate.y += search_square->tiles_count.y;
	
	return CoordinateStruct{search_coordinate.x, search_coordinate.y};
}

// 設置する場所に合うTypeとDirectionを取得
bool CityMap::getBuildTypeAndDirection(CoordinateStruct coordinate, Addon* selected_addon, String& ret_type, String& ret_direction, Array<CoordinateStruct>& need_update) {
	// 接続タイプ（道路など）アドオンの場合
	if (selected_addon->isInCategories(U"connectable_type")) {
		// 周囲に道路があるか（建設可能か）確認する
		int total_around_road = 0;
		Array<pair<String, CoordinateStruct>> around_road_coordinate;
		
		bool need_update_more = true;
		if (need_update.size() > 0) {
			need_update_more = false;
		}
		
		for (int i=0; i<AROUND_TILES; i++) {
			CoordinateStruct current_square = {coordinate.x+AroundTiles[i].second.x, coordinate.y+AroundTiles[i].second.y};
			
			if (current_square.x < 0 || current_square.y < 0 || current_square.x >= mapsize.width || current_square.y >= mapsize.height) {
				continue;
			}
			
			for (int j=0; j<squares[current_square.y][current_square.x].addons.size(); j++) {
				// 道路の場合
				if ((selected_addon->isInCategories(U"road") && squares[current_square.y][current_square.x].addons[j]->isInCategories(U"road")) ||
					(selected_addon->isInCategories(U"train") && squares[current_square.y][current_square.x].addons[j]->isInCategories(U"train")) ||
					(selected_addon->isInCategories(U"waterway") && squares[current_square.y][current_square.x].addons[j]->isInCategories(U"waterway")) ||
					(selected_addon->isInCategories(U"airport") && squares[current_square.y][current_square.x].addons[j]->isInCategories(U"airport"))) {
					total_around_road ++;
					around_road_coordinate << AroundTiles[i];
					
					if (need_update_more) {
						need_update << current_square;
					}
					
					break;
				}
			}
		}
		
		// 踏切を設置する必要がある場合
		if (selected_addon->isInCategories(U"road")) {
			for (int i=0; i<squares[coordinate.y][coordinate.x].addons.size(); i++) {
				if (squares[coordinate.y][coordinate.x].addons[i]->isInCategories(U"railroad")) {
					ret_type = U"train_crossing";
					
					if (squares[coordinate.y][coordinate.x].directions[i] == U"width") {
						ret_direction = U"width";
					}
					else {
						ret_direction = U"depth";
					}
					
					for (int j=0; j<need_update.size(); j++) {
						if (squares[need_update[j].y][need_update[j].x].addons[i]->isInCategories(U"railroad")) {
							need_update[j] = {-1, -1};
						}
					}
					
					return true;
				}
			}
		}
		
		// 橋を設置する必要がある場合
		if (selected_addon->isInCategories(U"road")) {
			for (int i=0; i<squares[coordinate.y][coordinate.x].addons.size(); i++) {
				if (squares[coordinate.y][coordinate.x].addons[i]->isInCategories(U"waterway")) {
					ret_type = U"bridge";
					
					if (squares[coordinate.y][coordinate.x].directions[i] == U"width") {
						ret_direction = U"depth";
					}
					else {
						ret_direction = U"width";
					}
					
					for (int j=0; j<need_update.size(); j++) {
						if (squares[need_update[j].y][need_update[j].x].addons[i]->isInCategories(U"waterway")) {
							need_update[j] = {-1, -1};
						}
					}
					
					return true;
				}
			}
		}
		
		// その他
		if (total_around_road == 0) {
			ret_type = U"intersection_cross";
			ret_direction = U"normal";
			return true;
		}
		if (total_around_road == 1) {
			ret_type = U"dead_end";
			ret_direction = around_road_coordinate[0].first;
			return true;
		}
		if (total_around_road == 2) {
			CoordinateStruct road_delta = {0, 0};
			
			for (int i=0; i<2; i++) {
				road_delta.x += around_road_coordinate[i].second.x;
				road_delta.y += around_road_coordinate[i].second.y;
			}
			
			// 縦横方向
			if (road_delta.x == 0 && road_delta.y == 0) {
				ret_type = U"default";
				
				if (around_road_coordinate[0].second.x != 0) {
					ret_direction = U"depth";
				}
				else {
					ret_direction = U"width";
				}
				
				return true;
			}
			// 曲がり角
			else {
				ret_type = U"turn";
				
				if (findStringArray(around_road_coordinate, Array<String>{U"left", U"top"})){
					ret_direction = U"left-top";
					return true;
				}
				if (findStringArray(around_road_coordinate, Array<String>{U"right", U"top"})){
					ret_direction = U"right-top";
					return true;
				}
				if (findStringArray(around_road_coordinate, Array<String>{U"left", U"bottom"})){
					ret_direction = U"left-bottom";
					return true;
				}
				if (findStringArray(around_road_coordinate, Array<String>{U"right", U"bottom"})){
					ret_direction = U"right-bottom";
					return true;
				}
				
				return false;
			}
		}
		if (total_around_road == 3) {
			ret_type = U"intersection_T";
			
			if (findStringArray(around_road_coordinate, Array<String>{U"left", U"top", U"bottom"})){
				ret_direction = U"left-top-bottom";
				return true;
			}
			if (findStringArray(around_road_coordinate, Array<String>{U"right", U"top", U"bottom"})){
				ret_direction = U"right-top-bottom";
				return true;
			}
			if (findStringArray(around_road_coordinate, Array<String>{U"left", U"right", U"top"})){
				ret_direction = U"left-right-top";
				return true;
			}
			if (findStringArray(around_road_coordinate, Array<String>{U"left", U"right", U"bottom"})){
				ret_direction = U"left-right-bottom";
				return true;
			}
			
			return false;
		}
		if (total_around_road == 4) {
			ret_type = U"intersection_cross";
			ret_direction = U"normal";
			return true;
		}
	}
	
	// オブジェクトタイプの場合
	if (selected_addon->isInCategories(U"object_type")) {
		// 周囲に道路があるか確認する
		for (int i=0; i<AROUND_TILES; i++) {
			CoordinateStruct current_square = {coordinate.x+AroundTiles[i].second.x, coordinate.y+AroundTiles[i].second.y};
			
			for (int j=0; j<squares[current_square.y][current_square.x].addons.size(); j++) {
				if (squares[current_square.y][current_square.x].addons[j]->isInCategories(U"road")) {
					String ret_type_temp = U"normal";
					String ret_direction_temp = AroundTiles[i].first;
					
					// 複数のタイルを使う場合、建てる方向に障害物などがないか確認する
					bool cannot_build = false;
					
					int add_x = 1, add_y = 1;
					if (ret_type_temp == U"right") {
						add_x = -1;
					}
					else if (ret_type_temp == U"bottom") {
						add_y = -1;
					}
					
					for (int y=0; y<selected_addon->getUseTiles(ret_type, ret_direction).y; y+=add_y) {
						for (int x=0; y<selected_addon->getUseTiles(ret_type, ret_direction).x; x+=add_x) {
							for (int k=0; k<squares[y][x].addons.size(); k++) {
								if (squares[y][x].addons[k]->isInCategories(U"connectable_type")) {
									cannot_build = true;
									break;
								}
							}
							if (cannot_build) break;
						}
						if (cannot_build) break;
					}
					
					if (!cannot_build) {
						ret_type = ret_type_temp;
						ret_direction = ret_direction_temp;
						return true;
					}
				}
			}
		}
	}
	
	// タイルの場合
	if (selected_addon->isInCategories(U"put_type")) {
		ret_type = U"normal";
		ret_direction = U"normal";
		return true;
	}
	
	return false;					// 存在しない or 設置不可能な場合
}

// アドオンを削除
void CityMap::clear(CoordinateStruct position) {
	SquareStruct* current_square = &squares[position.y][position.x];
	Addon* selected_addon = addons[U"tile_greenfield"];
	
	current_square->addons.clear();
	current_square->types.clear();
	current_square->directions.clear();
	
	current_square->types << U"normal";
	current_square->directions << U"normal";
	current_square->serial_number = 0;
	current_square->tiles_count = {0, 0};
	//current_square->use_tiles = {1, 1};
	current_square->tiles_count = {0, 0};
	current_square->residents = 0;
	current_square->workers = {0, 0, 0, 0, 0};
	current_square->students = 0;
	current_square->reservation = RCOIFP::None;
	
	current_square->addons << selected_addon;
	
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

TimeStruct CityMap::cityTime(int minutes_delta) {
	time.minutes += minutes_delta;
	
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
	return squares[coordinate.y][coordinate.x].rate;
}

int CityMap::getRate(CoordinateStruct coordinate, String rate_name) {
	if (squares[coordinate.y][coordinate.x].rate.find(rate_name) != squares[coordinate.y][coordinate.x].rate.end()) {
		return squares[coordinate.y][coordinate.x].rate[rate_name];
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

void CityMap::setShowRate(String rate_name) {
	show_rate = rate_name;
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
										map_file.key(U"name").write(squares[y][x].addons[i]->getName());
										//map_file.key(U"category").write(squares[y][x].category[i]);
										map_file.key(U"type_number").write(squares[y][x].types[i]);
										map_file.key(U"direction_number").write(squares[y][x].directions[i]);
									}
									map_file.endObject();
								}
							}
							map_file.endArray();
							/*
							map_file.key(U"use_tiles").startObject();
							{
								map_file.key(U"x").write(squares[y][x].use_tiles.x);
								map_file.key(U"y").write(squares[y][x].use_tiles.y);
							}
							map_file.endObject();
							*/
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
							
							map_file.key(U"rate").startObject();
							{
								for (auto rate = squares[y][x].rate.begin(); rate != squares[y][x].rate.end() ; rate++) {
									if (rate->second != 0) {
										map_file.key(rate->first).write(rate->second);
									}
								}
							}
							map_file.endObject();
							
							/*
							map_file.key(U"crop").startObject();
							{
								map_file.key(U"name").write(squares[y][x].crop.name);
								map_file.key(U"amount").write(squares[y][x].crop.amount);
							}
							map_file.endObject();
							 */
							
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
	
	saveTextFile(map_file_path.toUTF8()+".cbj", stringXOR(map_file.get().toUTF8(), "citiesboxmapdatafilexor"));
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
