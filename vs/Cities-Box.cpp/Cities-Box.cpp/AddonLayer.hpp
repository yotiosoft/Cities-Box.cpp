//
//  AddonLayer.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/15.
//

#ifndef AddonLayer_hpp
#define AddonLayer_hpp

#pragma once

#include "Specific.hpp"
#include "AddonDirectionStruct.hpp"

class AddonLayer {
public:
	// コンストラクタ
	AddonLayer(Image arg_image, Array<LayerType::Type> arg_layer_type);
	
	// imageを返す
	Image getImage();
	
	// m_layer_typesを返す
	Array<LayerType::Type> getLayerTypes();
	
	// 指定したLayerType::Typeがレイヤ出現の条件に合うか？
	bool isEnable(LayerType::Type layer_type);
	
private:
	// レイヤ画像
	Image m_image;
	
	// レイヤのタイプ
	Array<LayerType::Type> m_layer_types;
};

#endif /* AddonLayer_hpp */
