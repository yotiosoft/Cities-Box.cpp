//
//  AddonLayer.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/15.
//

#include "AddonLayer.hpp"

AddonLayer::AddonLayer(Image arg_image, Array<LayerType::Type> arg_layer_types) {
	image = arg_image;
	layer_types = arg_layer_types;
}

Image AddonLayer::getImage(DirectionID::Type direction_id) {
	return image;
}

ThreeTypesAnswer::Type AddonLayer::isEnable(TimeStruct time) {
	// 地面か条件なしなら無条件にYes
	for (int i=0; i<layer_types.size(); i++) {
		if (layer_types[i] == LayerType::Ground || layer_types[i] == LayerType::Normal) {
			return ThreeTypesAnswer::Yes;
		}
	}
	
	// 夜間
	if ((time.hour >= 18 && time.hour <= 23) || (time.hour >= 0 && time.hour <= 5)) {
		for (int i=0; i<layer_types.size(); i++) {
			if (layer_types[i] == LayerType::Night) {
				return ThreeTypesAnswer::Yes;
			}
			
			if (layer_types[i] == LayerType::OnSpringNight) {
				if (time.month >= 3 && time.month <= 5) {
					return ThreeTypesAnswer::Yes;
				}
			}
			
			if (layer_types[i] == LayerType::OnSummerNight) {
				if (time.month >= 6 && time.month <= 8) {
					return ThreeTypesAnswer::Yes;
				}
			}
			
			if (layer_types[i] == LayerType::OnAutumnNight) {
				if (time.month >= 9 && time.month <= 11) {
					return ThreeTypesAnswer::Yes;
				}
			}
			
			if (layer_types[i] == LayerType::OnWinterNight) {
				if (time.month == 12 || time.month == 1 || time.month == 2) {
					return ThreeTypesAnswer::Yes;
				}
			}
		}
	}
	
	// 季節によるもの
	for (int i=0; i<layer_types.size(); i++) {
		if (layer_types[i] == LayerType::OnSpring) {
			if (time.month >= 3 && time.month <= 5) {
				return ThreeTypesAnswer::Yes;
			}
		}
		
		if (layer_types[i] == LayerType::OnSummer) {
			if (time.month >= 6 && time.month <= 8) {
				return ThreeTypesAnswer::Yes;
			}
		}
		
		if (layer_types[i] == LayerType::OnAutumn) {
			if (time.month >= 9 && time.month <= 11) {
				return ThreeTypesAnswer::Yes;
			}
		}
		
		if (layer_types[i] == LayerType::OnWinter) {
			if (time.month == 12 || time.month == 1 || time.month == 2) {
				return ThreeTypesAnswer::Yes;
			}
		}
	}
	
	// 夕方・朝方
	if ((time.hour >= 16 && time.hour <= 17) || time.hour == 6) {
		for (int i=0; i<layer_types.size(); i++) {
			// 乱数によるのでNeutralに
			if (layer_types[i] == LayerType::Evening) {
				return ThreeTypesAnswer::Neutral;
			}
		}
	}
	
	return ThreeTypesAnswer::No;
}
