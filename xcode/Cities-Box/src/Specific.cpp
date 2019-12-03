//
//  Specific.cpp
//  Cities Box
//
//  注 : これはmacOS向けです
//
//  Created by YotioSoft on 2019/12/01.
//

#include "Specific.hpp"

Array<FileStruct> specific::getAllFilesName(string folder_path, string extension) {
	Array<FileStruct> ret_str;
	
	const char* target_dirlog = folder_path.c_str();
	const char* target_file_extension = ("."+extension).c_str();
	DIR* dp = opendir(target_dirlog);
	if (dp == NULL) {
		return ret_str;
	}
	
	struct dirent* dent = NULL;
	while(true) {
		dent = readdir(dp);
		if(dent == NULL) {
			break;
		}
		string file_name = dent->d_name;
		
		FileStruct new_fs;
		
		if (dent != NULL && file_name != "." && file_name != "..") {
			// サブディレクトリの中身も検索
			if (file_name.find(".") == string::npos) {
				Array<FileStruct> sub_dir_str = getAllFilesName(folder_path+"/"+file_name, extension);
				copy(sub_dir_str.begin(), sub_dir_str.end(), back_inserter(ret_str));
			}
			
			if (extension.size() == 0 || file_name.find(target_file_extension) != string::npos) {
				new_fs.file_path = folder_path+"/"+file_name;
				
				new_fs.folder_path = folder_path;
				new_fs.folder_name = split(folder_path, "/").back();
				new_fs.file_name = file_name;
				
				ret_str.push_back(new_fs);
			}
		}
	};
	
	closedir(dp);
	
	return ret_str;
}

