//
//  AddonLayer.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/15.
//

#ifndef AddonLayer_hpp
#define AddonLayer_hpp

#pragma once

#include <Specific.hpp>
#include "AddonDirectionStruct.hpp"

class AddonLayer {
public:
	// コンストラクタ
	AddonLayer(String arg_image_path, Color transparent_color, Array<LayerType::Type> arg_layer_type);
	
	// imageを返す
	Image getImage();
	
	// m_layer_typesを返す
	Array<LayerType::Type> getLayerTypes();
	
	// 指定したLayerType::Typeがレイヤ出現の条件に合うか？
	bool isEnable(LayerType::Type layer_type);
	
	// 指定したLayerType::Typeそのものがm_layer_types_initに設定されているか？
	bool isExists(LayerType::Type layer_type);
	
	// 画像パスを返す
	String getImagePath();
	
private:
	// レイヤ画像
	Image m_image;
	
	// レイヤ画像のファイルパス
	String m_image_path;
	
	// レイヤのタイプ
	Array<LayerType::Type> m_layer_types;
	Array<LayerType::Type> m_layer_types_init;	// アドオン読み込み時に指定したもののみ
};

#endif /* AddonLayer_hpp */
