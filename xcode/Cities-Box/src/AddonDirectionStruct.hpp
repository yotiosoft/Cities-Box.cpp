//
//  AddonDirection.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#pragma once

#ifndef AddonDirectionStruct_hpp
#define AddonDirectionStruct_hpp

#include <stdio.h>

#endif /* AddonDirectionStruct_hpp */

typedef struct AddonDirectionStruct {
	string direction;
	
	int sizeWidth;					// 画像の横方向のサイズ
	int sizeHeight;					// 画像の縦方向のサイズ
	int chipX;						// アドオンが占める横方向のマスの数
	int chipY;						// アドオンが占める縦方向のマスの数
	
	int topLeftX;					// 左上のx座標
	int topLeftY;					// 左上のy座標
	int bottomRightX;				// 右下のx座標
	int bottomRightY;				// 右下のy座標
} AddonDirectionStruct;
