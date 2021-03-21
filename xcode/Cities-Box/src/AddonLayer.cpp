//
//  AddonLayer.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/15.
//

#include "AddonLayer.hpp"

AddonLayer::AddonLayer(Image arg_image, Array<LayerType::Type> arg_layer_types) {
	m_image = arg_image;
	m_layer_types = arg_layer_types;
}

Image AddonLayer::getImage() {
	return m_image;
}

Image AddonLayer::getImage(DirectionID::Type direction_id) {
	// 指定したdirection_idが存在しない場合は空のImageを返す
	if (m_directions.count(direction_id) == 0) {
		return Image();
	}
	
	AddonDirectionStruct direction = m_directions[direction_id];
	return m_image.clipped(direction.topLeft.x, direction.topLeft.y, direction.size.x, direction.size.y);
}

bool AddonLayer::isThere(DirectionID::Type direction_id) {
	if (m_directions.count(direction_id) == 0) {
		return false;
	}
	
	return true;
}

ThreeTypesAnswer::Type AddonLayer::isEnable(TimeStruct time) {
	// 地面か条件なしなら無条件にYes
	for (int i=0; i<m_layer_types.size(); i++) {
		if (m_layer_types[i] == LayerType::Ground || m_layer_types[i] == LayerType::Normal) {
			return ThreeTypesAnswer::Yes;
		}
	}
	
	// 夜間
	if ((time.hour >= 18 && time.hour <= 23) || (time.hour >= 0 && time.hour <= 5)) {
		for (int i=0; i<m_layer_types.size(); i++) {
			if (m_layer_types[i] == LayerType::Night) {
				return ThreeTypesAnswer::Yes;
			}
			
			if (m_layer_types[i] == LayerType::OnSpringNight) {
				if (time.month >= 3 && time.month <= 5) {
					return ThreeTypesAnswer::Yes;
				}
			}
			
			if (m_layer_types[i] == LayerType::OnSummerNight) {
				if (time.month >= 6 && time.month <= 8) {
					return ThreeTypesAnswer::Yes;
				}
			}
			
			if (m_layer_types[i] == LayerType::OnAutumnNight) {
				if (time.month >= 9 && time.month <= 11) {
					return ThreeTypesAnswer::Yes;
				}
			}
			
			if (m_layer_types[i] == LayerType::OnWinterNight) {
				if (time.month == 12 || time.month == 1 || time.month == 2) {
					return ThreeTypesAnswer::Yes;
				}
			}
		}
	}
	
	// 季節によるもの
	for (int i=0; i<m_layer_types.size(); i++) {
		if (m_layer_types[i] == LayerType::OnSpring) {
			if (time.month >= 3 && time.month <= 5) {
				return ThreeTypesAnswer::Yes;
			}
		}
		
		if (m_layer_types[i] == LayerType::OnSummer) {
			if (time.month >= 6 && time.month <= 8) {
				return ThreeTypesAnswer::Yes;
			}
		}
		
		if (m_layer_types[i] == LayerType::OnAutumn) {
			if (time.month >= 9 && time.month <= 11) {
				return ThreeTypesAnswer::Yes;
			}
		}
		
		if (m_layer_types[i] == LayerType::OnWinter) {
			if (time.month == 12 || time.month == 1 || time.month == 2) {
				return ThreeTypesAnswer::Yes;
			}
		}
	}
	
	// 夕方・朝方
	if ((time.hour >= 16 && time.hour <= 17) || time.hour == 6) {
		for (int i=0; i<m_layer_types.size(); i++) {
			// 乱数によるのでNeutralに
			if (m_layer_types[i] == LayerType::Evening) {
				return ThreeTypesAnswer::Neutral;
			}
		}
	}
	
	return ThreeTypesAnswer::No;
}

void AddonLayer::addAddonDirectionStruct(AddonDirectionStruct arg_direction_struct) {
	m_directions[arg_direction_struct.directionID] = arg_direction_struct;
}
