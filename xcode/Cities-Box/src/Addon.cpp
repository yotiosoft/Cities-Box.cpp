//
//  Addon.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "Addon.hpp"

Addon::Addon() {
}

bool Addon::getElement(String str, String search_element_name, String& ret) {
	string str_utf8 = str.toUTF8();
	
	if (str_utf8.find(search_element_name.toUTF8()) != string::npos && str_utf8.find("=") != string::npos) {
		ret = str.substr(str_utf8.find("\"") + 1, str.length() - (str_utf8.find("\"") + 2));
		return true;
	}
	return false;
}

bool Addon::getElement(String str, String search_element_name, int& ret) {
	string str_utf8 = str.toUTF8();
	
	if (str_utf8.find(search_element_name.toUTF8()) != string::npos && str_utf8.find("=") != string::npos) {
		ret = stoi(str_utf8.substr(str_utf8.find("\"") + 1, str.length() - (str_utf8.find("\"") + 2)));
		return true;
	}
	return false;
}

bool Addon::getTypes(String str, String search_element_name, Array<String>& ret) {
	String a_ret;
	if (getElement(str, search_element_name, a_ret)) {
		ret = split(a_ret, U", ");
		return true;
	}
	return false;
}

void Addon::set_alpha_color(Image& image_temp, Color transparent_rgb) {
	for (int h=0; h<image_temp.height(); h++) {
		for (int w=0; w<image_temp.width(); w++) {
			if (image_temp[h][w].r == transparent_rgb.r && image_temp[h][w].g == transparent_rgb.g && image_temp[h][w].b == transparent_rgb.b) {
				image_temp[h][w].a = 0;
			}
		}
	}
}

bool Addon::load(FileStruct new_file_path, String loading_addons_set_name) {
	if (FileSystem::Extension(Unicode::Widen(new_file_path.file_path)) == U"adat") {
		return loadADAT(new_file_path, loading_addons_set_name);
	}
	else if (FileSystem::Extension(Unicode::Widen(new_file_path.file_path)) == U"adj") {
		return loadADJ(new_file_path, loading_addons_set_name);
	}
	return false;
}

bool Addon::loadADAT(FileStruct new_file_path, String loading_addons_set_name) {
	// アドオンファイルの読み込み
	addon_file_path = new_file_path;
	
	TextReader addon_data(Unicode::Widen(addon_file_path.file_path));
	string str_temp_utf8;
	String str_temp;
	
	// 各要素の読み出し
	String current_loading_type = U"";
	String current_direction = U"";
	
	bool loading_type = false;
	bool loading_direction = false;
	
	Color transparent_color;
	transparent_color.r = 0;
	transparent_color.g = 0;
	transparent_color.b = 0;
	
	bool categories[3] = {false, false, false};
	bool belong = false;
	
	while (addon_data.readLine(str_temp)) {
		str_temp = str_temp.substr(0, str_temp.length()-LINE_FEED_CODE);
		str_temp_utf8 = str_temp.toUTF8();
		
		// 名前
		getElement(str_temp, U"addon_name", addon_name);
		getElement(str_temp, U"addon_jp_name", addon_jp_name);
		
		// 製作者名
		getElement(str_temp, U"addon_author", addon_author);
		
		// 説明文
		getElement(str_temp, U"addon_summary", addon_summary);
		
		// 所属するアドオンセットの名前
		String belong_temp;
		if (getElement(str_temp, U"belong_addons_set_name", belong_temp) && !belong) {
			belong_addons_set_name << belong_temp;
			
			if (belong_temp.length() > 0) {
				if (belong_temp != loading_addons_set_name || !(loading_addons_set_name.length() > 0)) {
					return false;
				}
			}
			
			belong = true;
		}
		
		// アイコン画像のパス
		getElement(str_temp, U"addon_icon", addon_icon);
		
		// アイコンを読み込み
		Image icon_image(FileSystem::ParentPath(Unicode::Widen(addon_file_path.file_path))+addon_icon);
		set_alpha_color(icon_image, Color(0, 0, 0));
		icon_texture = Texture(icon_image);
		
		// アドオンのtype
		String category_name_temp;
		getElement(str_temp, U"addon_type", category_name_temp);
		
		if (category_name_temp.length() > 0 && !categories[0]) {
			string category_name_temp_utf8 = category_name_temp.toUTF8();
			
			transform(category_name_temp_utf8.begin(), category_name_temp_utf8.end(), category_name_temp_utf8.begin(), ::tolower);
			
			if (category_name_temp_utf8.find("road") != string::npos && category_name_temp_utf8.find("railroad") == string::npos) {
				addon_categories << U"road";
				addon_categories << U"car";
			}
			else if (category_name_temp_utf8.find("promenade") != string::npos) {
				addon_categories << U"road";
				addon_categories << U"promenade";
			}
			else if (category_name_temp_utf8.find("railroad") != string::npos) {
				addon_categories << U"train";
				addon_categories << U"railroad";
			}
			else if (category_name_temp_utf8.find("station") != string::npos) {
				addon_categories << U"train";
				addon_categories << U"station";
			}
			else if (category_name_temp_utf8.find("residential") != string::npos) {
				addon_categories << U"residential";
			}
			else if (category_name_temp_utf8.find("commercial") != string::npos) {
				addon_categories << U"commercial";
			}
			else if (category_name_temp_utf8.find("office") != string::npos) {
				addon_categories << U"office";
			}
			else if (category_name_temp_utf8.find("industrial") != string::npos) {
				addon_categories << U"industrial";
			}
			else if (category_name_temp_utf8.find("farm") != string::npos) {
				addon_categories << U"farm";
			}
			else if (category_name_temp_utf8.find("public") != string::npos) {
				addon_categories << U"public";
			}
			else if (category_name_temp_utf8.find("park") != string::npos) {
				addon_categories << U"park";
			}
			else if (category_name_temp_utf8.find("port") != string::npos) {
				addon_categories << U"ship";
				addon_categories << U"port";
			}
			else if (category_name_temp_utf8.find("waterway") != string::npos) {
				addon_categories << U"ship";
				addon_categories << U"waterway";
			}
			else if (category_name_temp_utf8.find("airport") != string::npos) {
				addon_categories << U"airport";
			}
			else if (category_name_temp_utf8.find("tile") != string::npos) {
				addon_categories << U"tile";
			}
			
			if (category_name_temp_utf8.find("low_density") != string::npos) {
				addon_categories << U"low_density";
			}
			else if (category_name_temp_utf8.find("high_density") != string::npos) {
				addon_categories << U"high_density";
			}
			
			if (category_name_temp_utf8.find("two_lane") != string::npos) {
				addon_categories << U"two_lane";
			}
			
			categories[0] = true;
		}
		
		String category_name_temp_2;
		getElement(str_temp, U"addon_type_2", category_name_temp_2);
		
		if (category_name_temp_2.length() > 0 && !categories[1]) {
			string category_name_temp_2_utf8 = category_name_temp_2.toUTF8();
			
			transform(category_name_temp_2_utf8.begin(), category_name_temp_2_utf8.end(), category_name_temp_2_utf8.begin(), ::tolower);
			
			if (category_name_temp_2_utf8.find("city_hall") != string::npos) {
				addon_categories << U"city_hall";
			}
			else if (category_name_temp_2_utf8.find("education") != string::npos) {
				addon_categories << U"education";
			}
			else if (category_name_temp_2_utf8.find("post_office") != string::npos) {
				addon_categories << U"post_office";
			}
			else if (category_name_temp_2_utf8.find("education") != string::npos) {
				addon_categories << U"education";
			}
			else if (category_name_temp_2_utf8.find("fire_depertment") != string::npos) {
				addon_categories << U"fire_depertment";
			}
			else if (category_name_temp_2_utf8.find("police_station") != string::npos) {
				addon_categories << U"police";
				addon_categories << U"police_station";
			}
			
			categories[1] = true;
		}
		
		String category_name_temp_3;
		getElement(str_temp, U"addon_type_3", category_name_temp_3);
		
		if (category_name_temp_3.length() > 0 && !categories[2]) {
			string category_name_temp_3_utf8 = category_name_temp_3.toUTF8();
			
			transform(category_name_temp_3_utf8.begin(), category_name_temp_3_utf8.end(), category_name_temp_3_utf8.begin(), ::tolower);
			
			if (category_name_temp_3_utf8.find("elementary-school") != string::npos) {
				addon_categories << U"elementary_school";
			}
			else if (category_name_temp_3_utf8.find("junior-high-school") != string::npos) {
				addon_categories << U"junior_high_school";
			}
			else if (category_name_temp_3_utf8.find("high-school") != string::npos) {
				addon_categories << U"high_school";
			}
			else if (category_name_temp_3_utf8.find("university") != string::npos) {
				addon_categories << U"university";
			}
			
			categories[2] = true;
		}
		
		// 最大収容人数
		getElement(str_temp, U"maximum_capacity", maximum_capacity);
		
		// 建物の効果
		getElement(str_temp, U"land_price_influence", effects[U"land_price"].influence);
		getElement(str_temp, U"land_price_influence_grid", effects[U"land_price"].grid);
		
		getElement(str_temp, U"education_rate_influence", effects[U"education_rate"].influence);
		getElement(str_temp, U"education_rate_influence_grid", effects[U"education_rate"].grid);
		
		getElement(str_temp, U"crime_rate_influence", effects[U"crime_rate"].influence);
		getElement(str_temp, U"crime_rate_influence_grid", effects[U"crime_rate"].grid);
		
		getElement(str_temp, U"noise_influence", effects[U"noise"].influence);
		getElement(str_temp, U"noise_influence_grid", effects[U"noise"].grid);
		
		// 使用するtype
		getTypes(str_temp, U"use_types", use_types);
		
		// 各typeの内容を取得
		// 現在読込中のtypeを取得
		for (int i=0; i<use_types.size(); i++) {
			if (str_temp_utf8.find(use_types[i].toUTF8() + " {") == 0 && !loading_type) {
				if (use_types[i] == U"null") {
					use_types[i] = U"normal";
				}
				
				current_loading_type = use_types[i];
				loading_type = true;
			}
		}
		if (str_temp_utf8.find("}") == 0 && !loading_direction) {
			// typeが切り替わるときにTextureの設定
			if (types[current_loading_type].image.length() > 0) {
				Image itemp(Unicode::Widen(addon_file_path.folder_path)+U"/"+types[current_loading_type].image);
				set_alpha_color(itemp, transparent_color);
				types[current_loading_type].texture = Texture(itemp);
			}
			
			current_direction = U"";
			loading_type = false;
		}
		
		if (current_loading_type != U"") {
			// 画像のパス
			getElement(str_temp, U"image", types[current_loading_type].image);
			
			// 透過色
			String rgb_str;
			getElement(str_temp, U"transparent_color", rgb_str);
			
			if (rgb_str.length() > 0) {
				Array<String> rgb_strv = split(rgb_str, U", ");
				
				if (rgb_strv.size() == 3) {
					transparent_color.r = stoi(rgb_strv[0].toUTF8());
					transparent_color.g = stoi(rgb_strv[1].toUTF8());
					transparent_color.b = stoi(rgb_strv[2].toUTF8());
				}
			}
			
			// ナイトマスク画像のパス
			getElement(str_temp, U"night_mask", types[current_loading_type].night_mask);
			
			// typeに含まれる方向と各方向の情報を取得
			getTypes(str_temp, U"direction", types[current_loading_type].direction_names);
			
			for (int i=0; i<types[current_loading_type].direction_names.size(); i++) {
				if (str_temp_utf8.find(types[current_loading_type].direction_names[i].toUTF8()+" {") != string::npos && !loading_direction) {
					if (types[current_loading_type].direction_names[i] == U"null") {
						types[current_loading_type].direction_names[i] = U"normal";
					}
					current_direction = types[current_loading_type].direction_names[i];
				}
			}
			if (str_temp_utf8.find("}") != string::npos && loading_direction) {
				current_direction = U"";
				loading_direction = false;
			}
			
			if (current_direction != U"") {
				AddonDirectionStruct direction_temp;
				
				// アドオンの大きさ
				getElement(str_temp, U"size_x", direction_temp.size_width);		// 横
				getElement(str_temp, U"size_y", direction_temp.size_height);		// 縦
				
				// アドオンが占めるマスの数
				getElement(str_temp, U"chip_x", direction_temp.chip_x);			// 横
				getElement(str_temp, U"chip_y", direction_temp.chip_y);			// 縦
				
				// 画像上の左上の座標
				getElement(str_temp, U"top_left_x", direction_temp.top_left_x);
				getElement(str_temp, U"top_left_y", direction_temp.top_left_y);
				
				// 画面上の右下の座標
				getElement(str_temp, U"bottom_right_x", direction_temp.bottom_right_x);
				getElement(str_temp, U"bottom_right_y", direction_temp.bottom_right_y);
				
				types[current_loading_type].directions[current_direction] = direction_temp;
			}
		}
	}
	
	if (!belong) {
		belong_addons_set_name << U"Normal";
	}
	
	converter();
	
	return true;
}

bool Addon::loadADJ(FileStruct new_file_path, String loading_addons_set_name) {
	addon_file_path = new_file_path;
	JSONReader addon_data(Unicode::Widen(addon_file_path.file_path));
	
	belong_addons_set_name = addon_data[U"Belong_addon_set_name"].getArray<String>();
	bool belong = false;
	for (int i=0; i<belong_addons_set_name.size(); i++) {
		if (belong_addons_set_name[i].length() > 0) {
			if (belong_addons_set_name[i] == loading_addons_set_name) {
				belong = true;
			}
		}
	}
	if (!belong) {
		return false;
	}
	
	addon_name = addon_data[U"name"].getString();
	addon_jp_name = addon_data[U"jp_name"].getString();
	
	addon_author = addon_data[U"author"].getString();
	addon_summary = addon_data[U"summary"].getString();
	
	addon_icon = addon_data[U"icon"].getString();
	
	// アイコンを読み込み
	Image icon_image(FileSystem::ParentPath(Unicode::Widen(addon_file_path.file_path))+addon_icon);
	set_alpha_color(icon_image, Color(0, 0, 0));
	icon_texture = Texture(icon_image);
	
	// カテゴリ
	addon_categories = addon_data[U"Categories"].getArray<String>();
	
	// 建物の効果
	for (const auto& effect : addon_data[U"effects"].objectView()) {
		effects[effect.name].influence = effect.value[U"influence"].get<int>();
		effects[effect.name].grid = effect.value[U"grid"].get<int>();
	}
	
	use_types = addon_data[U"Use_types"].getArray<String>();
	
	for (const auto& type : addon_data[U"Types"].arrayView()) {
		String type_name = type[U"type_name"].getString();
		
		types[type_name].image = type[U"image"].getString();
		
		types[type_name].transparent_color.r = type[U"transparent_color.R"].get<int>();
		types[type_name].transparent_color.g = type[U"transparent_color.G"].get<int>();
		types[type_name].transparent_color.b = type[U"transparent_color.B"].get<int>();
		
		Image itemp(Unicode::Widen(addon_file_path.folder_path)+U"/"+types[type_name].image);
		set_alpha_color(itemp, Color(types[type_name].transparent_color.r, types[type_name].transparent_color.g, types[type_name].transparent_color.b));
		types[type_name].texture = Texture(itemp);
		
		types[type_name].direction_names = type[U"direction_names"].getArray<String>();
		
		for (const auto& direction : type[U"Directions"].arrayView()) {
			String direction_name = direction[U"direction_name"].getString();
			
			types[type_name].directions[direction_name].size_width = direction[U"size.width"].get<int>();
			types[type_name].directions[direction_name].size_height = direction[U"size.height"].get<int>();
			
			types[type_name].directions[direction_name].chip_x = direction[U"squares.width"].get<int>();
			types[type_name].directions[direction_name].chip_y = direction[U"squares.height"].get<int>();
			
			types[type_name].directions[direction_name].top_left_x = direction[U"top_left.x"].get<int>();
			types[type_name].directions[direction_name].top_left_y = direction[U"top_left.y"].get<int>();
			
			types[type_name].directions[direction_name].bottom_right_x = direction[U"bottom_right.x"].get<int>();
			types[type_name].directions[direction_name].bottom_right_y = direction[U"bottom_right.y"].get<int>();
		}
	}
	
	return true;
}

String Addon::getName() {
	return addon_name;
}

String Addon::getNameJP() {
	return addon_jp_name;
}

String Addon::getAuthorName() {
	return addon_author;
}

String Addon::getSummary() {
	return addon_summary;
}

String Addon::getTypeName(int type_num) {
	return use_types[type_num];
}

String Addon::getDirectionName(int type_num, int direction_num) {
	return types[getTypeName(type_num)].direction_names[direction_num];
}

Array<String> Addon::getCategories() {
	return addon_categories;
}

void Addon::drawIcon(PositionStruct position, PositionStruct left_top, SizeStruct size) {
	icon_texture(left_top.x, left_top.y, size.width, size.height).draw(position.x, position.y);
}

CoordinateStruct Addon::getUseTiles(int type_num, int direction_num) {
	return CoordinateStruct{types[getTypeName(type_num)].directions[getDirectionName(type_num, direction_num)].chip_x, types[getTypeName(type_num)].directions[getDirectionName(type_num, direction_num)].chip_y};
}

PositionStruct Addon::getPosition(String type_name, String direction_name, PositionStruct position, CoordinateStruct use_tiles, CoordinateStruct tiles_count) {
	AddonDirectionStruct* direction_temp = &(types[type_name].directions[direction_name]);
	if (direction_temp != nullptr)
		position.y = position.y + CHIP_SIZE/2 - direction_temp->size_height + CHIP_SIZE/4 * (max(1, use_tiles.x) - 1 - tiles_count.x) + CHIP_SIZE*3/4 * tiles_count.y;
	
	return position;
}

void Addon::draw(String type_name, String direction_name, PositionStruct position, CoordinateStruct use_tiles, CoordinateStruct tiles_count) {
	AddonDirectionStruct* direction_temp = &(types[type_name].directions[direction_name]);
	
	/*
	if (cursor.coordinate.x == coordinate.x && cursor.coordinate.y == coordinate.y) {
		cout << coordinate.x << "," << coordinate.y << " :  " << tiles_count.x << "," << tiles_count.y << endl;
	}*/
	
	//position.x = position.x + tiles_count.x * CHIP_SIZE/8;
	position = getPosition(type_name, direction_name, position, use_tiles, tiles_count);
	
	unsigned short int top_left_x = direction_temp->top_left_x;
	top_left_x += CHIP_SIZE/2 * tiles_count.x + CHIP_SIZE/2 * tiles_count.y;
	
	unsigned short int top_left_y = direction_temp->top_left_y;
	top_left_y += CHIP_SIZE/2 * tiles_count.y;
	
	unsigned short int size_width = direction_temp->size_width;
	size_width = CHIP_SIZE;
	
	unsigned short int size_height = direction_temp->size_height;
	
	types[type_name].texture(top_left_x, top_left_y, size_width, size_height).draw(position.x, position.y);
}

void Addon::converter() {
	JSONWriter addon_data;
	
	addon_data.startObject();
	{
		addon_data.key(U"name").write(addon_name);
		addon_data.key(U"jp_name").write(addon_jp_name);
		
		addon_data.key(U"author").write(addon_author);
		addon_data.key(U"summary").write(addon_summary);
		
		addon_data.key(U"version").write(RELEASE_NUMBER);
		
		addon_data.key(U"Belong_addon_set_name").startArray();
		{
			for (auto belong = belong_addons_set_name.begin(); belong!= belong_addons_set_name.end() ; belong++) {
				addon_data.write(*belong);
			}
		}
		addon_data.endArray();
		
		addon_data.key(U"icon").write(addon_icon);
		
		addon_data.key(U"Categories").startArray();
		{
			for (auto category_name = addon_categories.begin(); category_name != addon_categories.end(); category_name++) {
				addon_data.write(*category_name);
			}
		}
		addon_data.endArray();
		
		addon_data.key(U"effects").startObject();
		{
			for (auto e = effects.begin(); e != effects.end() ; e++) {
				if (e->second.influence != 0) {
					addon_data.key(e->first).startObject();
					{
						addon_data.key(U"influence").write(e->second.influence);
						addon_data.key(U"grid").write(e->second.grid);
					}
					addon_data.endObject();
				}
			}
		}
		addon_data.endObject();
		
		addon_data.key(U"maximum_capacity").write(maximum_capacity);
		
		addon_data.key(U"Use_types").startArray();
		{
			for (auto type_name = use_types.begin(); type_name != use_types.end() ; type_name++) {
				addon_data.write(*type_name);
			}
		}
		addon_data.endArray();
		
		addon_data.key(U"Types").startArray();
		{
			for (auto type = types.begin(); type != types.end(); type++) {
				addon_data.startObject();
				{
					addon_data.key(U"type_name").write(type->first);
					addon_data.key(U"image").write(type->second.image);
					addon_data.key(U"night_mask").write(type->second.night_mask);
					addon_data.key(U"transparent_color").startObject();
					{
						addon_data.key(U"R").write(type->second.transparent_color.r);
						addon_data.key(U"G").write(type->second.transparent_color.g);
						addon_data.key(U"B").write(type->second.transparent_color.b);
					}
					addon_data.endObject();
					
					addon_data.key(U"direction_names").startArray();
					{
						for (auto direction_name = type->second.direction_names.begin(); direction_name != type->second.direction_names.end(); direction_name++) {
							addon_data.write(*direction_name);
						}
					}
					addon_data.endArray();
					
					addon_data.key(U"Directions").startArray();
					{
						for (auto direction = type->second.directions.begin(); direction != type->second.directions.end(); direction++) {
							addon_data.startObject();
							{
								addon_data.key(U"direction_name").write(direction->first);
								addon_data.key(U"size").startObject();
								{
									addon_data.key(U"width").write(direction->second.size_width);
									addon_data.key(U"height").write(direction->second.size_height);
								}
								addon_data.endObject();
								addon_data.key(U"squares").startObject();
								{
									addon_data.key(U"width").write(direction->second.chip_x);
									addon_data.key(U"height").write(direction->second.chip_y);
								}
								addon_data.endObject();
								addon_data.key(U"top_left").startObject();
								{
									addon_data.key(U"x").write(direction->second.top_left_x);
									addon_data.key(U"y").write(direction->second.top_left_y);
								}
								addon_data.endObject();
								addon_data.key(U"bottom_right").startObject();
								{
									addon_data.key(U"x").write(direction->second.bottom_right_x);
									addon_data.key(U"y").write(direction->second.bottom_right_y);
								}
								addon_data.endObject();
							}
							addon_data.endObject();
						}
						addon_data.endArray();
					}
				}
				addon_data.endObject();
			}
		}
		addon_data.endArray();
	}
	addon_data.endObject();
	
	addon_data.save(FileSystem::ParentPath(Unicode::Widen(addon_file_path.file_path))+FileSystem::BaseName(Unicode::Widen(addon_file_path.file_path))+U".adj");
}
