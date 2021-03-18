//
//  OtherFunctions.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "OtherFunctions.hpp"

Array<string> splitUTF8(string str, string separator) {
	auto separatorLength = separator.length();
	
	Array<string> list = Array<string>();
	
	if (separatorLength == 0) {
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
			offset = pos + separatorLength;
		}
	}
	
	return list;
}
Array<String> split(String str, String separator) {
	string strUTF8 = str.toUTF8();
	string separatorUTF8 = separator.toUTF8();
	
	auto separatorLength = separator.length();
	
	Array<String> list = Array<String>();
	
	if (separatorLength == 0) {
		list.push_back(str);
	} else {
		auto offset = std::string::size_type(0);
		while (1) {
			auto pos = strUTF8.find(separatorUTF8, offset);
			if (pos == std::string::npos) {
				list.push_back(Unicode::Widen(strUTF8.substr(offset)));
				break;
			}
			list.push_back(Unicode::Widen(strUTF8.substr(offset, pos - offset)));
			offset = pos + separatorLength;
		}
	}
	
	return list;
}

void cMes(Font& font, String str, PositionStruct topLeft, PositionStruct bottomRight, Color& color) {
	PositionStruct str_long = getStringTopLeft(font, str, topLeft, bottomRight);
	font(str).draw(str_long.x, str_long.y, color);
}
void cMes(Font& font, String str, PositionStruct topLeft, Size size, Color& color) {
	PositionStruct str_long = getStringTopLeft(font, str, topLeft, size);
	font(str).draw(str_long.x, str_long.y, color);
}

PositionStruct getStringTopLeft(Font& font, String str, PositionStruct topLeft, PositionStruct bottomRight) {
	PositionStruct retSize;
	retSize.x = (bottomRight.x + topLeft.x) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).w / 2;
	retSize.y = (bottomRight.y + topLeft.y) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).h / 2;
	return retSize;
}
PositionStruct getStringTopLeft(Font& font, String str, PositionStruct topLeft, Size size) {
	PositionStruct retSize;
	retSize.x = (topLeft.x * 2 + size.x) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).w / 2;
	retSize.y = (topLeft.y * 2 + size.y) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).h / 2;
	return retSize;
}

void saveTextFile(string filePath, string str) {
	ofstream ofs;
	ofs.open(filePath, ios::out | ios::binary);
	
	ofs << str;
}

void saveTextFile(string filePath, Array<string> strV) {
	ofstream ofs;
	ofs.open(filePath, ios::out | ios::binary);
	
	for (int i = 0; i < strV.size(); i++) {
		ofs << strV[i] << endl;
	}
}

void saveTextFile(string filePath, Array<FileStruct> fs) {
	ofstream ofs;
	ofs.open(filePath, ios::out | ios::binary);
	
	for (int i = 0; i < fs.size(); i++) {
		ofs << fs[i].file_path << endl;
		ofs << "    file_name   : " << fs[i].file_name << endl;
		ofs << "    folder_path : " << fs[i].folder_path << endl;
		ofs << "    folder_name : " << fs[i].folder_name << endl;
	}
}

void saveTextFile(string filePath, FileStruct fs) {
	ofstream ofs;
	ofs.open(filePath, ios::out | ios::binary);
	
	ofs << fs.file_path << endl;
	ofs << "    file_name   : " << fs.file_name << endl;
	ofs << "    folder_path : " << fs.folder_path << endl;
	ofs << "    folder_name : " << fs.folder_name << endl;
}

string replaceString(string beforeStr, string searchStr, string afterStr) {
	string::size_type pos(beforeStr.find(searchStr));
	
	while (pos != string::npos) {
		beforeStr.replace(pos, searchStr.length(), afterStr);
		pos = beforeStr.find(searchStr, pos + afterStr.length());
	}
	
	return beforeStr;
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

bool findStringArray(Array<pair<String, CoordinateStruct>> arrStr, Array<String> searchStr) {
	int total = 0;
	for (int i=0; i<arrStr.size(); i++) {
		for (int j=0; j<searchStr.size(); j++) {
			if (arrStr[i].first == searchStr[j]) {
				total ++;
			}
		}
	}
	
	if (total == searchStr.size()) {
		return true;
	}
	return false;
}
