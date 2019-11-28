//
//  Addon.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "Addon.hpp"

Addon::Addon() {
}

bool Addon::getElement(string str, string search_element_name, string& ret) {
	if (str.find(search_element_name) != string::npos && str.find("=") != string::npos) {
		ret = str.substr(str.find("\"")+1, str.length()-str.find("\"")-3);
		return true;
	}
	return false;
}

bool Addon::getElement(string str, string search_element_name, int& ret) {
	if (str.find(search_element_name) != string::npos && str.find("=") != string::npos) {
		ret = stoi(str.substr(str.find("\"")+1, str.length()-str.find("\"")-3));
		return true;
	}
	return false;
}

bool Addon::getTypes(string str, string search_element_name, vector<string>& ret) {
	string a_ret;
	if (getElement(str, search_element_name, a_ret)) {
		ret = split(a_ret, ", ");
		return true;
	}
	return false;
}

Image Addon::set_alpha_color(string image_file_path, int clear_r, int clear_g, int clear_b) {
	Image image_temp(Unicode::Widen(image_file_path));
	
	for (int h=0; h<image_temp.height(); h++) {
		for (int w=0; w<image_temp.width(); w++) {
			if (image_temp[w][h].r == 0 && image_temp[w][h].g == 0 && image_temp[w][h].b == 0) {
				image_temp[w][h].setA(0);
			}
		}
	}
	
	return image_temp;
}

void Addon::load(FileStruct file_path) {
	// アドオンファイルの読み込み
	ifstream ifs(file_path.file_path.c_str());
	string str_temp;
	
	if (ifs.fail()) {
		cerr << "Failed to open file." << endl;
	}
	
	// 各要素の読み出し
	string current_loading_type = "";
	string current_direction = "";
	
	vector<string> directions_name;
	
	while (getline(ifs, str_temp)) {
		// 名前
		getElement(str_temp, "addon_name", addon_name);
		getElement(str_temp, "addon_jp_name", addon_jp_name);
		
		// 製作者名
		getElement(str_temp, "addon_author", addon_author);
		
		// 説明文
		getElement(str_temp, "addon_summary", addon_summary);
		
		// アイコン画像のパス
		getElement(str_temp, "addon_icon", addon_icon);
		
		// アドオンのtype
		getElement(str_temp, "addon_type", addon_type);
		
		// 最大収容人数
		getElement(str_temp, "maxium_capacity", maxium_capacity);
		
		// 地価
		getElement(str_temp, "land_price_influence", land_price_influence);
		getElement(str_temp, "land_price_influence_grid", land_price_influence_grid);
		
		// 使用するtype
		getTypes(str_temp, "use_types", use_types);
		
		// 各typeの内容を取得
		// 現在読込中のtypeを取得
		for (int i=0; i<use_types.size(); i++) {
			if (str_temp.find(use_types[i]) != string::npos && str_temp.find("{") != string::npos) {
				current_loading_type = use_types[i];
			}
		}
		if (current_loading_type != "") {
			// 画像のパス
			getElement(str_temp, "image", types[current_loading_type].image);
			
			// Textureの設定
			if (types[current_loading_type].image.size() > 0) {
				types[current_loading_type].texture = Texture(set_alpha_color(file_path.folder_path+"/"+types[current_loading_type].image, 0, 0, 0));
			}
			
			// ナイトマスク画像のパス
			getElement(str_temp, "night_mask", types[current_loading_type].night_mask);
			
			// typeに含まれる方向と各方向の情報を取得
			getTypes(str_temp, "direction", directions_name);
			
			for (int i=0; i<directions_name.size(); i++) {
				if (str_temp.find(directions_name[i]+" {") != string::npos) {
					current_direction = directions_name[i];
				}
			}
			if (current_direction != "") {
				AddonDirectionStruct direction_temp;
				
				// アドオンの大きさ
				getElement(str_temp, "size_x", direction_temp.size_width);		// 横
				getElement(str_temp, "size_y", direction_temp.size_height);		// 縦
				
				// アドオンが占めるマスの数
				getElement(str_temp, "chip_x", direction_temp.chip_x);			// 横
				getElement(str_temp, "chip_y", direction_temp.chip_y);			// 縦
				
				// 画像上の左上の座標
				getElement(str_temp, "top_left_x", direction_temp.top_left_x);
				getElement(str_temp, "top_left_y", direction_temp.top_left_y);
				
				// 画面上の右下の座標
				getElement(str_temp, "bottom_right_x", direction_temp.bottom_right_x);
				getElement(str_temp, "bottom_right_y", direction_temp.bottom_right_y);
				
				types[current_loading_type].directions[current_direction] = direction_temp;
			}
		}
	}
}

string Addon::getName() {
	return addon_name;
}

string Addon::getNameJP() {
	return addon_jp_name;
}

string Addon::getAuthorName() {
	return addon_author;
}

string Addon::getSummary() {
	return addon_summary;
}

void Addon::draw(string type_name, string direction_name, PositionStruct position) {
	AddonDirectionStruct direction_temp = types[type_name].directions[direction_name];
	
	types[type_name].texture(direction_temp.top_left_x, direction_temp.top_left_y, direction_temp.bottom_right_x-direction_temp.top_left_x, direction_temp.bottom_right_y-direction_temp.top_left_y)
	 .draw(position.x, position.y);
}
