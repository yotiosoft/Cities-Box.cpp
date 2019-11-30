//
//  Images.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//
#pragma once

#ifndef Images_hpp
#define Images_hpp

#include "Header.hpp"
#include "IOfunctions.hpp"
#include "OtherFunctions.hpp"

#endif /* Images_hpp */

typedef struct Images {
	// 指定したパスから画像ファイルをすべて読み込み
	void load(string file_path, string tag);
	void load(string file_path, string tag, int transparent_r, int transparent_g, int transparent_b);

	// 画像の格納する構造体の連想配列
	map<string, map<string, ImageStruct>> images;
} Images;
