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

#include "Header.hpp"
#include "OtherFunctions.hpp"

namespace specific {
	Array<FileStruct> getAllFilesName(string folder_path, string extension);
}

#endif /* Specific_h */
