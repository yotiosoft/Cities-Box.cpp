//
//  Specific.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/30.
//

#include "Specific.hpp"

Array<FileStruct> specific::getAllFilesName(string folder_path, string extension) {
	using namespace filesystem;
	directory_iterator iter(folder_path), end;
	error_code err;

	Array<string> s;

	Array<FileStruct> ret_str;

	for (; iter != end && !err; iter.increment(err)) {
		const directory_entry entry = *iter;

		string entry_str = replaceString(entry.path().string(), "\\", "/");
		s.push_back(entry_str);
		Array<string> spl = splitUTF8(entry_str, "/");
		
		// サブディレクトリの中身も検索
		if (spl.back().find(".") == string::npos) {
			Array<FileStruct> sub_dir_str = getAllFilesName(entry_str, extension);
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

void specific::chdir(string dir) {
	::_chdir(dir.c_str());
}

void specific::moveToCurrentDir() {
	return;		// 何もしない
}

bool specific::isCursorEntered(Vec2& before_cursor_pos) {
	Vec2 cursor_pos = Cursor::Pos();
	if (before_cursor_pos.x < 0 || before_cursor_pos.y < 0 || before_cursor_pos.x > Scene::Size().x || before_cursor_pos.y > Scene::Size().y) {
		if (cursor_pos.x >= 0 || cursor_pos.y >= 0 || cursor_pos.x <= Scene::Size().x || cursor_pos.y <= Scene::Size().y) {
			Cursor::RequestStyle(CursorStyle::Hidden);

			before_cursor_pos = cursor_pos;
			return true;
		}
	}
	before_cursor_pos = cursor_pos;
	return false;
}

void specific::changeCursor() {
	Cursor::RequestStyle(CursorStyle::Arrow);
}

bool specific::openSaveData(String& arg_file_path) {
	Array<FileFilter> ff = { {U"セーブデータ", {/*U"cbd", */U"cbj"}} };
	if (const auto open = Dialog::OpenFile(ff, specific::SaveDataFilePath)) {
		arg_file_path = open.value();

		return true;
	}

	return false;
}

String specific::getFontsDir() {
	return U"example/font";
}
