//
//  OtherFunctions.cpp
//

#include "OtherFunctions.hpp"

vector<string> split(string str, string separator) {
	auto separator_length = separator.length();
	
	vector<string> list = vector<string>();
	
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

void saveTextFile(string file_path, string str) {
	ofstream ofs;
	ofs.open(file_path, ios::out);

	ofs << str;
}

void saveTextFile(string file_path, vector<string> str_v) {
	ofstream ofs;
	ofs.open(file_path, ios::out);

	for (int i = 0; i < str_v.size(); i++) {
		ofs << str_v[i] << endl;
	}
}

void saveTextFile(string file_path, vector<FileStruct> fs) {
	ofstream ofs;
	ofs.open(file_path, ios::out);

	for (int i = 0; i < fs.size(); i++) {
		ofs << fs[i].file_path << endl;
		ofs << "    file_name   : " << fs[i].file_name << endl;
		ofs << "    folder_path : " << fs[i].folder_path << endl;
		ofs << "    folder_name : " << fs[i].folder_name << endl;
	}
}

void saveTextFile(string file_path, FileStruct fs) {
	ofstream ofs;
	ofs.open(file_path, ios::out);

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
