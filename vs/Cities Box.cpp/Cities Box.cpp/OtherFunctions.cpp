//
//  OtherFunctions.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "OtherFunctions.hpp"

Array<string> splitUTF8(string str, string separator) {
	auto separator_length = separator.length();
	
	Array<string> list = Array<string>();
	
	if (separator_length == 0) {
		list.push_back(str);
	} else {
		auto offset = std::string::size_type(0);
		while (1) {
			auto pos = str.find(separator, offset);
			if (pos == std::string::npos) {
				list.push_back(str.substr(offset));
				break;
			}
			list.push_back(str.substr(offset, pos - offset));
			offset = pos + separator_length;
		}
	}
	
	return list;
}
Array<String> split(String str, String separator) {
	string str_utf8 = str.toUTF8();
	string separator_utf8 = separator.toUTF8();
	
	auto separator_length = separator.length();
	
	Array<String> list = Array<String>();
	
	if (separator_length == 0) {
		list.push_back(str);
	} else {
		auto offset = std::string::size_type(0);
		while (1) {
			auto pos = str_utf8.find(separator_utf8, offset);
			if (pos == std::string::npos) {
				list.push_back(Unicode::Widen(str_utf8.substr(offset)));
				break;
			}
			list.push_back(Unicode::Widen(str_utf8.substr(offset, pos - offset)));
			offset = pos + separator_length;
		}
	}
	
	return list;
}

void cMes(Font& font, String str, PositionStruct top_left, PositionStruct bottom_right, Color& color) {
	PositionStruct str_long = getStringTopLeft(font, str, top_left, bottom_right);
	font(str).draw(str_long.x, str_long.y, color);
}
void cMes(Font& font, String str, PositionStruct top_left, SizeStruct size, Color& color) {
	PositionStruct str_long = getStringTopLeft(font, str, top_left, size);
	font(str).draw(str_long.x, str_long.y, color);
}

PositionStruct getStringTopLeft(Font& font, String str, PositionStruct top_left, PositionStruct bottom_right) {
	PositionStruct ret_size;
	ret_size.x = (bottom_right.x + top_left.x) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).w / 2;
	ret_size.y = (bottom_right.y + top_left.y) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).h / 2;
	return ret_size;
}
PositionStruct getStringTopLeft(Font& font, String str, PositionStruct top_left, SizeStruct size) {
	PositionStruct ret_size;
	ret_size.x = (top_left.x * 2 + size.width) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).w / 2;
	ret_size.y = (top_left.y * 2 + size.height) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).h / 2;
	return ret_size;
}

void saveTextFile(string file_path, string str) {
	ofstream ofs;
	ofs.open(file_path, ios::out | ios::binary);
	
	ofs << str;
}

void saveTextFile(string file_path, Array<string> str_v) {
	ofstream ofs;
	ofs.open(file_path, ios::out | ios::binary);
	
	for (int i = 0; i < str_v.size(); i++) {
		ofs << str_v[i] << endl;
	}
}

void saveTextFile(string file_path, Array<FileStruct> fs) {
	ofstream ofs;
	ofs.open(file_path, ios::out | ios::binary);
	
	for (int i = 0; i < fs.size(); i++) {
		ofs << fs[i].file_path << endl;
		ofs << "    file_name   : " << fs[i].file_name << endl;
		ofs << "    folder_path : " << fs[i].folder_path << endl;
		ofs << "    folder_name : " << fs[i].folder_name << endl;
	}
}

void saveTextFile(string file_path, FileStruct fs) {
	ofstream ofs;
	ofs.open(file_path, ios::out | ios::binary);
	
	ofs << fs.file_path << endl;
	ofs << "    file_name   : " << fs.file_name << endl;
	ofs << "    folder_path : " << fs.folder_path << endl;
	ofs << "    folder_name : " << fs.folder_name << endl;
}

string replaceString(string before_str, string search_str, string after_str) {
	string::size_type  pos(before_str.find(search_str));
	
	while (pos != string::npos) {
		before_str.replace(pos, search_str.length(), after_str);
		pos = before_str.find(search_str, pos + after_str.length());
	}
	
	return before_str;
}

RCOIFP::Type getRCOIFP(int number) {
	switch (number) {
		case 0:
			return RCOIFP::Residential;
		case 1:
			return RCOIFP::Commercial;
		case 2:
			return RCOIFP::Office;
		case 3:
			return RCOIFP::Industrial;
		case 4:
			return RCOIFP::Farm;
		case 5:
			return RCOIFP::Public;
		default:
			return RCOIFP::None;
	}
}

School::Type getSchool(int number) {
	switch (number) {
		case 0:
			return School::ElementarySchool;
		case 1:
			return School::JuniorHighSchool;
		case 2:
			return School::HighSchool;
		case 3:
			return School::University;
		default:
			return School::None;
	}
}

string stringXOR(const std::string &data, const std::string &key) {
	std::string result(data.size(), '\0');
	for (std::size_t i = 0; i < data.size(); i++) {
		result[i] = data[i] ^ key[i % key.size()];
	}
	return result;
}

bool findStringArray(Array<pair<String, CoordinateStruct>> arr_str, Array<String> search_str) {
	int total = 0;
	for (int i=0; i<arr_str.size(); i++) {
		for (int j=0; j<search_str.size(); j++) {
			if (arr_str[i].first == search_str[j]) {
				total ++;
			}
		}
	}
	
	if (total == search_str.size()) {
		return true;
	}
	return false;
}
