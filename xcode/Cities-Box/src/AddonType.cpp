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

int AddonType::countLayers() {
	return (int)m_layers.size();
}

Array<DirectionID::Type> AddonType::m_get_all_direction_IDs() {
	Array<DirectionID::Type> ret;
	
	// すべてのDirectionIDについて、いずれかのレイヤに存在するか確認
	for (int d=0; d<DIRECTIONS; d++) {
		for(auto iter = begin(m_layers); iter != end(m_layers); ++iter) {
			auto& layer = *iter;
			
			if (layer.isThere(DirectionID::Type(d))) {
				ret << DirectionID::Type(d);
			}
		}
	}
	
	return ret;
}

void AddonType::m_update_layers(TimeStruct time) {
	// 各レイヤの有効状況を確認
	Array<bool> enabled_layers_list = m_get_enable_layers_list(time);
	
	// テクスチャ用Image
	map<DirectionID::Type, Image> updated_images;
	
	// レイヤを重ね合わせ
	for (int i=0; i<m_enable_direction_id_list.size(); i++) {
		// そのDirectionIDが存在するレイヤから1区分を切り出し
		int j=0;
		for(auto iter = begin(m_layers); iter != end(m_layers); ++iter) {
			auto& layer = *iter;
			
			if (!enabled_layers_list[j]) {
				j++;
				continue;
			}
			
			// 重ね合わせ
			layer.getImage(m_enable_direction_id_list[i]).overwrite(updated_images[m_enable_direction_id_list[i]], 0, 0);
			
			j++;
		}
	}
	
	// テクスチャに反映
	for(auto iter = begin(updated_images); iter != end(updated_images); ++iter) {
		auto& im = *iter;
		m_textures[im.first] = Texture(im.second);
	}
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


