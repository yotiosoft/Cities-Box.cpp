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
	
	// 指定したLayerType::Typeがレイヤ出現の条件に合うか？
	bool isEnable(LayerType::Type);
	
private:
	// レイヤ画像
	Image m_image;
	
	// レイヤのタイプ
	Array<LayerType::Type> m_layer_types;
};

#endif /* AddonLayer_hpp */
