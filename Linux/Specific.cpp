//
//  Specific.cpp
//  Cities Box
//
//  注 : これはLinux向けです
//
//  Created by YotioSoft on 2021/04/03.
//

#include "Specific.hpp"

Array<FileStruct> specific::getAllFilesName(string folderPath, string extension) {
	Array<FileStruct> retStr;
	const char* targetDirlog = folderPath.c_str();
	const char* targetFileExtension = ("."+extension).c_str();
	DIR* dp = opendir(targetDirlog);
	if (dp == NULL) {
		return retStr;
	}
	
	struct dirent* dent = NULL;
	while(true) {
		dent = readdir(dp);
		if(dent == NULL) {
			break;
		}
		string fileName = dent->d_name;
		
		FileStruct newFS;
		
		if (dent != NULL && fileName != "." && fileName != "..") {
			// サブディレクトリの中身も検索
			if (fileName.find(".") == string::npos) {
				Array<FileStruct> subDirStr = getAllFilesName(folderPath+"/"+fileName, extension);
				copy(subDirStr.begin(), subDirStr.end(), back_inserter(retStr));
			}
			
			if (extension.size() == 0 || fileName.find(targetFileExtension) != string::npos) {
				newFS.file_path = folderPath+"/"+fileName;
				
				newFS.folder_path = folderPath;
				newFS.folder_name = splitUTF8(folderPath, "/").back();
				newFS.file_name = fileName;
				
				retStr.push_back(newFS);
			}
		}
	};
	
	closedir(dp);
	
	return retStr;
}

void specific::chdir(string dir) {
	::chdir(dir.c_str());
}

void specific::moveToCurrentDir() {
	chdir("./");
}

bool specific::isCursorEntered(Vec2 before_cursor_pos) {
	return false;	// Linux版では何もしない
}

void specific::changeCursor() {
	return;			// Linux版では何もしない
}

String specific::getFontsDir() {
	return U"./fonts";
}

bool specific::openSaveData(String& file_path) {
	file_path = U"./data/maps/Sample_City.cbd.cbj";
	return true;
}
