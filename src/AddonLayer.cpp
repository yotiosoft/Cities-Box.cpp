//
//  AddonLayer.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/15.
//

#include "AddonLayer.hpp"

AddonLayer::AddonLayer(String arg_image_path, Color transparent_color, Array<LayerType::Type> arg_layer_types) {
	m_image = Image(arg_image_path);
	m_image_path = arg_image_path;
	
	// 透過色を透過させる
	m_transparent_color = transparent_color;
	setAlphaColor(m_image, Color(transparent_color.r, transparent_color.g, transparent_color.b));
	
	m_layer_types_init = arg_layer_types;
	m_layer_types = m_layer_types_init;
	
	if (arg_layer_types.count(LayerType::Normal) > 0 || arg_layer_types.count(LayerType::Ground) > 0) {
		for (int i=0; i<LAYER_TYPES; i++) {
			m_layer_types << AllLayerTypes[i];
		}
	}
	if (arg_layer_types.count(LayerType::Night) > 0) {
		if (arg_layer_types.count(LayerType::OnSpringNight) == 0) {
			m_layer_types << LayerType::OnSpringNight;
		}
		if (arg_layer_types.count(LayerType::OnSummerNight) == 0) {
			m_layer_types << LayerType::OnSummerNight;
		}
		if (arg_layer_types.count(LayerType::OnAutumnNight) == 0) {
			m_layer_types << LayerType::OnAutumnNight;
		}
		if (arg_layer_types.count(LayerType::OnWinterNight) == 0) {
			m_layer_types << LayerType::OnWinterNight;
		}
	}
	if (arg_layer_types.count(LayerType::Evening) > 0) {
		if (arg_layer_types.count(LayerType::OnSpringEvening) == 0) {
			m_layer_types << LayerType::OnSpringEvening;
		}
		if (arg_layer_types.count(LayerType::OnSummerEvening) == 0) {
			m_layer_types << LayerType::OnSummerEvening;
		}
		if (arg_layer_types.count(LayerType::OnAutumnEvening) == 0) {
			m_layer_types << LayerType::OnAutumnEvening;
		}
		if (arg_layer_types.count(LayerType::OnWinterEvening) == 0) {
			m_layer_types << LayerType::OnWinterEvening;
		}
	}
}

Image AddonLayer::getImage() {
	return m_image;
}

Array<LayerType::Type> AddonLayer::getLayerTypes() {
	return m_layer_types;
}

Array<LayerType::Type> AddonLayer::getLayerTypesInit() {
	return m_layer_types_init;
}

Color AddonLayer::getTransparentColor() {
	return m_transparent_color;
}

bool AddonLayer::isEnable(LayerType::Type layer_type) {
	if (m_layer_types.count(layer_type) > 0) {
		return true;
	}
	
	return false;
}

bool AddonLayer::isExists(LayerType::Type layer_type) {
	if (m_layer_types_init.count(layer_type) > 0) {
		return true;
	}
	
	return false;
}

String AddonLayer::getImagePath() {
	return m_image_path;
}
