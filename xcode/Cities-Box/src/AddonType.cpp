//
//  AddonType.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#include "AddonType.hpp"


AddonType::AddonType() {
}

AddonType::AddonType(TypeID::Type arg_type_ID, Array<AddonLayer> arg_layers, bool arg_light_on_night) {
	
	
}

void AddonType::m_update_layers(TimeStruct time) {
	// 各レイヤの有効状況を確認
	Array<bool> enabled_layers_list = m_get_enable_layers_list(time);
	
	// テクスチャ用Image
	Image updated_image;
	
	// レイヤを重ね合わせ
	for (int i=0; i<m_layers.size(); i++) {
		if (!enabled_layers_list[i]) {
			continue;
		}
		
		// 重ね合わせ
		m_layers[i].getImage().overwrite(updated_image, 0, 0);
	}
	
	m_texture = Texture(updated_image);
}

Array<bool> AddonType::m_get_enable_layers_list(TimeStruct time) {
	Array<bool> ret;
	
	for (int i=0; i<m_layers.size(); i++) {
		switch(m_layers[i].isEnable(time)) {
			case ThreeTypesAnswer::Yes:
				ret << true;
				break;
			case ThreeTypesAnswer::No:
				ret << false;
				break;
			case ThreeTypesAnswer::Neutral:
				if (m_light_on_night) {
					ret << true;
				}
				else {
					ret << false;
				}
				break;
		}
	}
	
	return ret;
}
