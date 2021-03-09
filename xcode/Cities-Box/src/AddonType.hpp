//
//  AddonType.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#ifndef AddonType_hpp
#define AddonType_hpp

#include "Specific.hpp"
#include "Addon.hpp"
#include "AddonDirection.hpp"

class AddonType {
public:
	string type;
	
	String image;					// アドオン画像のパス
	String nightMask;				// ナイトマスク画像のパス
	Array<String> directionNames;	// typeに含まれる方向
	RGBstruct transparentColor;		// 透過色のRGB値
	
	//map<String, AddonDirection> directions;	// typeに含まれる各方向の情報
	
	Texture texture;				// アドオン画像のテクスチャ
	Texture nightMaskTexture;		// 夜間用マスク画像のテクスチャ
	
private:
	Texture m_texture;				// アドオン表示用テクスチャ
	
	
};

#endif /* AddonType_hpp */
