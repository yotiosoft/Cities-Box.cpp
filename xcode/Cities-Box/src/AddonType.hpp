//
//  AddonType.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#ifndef AddonType_hpp
#define AddonType_hpp

#include "Specific.hpp"
#include "AddonLayer.hpp"
#include "AddonDirectionStruct.hpp"

class AddonType {
public:
	// コンストラクタ
	AddonType();
	AddonType(TypeID::Type arg_type_ID, Array<AddonLayer> arg_layers, bool arg_light_on_night);
	
	// 描画
	void draw(TimeStruct time, AddonDirectionStruct direction_id,
			  PositionStruct position, CoordinateStruct coordinate);
	
	string type;
	
	String image;					// アドオン画像のパス
	String nightMask;				// ナイトマスク画像のパス
	Array<String> directionNames;	// typeに含まれる方向
	RGBstruct transparentColor;		// 透過色のRGB値
	
	map<String, AddonDirectionStruct> directions;	// typeに含まれる各方向の情報
	
	Texture texture;				// アドオン画像のテクスチャ
	Texture nightMaskTexture;		// 夜間用マスク画像のテクスチャ
	
private:
	/* プライベート関数 */
	// レイヤの更新
	void m_update_layers(TimeStruct time);
	
	// 有効なレイヤのリストを返す
	Array<bool> m_get_enable_layers_list(TimeStruct time);
	
	/* プライベート変数 */
	// レイヤ
	Array<AddonLayer> m_layers;
	
	// 夜間に電気を灯すか？
	bool m_light_on_night;
	
	Texture m_texture;				// アドオン表示用テクスチャ
	
	
};

#endif /* AddonType_hpp */
