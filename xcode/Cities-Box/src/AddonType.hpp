//
//  AddonType.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#ifndef AddonType_hpp
#define AddonType_hpp

#pragma once

#include "Specific.hpp"
#include "AddonLayer.hpp"
#include "AddonDirectionStruct.hpp"

class AddonType {
public:
	// コンストラクタ
	AddonType();
	AddonType(TypeID::Type arg_type_ID);
	
	// 描画
	void draw(TimeStruct time, AddonDirectionStruct direction_id,
			  PositionStruct position, CoordinateStruct coordinate);
	
	// 暫定
	Texture tempGetTexture(TimeStruct time);
	
	void setLayers(Array<AddonLayer> arg_layers);
	
	// レイヤ数のカウント
	int countLayers();
	
	// AddonDirectionStructをm_directionsに追加
	void addAddonDirectionStruct(AddonDirectionStruct arg_direction_struct);
	
	// AddonDirectionsStructを取得
	AddonDirectionStruct getDirectionStruct(DirectionID::Type direction_id);
	
	Array<DirectionID::Type> directionNames;	// typeに含まれる方向
	RGBstruct transparentColor;		// 透過色のRGB値
	
private:
	/* プライベート関数 */
	// 全レイヤに存在するすべてのDirectionIDを取得
	Array<DirectionID::Type> m_get_all_direction_IDs();
	
	// すべての状況に応じたテクスチャを構成
	void m_make_all_textures();
	
	// 保持するm_directionsのDirectionIDを検索
	bool m_is_there(DirectionID::Type direction_id);
	
	// レイヤの重ね合わせ
	void over_write(Image& to, Image& from, Array<LayerType::Type> layer_types, LayerType::Type making_type);
	
	/* プライベート変数 */
	// TypeID
	TypeID::Type m_type_id;
	
	// 方向
	map<DirectionID::Type, AddonDirectionStruct> m_directions;
	
	// レイヤ
	Array<AddonLayer> m_layers;
	
	// 夜間に電気を灯すか？
	bool m_light_on_night;
	
	// アドオン画像のテクスチャ
	map<LayerType::Type, Texture> m_textures;
	
	
};

#endif /* AddonType_hpp */
