//
//  LoadFiles.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/30.
//

#include "IOfunctions.hpp"

vector<FileStruct> specific::getAllFilesName(string folder_path, string extension) {
	using namespace filesystem;
	directory_iterator iter(folder_path), end;
	error_code err;

	vector<string> s;

	vector<FileStruct> ret_str;

	for (; iter != end && !err; iter.increment(err)) {
		const directory_entry entry = *iter;

		string entry_str = replaceString(entry.path().string(), "\\", "/");
		s.push_back(entry_str);
		vector<string> spl = split(entry_str, "/");
		
		// サブディレクトリの中身も検索
		if (spl.back().find(".") == string::npos) {
			vector<FileStruct> sub_dir_str = getAllFilesName(entry_str, extension);
			copy(sub_dir_str.begin(), sub_dir_str.end(), back_inserter(ret_str));
		}

		FileStruct new_fs;
		if (extension.size() == 0 || entry_str.find(extension) != string::npos) {
			new_fs.file_name = spl.back();
			new_fs.file_path = entry_str;
			new_fs.folder_name = spl.end()[-2];

			for (int i = 0; i < spl.size() - 1; i++) {
				new_fs.folder_path += spl[i] + "/";
			}

			ret_str.push_back(new_fs);
		}
	}

	return ret_str;
}
