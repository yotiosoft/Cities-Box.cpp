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

bool AddonLayer::isEnable(LayerType::Type layer_type) {
	if (layer_type == LayerType::Normal || layer_type == LayerType::Ground || m_layer_types.count(layer_type) > 0) {
		return true;
	}
	
	return false;
}
