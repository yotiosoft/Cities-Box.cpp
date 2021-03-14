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
	
	// 指定した時刻がレイヤ出現の条件に合うか？
	ThreeTypesAnswer::Type isEnable(TimeStruct time);
	
private:
	// レイヤ画像
	Image image;
	
	// レイヤのタイプ
	Array<LayerType::Type> layer_types;
	
	// 方向
	Array<AddonDirectionStruct> directions;
};

#endif /* AddonLayer_hpp */
