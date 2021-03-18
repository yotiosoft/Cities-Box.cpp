//
//  AddonLayer.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/15.
//

#ifndef AddonLayer_hpp
#define AddonLayer_hpp

#include "Specific.hpp"
#include "AddonDirectionStruct.hpp"

class AddonLayer {
public:
	// コンストラクタ
	AddonLayer(Image arg_image, Array<LayerType::Type> arg_layer_type);
	
	// imageを返す
	Image getImage();
	// 合致するdirectionの部分を切り取ってimageを返す
	Image getImage(DirectionID::Type direction_id);
	
	// 保持するm_directionsのDirectionIDを検索
	bool isThere(DirectionID::Type direction_id);
	
	// 指定した時刻がレイヤ出現の条件に合うか？
	ThreeTypesAnswer::Type isEnable(TimeStruct time);
	
private:
	// レイヤ画像
	Image m_image;
	
	// レイヤのタイプ
	Array<LayerType::Type> m_layer_types;
	
	// 方向
	map<DirectionID::Type, AddonDirectionStruct> m_directions;
};

#endif /* AddonLayer_hpp */
