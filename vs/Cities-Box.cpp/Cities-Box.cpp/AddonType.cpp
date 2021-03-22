//
//  AddonType.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#include "AddonType.hpp"

AddonType::AddonType() {
}

AddonType::AddonType(TypeID::Type arg_type_ID) {
	m_type_id = arg_type_ID;
}

void AddonType::draw(TimeStruct time, AddonDirectionStruct direction_id, PositionStruct position, CoordinateStruct coordinate) {
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
	return m_textures[timeToLayerType(time)];
}

void AddonType::setLayers(Array<AddonLayer> layers) {
	m_layers = layers;
	m_make_all_textures();
}

int AddonType::countLayers() {
	return (int)m_layers.size();
}

void AddonType::addAddonDirectionStruct(AddonDirectionStruct arg_direction_struct) {
	m_directions[arg_direction_struct.directionID] = arg_direction_struct;
}

AddonDirectionStruct AddonType::getDirectionStruct(DirectionID::Type direction_id) {
	return m_directions[direction_id];
}

void AddonType::m_make_all_textures() {
	for (int i=0; i<LAYER_TYPES; i++) {
		// テクスチャ用Image
		Image updated_image(m_layers[0].getImage().size());
		
		// レイヤを重ね合わせ
		int count = 0;
		for(auto iter = begin(m_layers); iter != end(m_layers); ++iter) {
			auto& layer = *iter;
			
			if (!layer.isEnable(AllLayerTypes[i])) {
				continue;
			}
				
			// 重ね合わせ
			Image layer_image = layer.getImage();
			m_over_write(updated_image, layer_image, layer.getLayerTypes(), AllLayerTypes[i]);
			count ++;
		}
		
		// テクスチャに反映
		cout << AllLayerTypes[i] << " : " << count << endl;
		m_textures[AllLayerTypes[i]] = Texture(updated_image);
	}
}

bool AddonType::m_is_there(DirectionID::Type direction_id) {
	if (m_directions.count(direction_id) == 0) {
		return false;
	}
	
	return true;
}

void AddonType::m_over_write(Image &to, Image &from, Array<LayerType::Type> layer_types, LayerType::Type making_type) {
	for (int y=0; y<from.size().y; y++) {
		for (int x=0; x<from.size().x; x++) {
			if (from.getPixel_Clamp(x, y).r != transparentColor.r ||
				from.getPixel_Clamp(x, y).g != transparentColor.g ||
				from.getPixel_Clamp(x, y).b != transparentColor.b) {
				
				to[y][x] = from[y][x];
			}
		}
	}
	
	if (layer_types.count(LayerType::Normal) > 0 && m_is_evening(making_type)) {		// 夕方用
		blendColorAndImage(to, Color(255, 135, 0, 50));
	}
	else if (layer_types.count(LayerType::Normal) > 0 && m_is_night(making_type)) {		// 夜間用
		blendColorAndImage(to, Color(0, 0, 0, 200));
	}
}

bool AddonType::m_is_evening(LayerType::Type layer_type) {
	if ((int)layer_type / 10 == 1) {
		return true;
	}
	return false;
}

bool AddonType::m_is_night(LayerType::Type layer_type) {
	if ((int)layer_type / 10 == 2) {
		return true;
	}
	return false;
}
