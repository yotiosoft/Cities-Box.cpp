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
		getElement(str_temp, U"belong_addons_set_name", belong_addons_set_name);
		
		if (belong_addons_set_name.length() > 0) {
			if (belong_addons_set_name != loading_addons_set_name || !(loading_addons_set_name.length() > 0)) {
				return false;
			}
		}
		
		// アイコン画像のパス
		getElement(str_temp, U"addon_icon", addon_icon);
		
		// アドオンのtype
		getElement(str_temp, U"addon_type", addon_types);
		
		// 最大収容人数
		getElement(str_temp, U"maxium_capacity", maxium_capacity);
		
		// 地価
		getElement(str_temp, U"land_price_influence", land_price_influence);
		getElement(str_temp, U"land_price_influence_grid", land_price_influence_grid);
		
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
				
				direction_names.push_back(Array<String>());
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
			getTypes(str_temp, U"direction", direction_names.back());
			
			for (int i=0; i<direction_names.back().size(); i++) {
				if (str_temp_utf8.find(direction_names.back()[i].toUTF8()+" {") != string::npos && !loading_direction) {
					if (direction_names.back()[i] == U"null") {
						direction_names.back()[i] = U"normal";
					}
					current_direction = direction_names.back()[i];
					types[current_loading_type].direction_names.push_back(direction_names.back()[i]);
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
	
	converter();
	
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
	return direction_names[type_num][direction_num];
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
		
		addon_data.key(U"icon").write(addon_icon);
		
		addon_data.key(U"types_names").write(addon_types);
		addon_data.key(U"use_types").write(use_types);
		
		addon_data.key(U"Types").startArray();
		{
			for (auto type = types.begin(); type != types.end() ; type++) {
				addon_data.startObject();
				{
					addon_data.key(U"type_name").write(type->first);
					addon_data.key(U"image").write(type->second.image);
					addon_data.key(U"transparent_color").startObject();
					{
						addon_data.key(U"R").write(type->second.transparent_color.r);
						addon_data.key(U"G").write(type->second.transparent_color.g);
						addon_data.key(U"B").write(type->second.transparent_color.b);
					}
					addon_data.endObject();
					
					addon_data.key(U"direction_names").write(type->second.direction_names);
					
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
