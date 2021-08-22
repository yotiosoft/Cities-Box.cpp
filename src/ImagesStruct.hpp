//
//  Images.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//
#pragma once

#ifndef Images_hpp
#define Images_hpp

#include <Specific.hpp>
#include "UnitaryTools.hpp"

#endif /* Images_hpp */

typedef struct ImagesStruct {
	// 指定したパスから画像ファイルをすべて読み込み
	void load(string filePath, string tag);
	void load(string filePath, string tag, Color transparent);
	
	// 画像の格納する構造体の連想配列
	map<string, map<string, ImageStruct>> images;
} ImagesStruct;
