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
#include "Specific.hpp"

#endif /* AddonDirectionStruct_hpp */

typedef struct AddonDirectionStruct {
	DirectionID::Type directionID;
	
	Size size;						// 画像のサイズ
	Size requiredTiles;				// アドオンが占めるマスの数
	
	Point topLeft;					// 左上の座標
	Point bottomRight;				// 右下の座標
} AddonDirectionStruct;
