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

void AddonType::draw(TimeStruct time, DirectionID::Type direction_id, PositionStruct position, RelativeCoordinateStruct coordinate, Color add_color) {
	position = getPosition(direction_id, position, m_directions[direction_id].requiredTiles, coordinate);
	
	unsigned short int topLeftX = m_directions[direction_id].topLeft.x;
	topLeftX += CHIP_SIZE/2 * coordinate.relative.x + CHIP_SIZE/2 * (m_directions[direction_id].requiredTiles.y - 1 - coordinate.relative.y);
	
	unsigned short int topLeftY = m_directions[direction_id].topLeft.y;
	//topLeftY += CHIP_SIZE/2 * coordinate.relative.y;
	
	unsigned short int sizeWidth = m_directions[direction_id].size.x;
	sizeWidth = CHIP_SIZE;
	
	unsigned short int sizeHeight = m_directions[direction_id].size.y;
	
	//position.x -= CHIP_SIZE/2 * coordinate.relative.x;
	//position.y -= CHIP_SIZE/4 * coordinate.relative.y;
	
	// オブジェクトの描画
	if (add_color.a > 0) {
		getTexture(time)(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y, add_color);
	}
	else {
		getTexture(time)(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y);
	}
}

PositionStruct AddonType::getPosition(DirectionID::Type directionID, PositionStruct position, Size useTiles, RelativeCoordinateStruct tilesCount) {
	position.y = position.y + CHIP_SIZE/2 - m_directions[directionID].size.y + CHIP_SIZE/4 * (max(1, useTiles.x) - 1 - tilesCount.relative.x);
	if (useTiles.y > 1) {
		position.y += CHIP_SIZE*1/4 * (1 - tilesCount.relative.y);
	}
	
	return position;
}

Texture AddonType::getTexture(TimeStruct time) {
	if (m_before_layer_type != UnitaryTools::timeToLayerType(time)) {
		m_before_layer_type = UnitaryTools::timeToLayerType(time);
		
		LayerType::Type layer_type = UnitaryTools::timeToLayerType(time);
		if (m_layer_exists_list[layer_type]) {
			m_texture = Texture(m_images[layer_type]);
		}
		else {
			m_texture = Texture(m_images[m_get_default_layer(layer_type)]);
		}
	}
	return m_texture;
}

void AddonType::setLayers(Array<AddonLayer> layers) {
	m_layers = layers;
	
	// 各LayerType独自のレイヤの有無を確認
	for (int i=0; i<LAYER_TYPES; i++) {
		// デフォルトレイヤ（Normal, Evening, Night）は必ず存在するものとする
		if (AllLayerTypes[i] % 10 == 0) {
			m_layer_exists_list[AllLayerTypes[i]] = true;
			continue;
		}
		
		m_layer_exists_list[AllLayerTypes[i]] = false;
		for(auto iter = begin(m_layers); iter != end(m_layers); ++iter) {
			auto& layer = *iter;
			
			if (layer.isExists(AllLayerTypes[i])) {
				m_layer_exists_list[AllLayerTypes[i]] = true;
				break;
			}
		}
	}
	
	// m_imagesの作成
	m_make_all_textures();
}

Array<AddonLayer>& AddonType::getLayers() {
	return m_layers;
}

int AddonType::countLayers() {
	return (int)m_layers.size();
}

void AddonType::addAddonDirectionStruct(AddonDirectionStruct arg_direction_struct) {
	m_directions[arg_direction_struct.directionID] = arg_direction_struct;
	m_addon_direction_ids << arg_direction_struct.directionID;
}

AddonDirectionStruct AddonType::getDirectionStruct(DirectionID::Type direction_id) {
	return m_directions[direction_id];
}

map<DirectionID::Type, AddonDirectionStruct> AddonType::getDirectionStructs() {
	return m_directions;
}

DirectionID::Type AddonType::getDirectionID(int num) {
	return m_addon_direction_ids[num];
}

Array<DirectionID::Type> AddonType::getDirectionIDs() {
	return m_addon_direction_ids;
}

void AddonType::m_make_all_textures() {
	for (int i=0; i<LAYER_TYPES; i++) {
		// 独自レイヤが存在しない場合は作成を省く（デフォルトレイヤで対応する）
		if (!m_layer_exists_list[AllLayerTypes[i]]) {
			cout << "continue at " << AllLayerTypes[i] << endl;
			continue;
		}
		
		// テクスチャ用Image
		Image updated_image(m_layers[0].getImage().size());
		
		// レイヤを重ね合わせ
		for(auto iter = begin(m_layers); iter != end(m_layers); ++iter) {
			auto& layer = *iter;
			
			if (!layer.isEnable(AllLayerTypes[i])) {
				continue;
			}
				
			// 重ね合わせ
			Image layer_image = layer.getImage();
			m_over_write(updated_image, layer_image, layer, AllLayerTypes[i]);
		}
		
		// テクスチャに反映
		//cout << AllLayerTypes[i] << " : " << count << endl;
		m_images[AllLayerTypes[i]] = updated_image;
	}
}

bool AddonType::m_is_there(DirectionID::Type direction_id) {
	if (m_directions.count(direction_id) == 0) {
		return false;
	}
	
	return true;
}

void AddonType::m_over_write(Image &to, Image &from, AddonLayer layer, LayerType::Type making_type) {
	for (int y=0; y<from.size().y; y++) {
		for (int x=0; x<from.size().x; x++) {
			if (from[y][x].r != layer.getTransparentColor().r ||
				from[y][x].g != layer.getTransparentColor().g ||
				from[y][x].b != layer.getTransparentColor().b) {
				to[y][x] = from[y][x];
			}
		}
	}
	
	if (layer.getLayerTypes().count(LayerType::Normal) > 0 && m_is_evening(making_type)) {		// 夕方用
		UnitaryTools::blendColorAndImage(to, Color(255, 135, 0, 50));
	}
	else if (layer.getLayerTypes().count(LayerType::Normal) > 0 && m_is_night(making_type)) {		// 夜間用
		UnitaryTools::blendColorAndImage(to, Color(0, 0, 0, 200));
	}
}

bool AddonType::m_is_evening(LayerType::Type layer_type) {
	if (m_get_default_layer(layer_type) == LayerType::Evening) {
		return true;
	}
	return false;
}

bool AddonType::m_is_night(LayerType::Type layer_type) {
	if (m_get_default_layer(layer_type) == LayerType::Night) {
		return true;
	}
	return false;
}

LayerType::Type AddonType::m_get_default_layer(LayerType::Type layer_type) {
	return (LayerType::Type)(layer_type / 10 * 10);
}
