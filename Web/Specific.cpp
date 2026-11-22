//
//  Specific.cpp
//  Cities Box
//
//  注 : これはLinux向けです
//
//  Created by YotioSoft on 2021/04/03.
//

#include "Specific.hpp"
#include <emscripten.h>

EM_ASYNC_JS(int, citiesBoxSelectSaveDestination, (const char* suggestedFileName), {
	const suggestedName = UTF8ToString(suggestedFileName);

	if (typeof window.showSaveFilePicker !== "function") {
		Module.citiesBoxSaveFileHandle = null;
		return 1;
	}

	return await new Promise((resolve) => {
		siv3dRegisterUserAction(async () => {
			try {
				Module.citiesBoxSaveFileHandle = await window.showSaveFilePicker({
					suggestedName,
					types: [{
						description: "Cities Box save data",
						accept: { "application/json": [".cbj"] }
					}],
					excludeAcceptAllOption: true
				});
				resolve(1);
			}
			catch (error) {
				Module.citiesBoxSaveFileHandle = null;
				if (error.name === "AbortError") {
					resolve(0);
					return;
				}
				console.warn("Save picker is unavailable; falling back to a download", error);
				resolve(1);
			}
		});
	});
});

EM_ASYNC_JS(int, citiesBoxExportSaveData, (const char* filePath, const char* downloadFileName), {
	try {
		const path = UTF8ToString(filePath);
		const fileName = UTF8ToString(downloadFileName);
		const data = FS.readFile(path);
		const handle = Module.citiesBoxSaveFileHandle;

		if (handle) {
			const writable = await handle.createWritable();
			await writable.write(data);
			await writable.close();
			Module.citiesBoxSaveFileHandle = null;
			return 1;
		}

		const blob = new Blob([data], { type: "application/json" });
		const url = URL.createObjectURL(blob);
		const link = document.createElement("a");
		link.href = url;
		link.download = fileName;
		link.click();
		setTimeout(() => URL.revokeObjectURL(url), 1000);
		return 1;
	}
	catch (error) {
		console.error("Failed to export save data", error);
		Module.citiesBoxSaveFileHandle = null;
		return 0;
	}
});

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
				newFS.folder_name = UnitaryTools::splitUTF8(folderPath, "/").back();
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
	const Array<FileFilter> filters = { { U"セーブデータ", { U"cbj" } } };
	if (const auto selected = Dialog::OpenFile(filters, specific::SaveDataFilePath)) {
		file_path = selected.value();
		return true;
	}

	return false;
}

bool specific::selectSaveDataDestination(String& file_path, const String& suggested_file_name) {
	String fileName = FileSystem::FileName(suggested_file_name);
	if (fileName.isEmpty()) {
		fileName = U"CitiesBox.cbj";
	}
	if (FileSystem::Extension(fileName).lowercased() != U"cbj") {
		fileName += U".cbj";
	}

	if (!citiesBoxSelectSaveDestination(fileName.toUTF8().c_str())) {
		return false;
	}

	file_path = U"/tmp/" + fileName;
	return true;
}

bool specific::exportSaveData(const String& file_path) {
	const String fileName = FileSystem::FileName(file_path);
	return citiesBoxExportSaveData(file_path.toUTF8().c_str(), fileName.toUTF8().c_str());
}
