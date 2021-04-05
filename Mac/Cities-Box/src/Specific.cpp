//
//  Specific.cpp
//  Cities Box
//
//  注 : これはmacOS向けです
//
//  Created by YotioSoft on 2019/12/01.
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
	chdir("./Cities-Box.app/Contents/Resources/assets");
}

bool specific::isCursorEntered(Vec2 before_cursor_pos) {
	return false;	// Mac版では何もしない
}

void specific::changeCursor() {
	return;			// Mac版では何もしない
}

bool specific::openSaveData(String& arg_file_path) {
	Array<FileFilter> ff = {{U"セーブデータ", {/*U"cbd", */U"cbj"}}};
	if (const auto open = Dialog::OpenFile(ff, specific::SaveDataFilePath)) {
		arg_file_path = open.value();
		
		return true;
	}
	
	return false;
}

String specific::getFontsDir() {
	return U"./fonts";
}
