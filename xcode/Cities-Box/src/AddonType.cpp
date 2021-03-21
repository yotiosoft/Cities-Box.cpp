//
//  AddonType.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#include "AddonType.hpp"


AddonType::AddonType() {
}

AddonType::AddonType(TypeID::Type arg_type_ID, Array<AddonLayer>& arg_layers, bool arg_light_on_night) {
	m_type_id = arg_type_ID;
	m_layers = arg_layers;
	m_light_on_night = arg_light_on_night;
	
	m_enable_direction_id_list = m_get_all_direction_IDs();
}

void AddonType::draw(TimeStruct time, AddonDirectionStruct direction_id, PositionStruct position, CoordinateStruct coordinate) {
	// レイヤの更新処理
	if (time.hour != m_before_time.hour) {
		m_update_layers(time);
	}
	m_before_time = time;
	
	// 画像上の描画始点とサイズを取得
	/*
	unsigned short int topLeftX = directionTemp->topLeft.x;
	topLeftX += CHIP_SIZE/2 * tilesCount.x + CHIP_SIZE/2 * tilesCount.y;
	
	unsigned short int topLeftY = directionTemp->topLeft.y;
	topLeftY += CHIP_SIZE/2 * tilesCount.y;
	
	unsigned short int sizeWidth = directionTemp->size.x;
	sizeWidth = CHIP_SIZE;
	
	unsigned short int sizeHeight = directionTemp->size.y;
*/
}

Texture AddonType::tempGetTexture(TimeStruct time) {
	// レイヤの更新処理
	if (time.hour != m_before_time.hour) {
		m_update_layers(time);
	}
	m_before_time = time;
	
	return m_texture;
}

int AddonType::countLayers() {
	return (int)m_layers.size();
}

void AddonType::addAddonLayer(AddonLayer arg_layer) {
	m_layers << arg_layer;
}

void AddonType::addAddonDirectionStruct(AddonDirectionStruct arg_direction_struct) {
	m_directions[arg_direction_struct.directionID] = arg_direction_struct;
}

AddonDirectionStruct AddonType::getDirectionStruct(DirectionID::Type direction_id) {
	return m_directions[direction_id];
}

Array<DirectionID::Type> AddonType::m_get_all_direction_IDs() {
	Array<DirectionID::Type> ret;
	
	// すべてのDirectionIDについて、m_directionに存在するか確認
	// Todo: これじゃダメでは
	for (int d=0; d<DIRECTIONS; d++) {
		if (m_is_there(DirectionID::Type(d))) {
			ret << DirectionID::Type(d);
		}
	}
	
	return ret;
}

void AddonType::m_make_all_textures() {
	
	
	// テクスチャ用Image
	Image updated_image(m_layers[0].getImage().size());
	
	// レイヤを重ね合わせ
	int i = 0;
	for(auto iter = begin(m_layers); iter != end(m_layers); ++iter) {
		auto& layer = *iter;
		/*
		if (!enabled_layers_list[i]) {
			i++;
			continue;
		}*/
			
		// 重ね合わせ
		layer.getImage().overwrite(updated_image, 0, 0);
			
		i++;
	}
	
	// テクスチャに反映
	m_texture = Texture(updated_image);
}

void AddonType::m_update_layers(TimeStruct time) {
	// 各レイヤの有効状況を確認
	Array<bool> enabled_layers_list = m_get_enable_layers_list(time);
	
	// テクスチャ用Image
	Image updated_image(m_layers[0].getImage().size());
	
	// レイヤを重ね合わせ
	int i = 0;
	for(auto iter = begin(m_layers); iter != end(m_layers); ++iter) {
		auto& layer = *iter;
		/*
		if (!enabled_layers_list[i]) {
			i++;
			continue;
		}*/
			
		// 重ね合わせ
		layer.getImage().overwrite(updated_image, 0, 0);
			
		i++;
	}
	
	// テクスチャに反映
	m_texture = Texture(updated_image);
}

Array<bool> AddonType::m_get_enable_layers_list(TimeStruct time) {
	Array<bool> ret;
	
	for(auto iter = begin(m_layers); iter != end(m_layers); ++iter) {
		auto& layer = *iter;
		
		switch(layer.isEnable(time)) {
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

bool AddonType::m_is_there(DirectionID::Type direction_id) {
	if (m_directions.count(direction_id) == 0) {
		return false;
	}
	
	return true;
}
