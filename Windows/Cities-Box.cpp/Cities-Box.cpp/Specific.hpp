//
//  SpecificHeader.hpp ( for Windows )
//  Cities Box
//
//  Created by YotioSoft on 2019/11/30.
//
//  注 : このファイルはWindows向けのヘッダファイルです。
//

#pragma once

#include <filesystem>
#include <direct.h>

#include "OtherFunctions.hpp"

/* OS依存の定数など */
#define LINE_FEED_CODE			0			// 改行コードに用いる文字数
#define OS						"Windows"	// OS

namespace specific {
	Array<FileStruct> getAllFilesName(string folderPath, string extension);

	void chdir(string dir);
	void moveToCurrentDir();
	String getFontsDir();

	bool isCursorEntered(Vec2& before_cursor_pos);
	void changeCursor();

	bool openSaveData(String& arg_file_path);

	const String SaveDataFilePath = U"";
}