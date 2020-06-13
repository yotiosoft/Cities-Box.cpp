//
//  Specific.hpp
//  Cities Box
//
//  注 : これはmacOS向けです
//
//  Created by YotioSoft on 2019/12/01.
//

#ifndef Specific_h
#define Specific_h

#include <unistd.h>
#include <dirent.h>
#include <random>

#include "Header.hpp"
#include "OtherFunctions.hpp"

/* OS依存の定数など */
#define LINE_FEED_CODE			0	// 改行コードに用いる文字数

namespace specific {
	Array<FileStruct> getAllFilesName(string folderPath, string extension);
}

#endif /* Specific_h */
