//
//  CBAddon.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "CBAddon.hpp"

CBAddon::CBAddon() {
}

bool CBAddon::m_get_element(String str, String searchElementName, String& ret) {
	string strUTF8 = str.toUTF8();
	
	if (strUTF8.find(searchElementName.toUTF8()) != string::npos && strUTF8.find("=") != string::npos) {
		ret = str.substr(strUTF8.find("\"") + 1, str.length() - (strUTF8.find("\"") + 2));
		return true;
	}
	return false;
}

bool CBAddon::m_get_element(String str, String searchElementName, int& ret) {
	string strUTF8 = str.toUTF8();
	
	if (strUTF8.find(searchElementName.toUTF8()) != string::npos && strUTF8.find("=") != string::npos) {
		ret = stoi(strUTF8.substr(strUTF8.find("\"") + 1, str.length() - (strUTF8.find("\"") + 2)));
		return true;
	}
	return false;
}

bool CBAddon::m_get_types(String str, String searchElementName, Array<String>& ret) {
	String aRet;
	if (m_get_element(str, searchElementName, aRet)) {
		ret = UnitaryTools::split(aRet, U", ");
		return true;
	}
	return false;
}

bool CBAddon::load(FileStruct newFilePath, String loadingAddonsSetName) {
	if (FileSystem::Extension(Unicode::Widen(newFilePath.file_path)) == U"adat") {
		//return m_load_adat(newFilePath, loadingAddonsSetName);
	}
	else if (FileSystem::Extension(Unicode::Widen(newFilePath.file_path)) == U"adj") {
		return m_load_adj(newFilePath, loadingAddonsSetName);
	}
	return false;
}

bool CBAddon::m_load_adj(FileStruct newFilePath, String loading_addons_set_name) {
	bool need_to_convert = false;
	
	m_addon_file_path = newFilePath;
	JSON addonData = JSON::Load(Unicode::Widen(m_addon_file_path.file_path));
	
	m_belong_addons_set_name = UnitaryTools::getStrArrayFromJsonArray(addonData[U"Belong_addon_set_name"]);
	bool belong = false;
	for (int i=0; i<m_belong_addons_set_name.size(); i++) {
		if (m_belong_addons_set_name[i].length() > 0) {
			if (m_belong_addons_set_name[i] == loading_addons_set_name) {
				belong = true;
			}
		}
	}
	if (!belong) {
		return false;
	}
	
	int version = addonData[U"version"].get<int>();
	
	m_addon_name = addonData[U"name"].getString();
	m_addon_jp_name = addonData[U"jp_name"].getString();
	
	m_addon_author = addonData[U"author"].getString();
	m_addon_summary = addonData[U"summary"].getString();
	
	m_addon_icon = addonData[U"icon"].getString();
	
	// アイコンを読み込み
	Image iconImage(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+m_addon_icon);
	UnitaryTools::setAlphaColor(iconImage, Color(0, 0, 0));
	m_icon_texture = Texture(iconImage);
	
	// カテゴリ
	Array<String> addon_categories_strarray = UnitaryTools::getStrArrayFromJsonArray(addonData[U"Categories"]);
	for (auto addon_category_str : addon_categories_strarray) {
		m_addon_categories << UnitaryTools::categoryNameToCategoryID(addon_category_str);
	}
	
	// 建物の効果
	for (const auto& effect : addonData[U"effects"]) {
		RateID::Type rate_id = UnitaryTools::rateNameToRateID(effect.key);
		m_effects[rate_id].influence = effect.value[U"influence"].get<int>();
		m_effects[rate_id].grid = effect.value[U"grid"].get<int>();
	}
	
	if (addonData[U"Types"].getType() == JSONValueType::Array) {
		for (const auto& type : addonData[U"Types"].arrayView()) {					// AddonType
			TypeID::Type typeID = UnitaryTools::typeNameToTypeID(type[U"type_name"].getString());
			
			m_use_types << UnitaryTools::typeIDToTypeName(typeID);
			m_types[typeID] = AddonType(typeID);
			cout << "TypeID: " << type[U"type_name"].getString() << endl;
			
			if (type[U"Directions"].getType() == JSONValueType::Array) {
				for (const auto& direction : type[U"Directions"].arrayView()) {			// AddonDirectionStruct
					DirectionID::Type direction_id = UnitaryTools::directionNameToDirectionID(direction[U"direction_name"].getString());
					
					if (direction_id == DirectionID::None && typeID == TypeID::IntersectionCross) {
						direction_id = DirectionID::All;
					}
					
					if (direction_id == DirectionID::Disabled) {
						cout << "Disabled direction at " << direction[U"direction_name"].getString() << " type: " << type[U"type_name"].getString() << endl;
					}
					if (typeID == TypeID::Disabled) {
						cout << "Disabled type at " << type[U"type_name"].getString() << endl;
					}
					
					// 新たなAddonDirectionStructを作成
					AddonDirectionStruct direction_struct;
					
					direction_struct.directionID = direction_id;
					
					direction_struct.size.x = direction[U"size"][U"width"].get<int>();
					direction_struct.size.y = direction[U"size"][U"height"].get<int>();
					
					direction_struct.requiredTiles.x = direction[U"squares"][U"width"].get<int>();
					direction_struct.requiredTiles.y = direction[U"squares"][U"height"].get<int>();
					
					// requiredTiles = (0, 0)の場合->(1, 1)に修正
					if (direction_struct.requiredTiles.x == 0) {
						direction_struct.requiredTiles.x = 1;
						need_to_convert = true;
					}
					if (direction_struct.requiredTiles.y == 0) {
						direction_struct.requiredTiles.y = 1;
						need_to_convert = true;
					}
					
					direction_struct.topLeft.x = direction[U"top_left"][U"x"].get<int>();
					direction_struct.topLeft.y = direction[U"top_left"][U"y"].get<int>();
					
					direction_struct.bottomRight.x = direction[U"bottom_right"][U"x"].get<int>();
					direction_struct.bottomRight.y = direction[U"bottom_right"][U"y"].get<int>();
					
					if (typeID == TypeID::WaterOffshore) {
						cout << "direction id: " << direction_id << endl;
						cout << "size: " << direction_struct.size.x << "," << direction_struct.size.y << endl;
						cout << "required tiles: " << direction_struct.requiredTiles.x << "," << direction_struct.requiredTiles.y << endl;
					}
					
					// direction_structをlayerに追加
					m_types[typeID].addAddonDirectionStruct(direction_struct);
				}
			}
			
			// r141以前のadjの場合
			if (version <= 141) {
				/*
				int total_layers = 1;
				String night_mask_filename = type[U"night_mask"].getString();
				if (FileSystem::IsFile(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+night_mask_filename)) {
					total_layers = 2;
				}
				
				Array<AddonLayer> layers;
				for (int layer_num=0; layer_num<total_layers; layer_num++) {						// AddonLayer r141以前
					m_load_layer_before141(layer_num, type, m_types[typeID], layers);
				}
				m_types[typeID].setLayers(layers);
				need_to_convert = true;
				 */
			}
			else {
				Array<AddonLayer> layers;
				if (type[U"Layers"].getType() == JSONValueType::Array) {
					for (const auto& layer : type[U"Layers"].arrayView()) {								// AddonLayer r142以降
						String image_path = layer[U"image"].getString();
						
						Color transparent_color;
						transparent_color.r = layer[U"transparent_color"][U"R"].get<int>();
						transparent_color.g = layer[U"transparent_color"][U"G"].get<int>();
						transparent_color.b = layer[U"transparent_color"][U"B"].get<int>();
						
						Array<LayerType::Type> layer_types;
						if (layer[U"layer_types"].getType() == JSONValueType::Array) {
							for (const auto& layer_type_str : layer[U"layer_types"].arrayView()) {
								layer_types << UnitaryTools::layerNameToLayerType(layer_type_str.getString());
							}
						}
						
						layers << AddonLayer(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+image_path, transparent_color, layer_types);
					}
				}
				m_types[typeID].setLayers(layers);
			}
		}
	}
	
	// アドオンデータの修正の必要がある場合は修正
	if (need_to_convert) {
		cout << "update addon file: " << m_addon_file_path.file_path << endl;
		m_converter();
	}
	
	return true;
}

String CBAddon::getName(NameMode::Type mode) {
	if (mode == NameMode::English) {
		return m_addon_name;
	}
	else {
		return m_addon_jp_name;
	}
}

String CBAddon::getAuthorName() {
	return m_addon_author;
}

String CBAddon::getSummary() {
	return m_addon_summary;
}
/*
String CBAddon::getTypeName(int type_num) {
	map<String, AddonTypeStruct>::iterator ite = types.begin();
	advance(ite, type_num);
	return ite->first;
}

String CBAddon::getDirectionName(int type_num, int direction_num) {
	map<String, AddonDirectionStruct>::iterator ite = types[getTypeName(type_num)].directions.begin();
	advance(ite, direction_num);
	return ite->first;
}
*/
TypeID::Type CBAddon::getTypeID(int typeNum) {
	return UnitaryTools::typeNameToTypeID(m_use_types[typeNum]);
}

DirectionID::Type CBAddon::getDirectionID(int typeNum, int directionNum) {
	return m_types[getTypeID(typeNum)].getDirectionID(directionNum);
}

DirectionID::Type CBAddon::getDirectionID(String typeName, int directionNum) {
	return m_types[UnitaryTools::typeNameToTypeID(typeName)].getDirectionID(directionNum);
}

AddonDirectionStruct CBAddon::getDirectionStruct(TypeID::Type arg_type_id, DirectionID::Type arg_direction_id) {
	return m_types[arg_type_id].getDirectionStruct(arg_direction_id);
}

Array<CategoryID::Type> CBAddon::getCategories() {
	return m_addon_categories;
}

bool CBAddon::isInCategories(CategoryID::Type searchCategory) {
	for (int i=0; i<m_addon_categories.size(); i++) {
		if (m_addon_categories[i] == searchCategory) {
			return true;
		}
	}
	return false;
}

bool CBAddon::isInCategories(Array<CategoryID::Type> searchCategories) {
	for (int i=0; i<m_addon_categories.size(); i++) {
		for (int j=0; j<searchCategories.size(); j++) {
			if (m_addon_categories[i] == searchCategories[j]) {
				return true;
			}
		}
	}
	return false;
}

bool CBAddon::isMatch(CBAddon* target_addon, CategoryID::Type hint) {
	if (target_addon == nullptr) {
		return false;
	}
	if (target_addon == this) {
		return true;
	}
	
	if (hint == CategoryID::Connectable) {
		if ((isInCategories(CategoryID::Road) && target_addon->isInCategories(CategoryID::Road)) ||
			(isInCategories(CategoryID::Railroad) && target_addon->isInCategories(CategoryID::Railroad)) ||
			(isInCategories(CategoryID::Station) && target_addon->isInCategories(CategoryID::Station)) ||
			(isInCategories(CategoryID::Waterway) && target_addon->isInCategories(CategoryID::Waterway)) ||
			(isInCategories(CategoryID::Taxiway) && target_addon->isInCategories(CategoryID::Taxiway)) ||
			(isInCategories(CategoryID::Runway) && target_addon->isInCategories(CategoryID::Runway))) {
			
			return true;
		}
	}
	else if (hint == CategoryID::ObjectType) {
		if ((isInCategories(CategoryID::Residential) && target_addon->isInCategories(CategoryID::Residential)) ||
			(isInCategories(CategoryID::Commecial) && target_addon->isInCategories(CategoryID::Commecial)) ||
			(isInCategories(CategoryID::Office) && target_addon->isInCategories(CategoryID::Office)) ||
			(isInCategories(CategoryID::Industrial) && target_addon->isInCategories(CategoryID::Industrial)) ||
			(isInCategories(CategoryID::Farm) && target_addon->isInCategories(CategoryID::Farm)) ||
			(isInCategories(CategoryID::Public) && target_addon->isInCategories(CategoryID::Public)) ||
			(isInCategories(CategoryID::Park) && target_addon->isInCategories(CategoryID::Park)) ||
			(isInCategories(CategoryID::Tile) && target_addon->isInCategories(CategoryID::Tile))) {
			
			return true;
		}
	}
	
	return false;
}

bool CBAddon::canConnect(CBAddon* target_addon) {
	if (target_addon == nullptr) {
		return false;
	}
	if (!isInCategories(CategoryID::Connectable) || !target_addon->isInCategories(CategoryID::Connectable)) {
		return false;
	}
	
	if ((isInCategories(CategoryID::Road) && target_addon->isInCategories(CategoryID::Road)) ||
		(isInCategories(CategoryID::Train) && target_addon->isInCategories(CategoryID::Train)) ||
		(isInCategories(CategoryID::Waterway) && target_addon->isInCategories(CategoryID::Waterway)) ||
		(isInCategories(CategoryID::Airport) && target_addon->isInCategories(CategoryID::Airport))) {
		
		return true;
	}
	
	return false;
}

map<RateID::Type, EffectStruct> CBAddon::getEffects() {
	return m_effects;
}

void CBAddon::drawIcon(PositionStruct position, PositionStruct leftTop, Size size) {
	m_icon_texture(leftTop.x, leftTop.y, size.x, size.y).draw(position.x, position.y);
}

CoordinateStruct CBAddon::getUseTiles(TypeID::Type typeID, DirectionID::Type directionID) {
	return CoordinateStruct{m_types[typeID].getDirectionStruct(directionID).requiredTiles.x, m_types[typeID].getDirectionStruct(directionID).requiredTiles.y};
}

void CBAddon::draw(TypeID::Type typeID, DirectionID::Type directionID, PositionStruct position, RelativeCoordinateStruct tilesCount, Color addColor, TimeStruct time) {
	m_types[typeID].draw(time, directionID, position, tilesCount, addColor);
}
/*
void CBAddon::m_load_layer_before141(int layer_num, JSONValue type, AddonType& arg_addon_type, Array<AddonLayer>& arg_layers) {
	String image_filename;
	if (layer_num == 0)
		image_filename = type[U"image"].getString();
	else
		image_filename = type[U"night_mask"].getString();
	
	Color transparent_color;
	transparent_color.r = type[U"transparent_color.R"].get<int>();
	transparent_color.g = type[U"transparent_color.G"].get<int>();
	transparent_color.b = type[U"transparent_color.B"].get<int>();
	
	// AddonLayerを作成（暫定）
	// Todo: 正式に対応する
	Array<LayerType::Type> layer_types;
	if (layer_num == 0)
		layer_types << LayerType::Normal;
	if (layer_num == 1)
		layer_types << LayerType::Night;
	AddonLayer layer(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+image_filename,
					 transparent_color, layer_types);
	
	/*
	String night_mask_filename = type[U"night_mask"].getString();
	if (FileSystem::IsFile(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+night_mask_filename)) {
		Image iTempNM(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+night_mask_filename);
		m_set_alpha_color(iTempNM, Color(m_types[typeID].transparentColor.r, m_types[typeID].transparentColor.g, m_types[typeID].transparentColor.b));
		m_types[typeID].nightMaskTexture = Texture(iTempNM);
	}
	
	
	// layerをlayersに追加
	arg_layers << layer;
}
*/
void CBAddon::m_converter() {
	JSON addonData;
	
	addonData[U"name"] = m_addon_name;
	addonData[U"jp_name"] = m_addon_jp_name;
	
	addonData[U"author"] = m_addon_author;
	addonData[U"summary"] = m_addon_summary;
	
	addonData[U"version"] = RELEASE_NUMBER;
	
	Array<JSON> json_belong_addon_set_names_array;
	for (auto belong = m_belong_addons_set_name.begin(); belong!= m_belong_addons_set_name.end() ; belong++) {
		JSON json_belong_addon_set_name = *belong;
		json_belong_addon_set_names_array << json_belong_addon_set_name;
	}
	addonData[U"Belong_addon_set_name"] = json_belong_addon_set_names_array;
	
	addonData[U"icon"] = m_addon_icon;
	
	Array<JSON> json_categories_array;
	for (auto categoryName = m_addon_categories.begin(); categoryName != m_addon_categories.end(); categoryName++) {
		JSON json_categories = *categoryName;
		json_categories_array << json_categories;
	}
	addonData[U"Categories"] = json_categories_array;
	
	for (auto e = m_effects.begin(); e != m_effects.end() ; e++) {
		if (e->second.influence != 0) {
			JSON json_effect;
			json_effect[U"influence"] = e->second.influence;
			json_effect[U"grid"] = e->second.grid;
			
			addonData[U"effects"][UnitaryTools::rateIDToRateName(e->first)] = json_effect;
		}
	}
	
	addonData[U"maximum_capacity"] = m_maximum_capacity;
	
	Array<JSON> json_types_array;
	for (auto type = m_types.begin(); type != m_types.end(); type++) {
		JSON json_type;
		
		// タイプの名前
		json_type[U"type_name"] = UnitaryTools::typeIDToTypeName(type->first);
		
		// 方向の名前
		map<DirectionID::Type, AddonDirectionStruct> direction_structs = type->second.getDirectionStructs();
		Array<String> direction_names_array;
		for (auto direction = direction_structs.begin(); direction != direction_structs.end(); direction++) {
			direction_names_array << UnitaryTools::directionIDToDirectionName(direction->first);
		}
		json_type[U"direction_names"] = direction_names_array;
		
		// 各方向のデータ
		Array<JSON> json_directions_array;
		for (auto direction = direction_structs.begin(); direction != direction_structs.end(); direction++) {
			JSON json_direction;
			
			json_direction[U"direction_name"] = UnitaryTools::directionIDToDirectionName(direction->first);
			
			JSON json_direction_size;
			json_direction_size[U"width"] = direction->second.size.x;
			json_direction_size[U"height"] = direction->second.size.y;
			json_direction[U"size"] = json_direction_size;
			
			JSON json_direction_squares;
			json_direction_squares[U"width"] = direction->second.requiredTiles.x;
			json_direction_squares[U"height"] = direction->second.requiredTiles.y;
			json_direction[U"squares"] = json_direction_squares;
			
			JSON json_direction_top_left;
			json_direction_top_left[U"x"] = direction->second.topLeft.x;
			json_direction_top_left[U"y"] = direction->second.topLeft.y;
			json_direction[U"top_left"] = json_direction_top_left;
			
			JSON json_direction_bottom_right;
			json_direction_bottom_right[U"x"] = direction->second.bottomRight.x;
			json_direction_bottom_right[U"y"] = direction->second.bottomRight.y;
			json_direction[U"bottom_right"] = json_direction_top_left;
			
			json_directions_array << json_direction;
		}
		json_type[U"Directions"] = json_directions_array;
		
		// 各レイヤのデータ
		Array<JSON> json_layers_array;
		Array<AddonLayer>& layers = type->second.getLayers();
		for (auto layer = layers.begin(); layer != layers.end(); layer++) {
			JSON json_layer;
			
			// 画像のパス
			json_layer[U"image"] = FileSystem::FileName(layer->getImagePath());
			
			// 透過色
			Color transparent_color = layer->getTransparentColor();
			JSON json_layer_transparent_color;
			json_layer_transparent_color[U"R"] = transparent_color.r;
			json_layer_transparent_color[U"G"] = transparent_color.g;
			json_layer_transparent_color[U"B"] = transparent_color.b;
			json_layer[U"transparent_color"] = json_layer_transparent_color;
			
			// レイヤのタイプ
			Array<JSON> json_layer_types;
			Array<LayerType::Type> layer_types = layer->getLayerTypesInit();
			for (int layer_type_num = 0; layer_type_num < layer_types.size(); layer_type_num++) {
				JSON json_layer_type;
				json_layer_type = UnitaryTools::layerTypeToLayerName(layer_types[layer_type_num]);
				json_layer_types << json_layer_type;
			}
			json_layer[U"layer_types"] = json_layer_types;
			
			json_layers_array << json_layer;
		}
		json_type[U"Layers"] = json_layers_array;
		
		json_types_array << json_type;
	}
	addonData[U"Types"] = json_types_array;
	
	cout << FileSystem::ParentPath(Unicode::Widen(m_addon_file_path.file_path))+FileSystem::BaseName(Unicode::Widen(m_addon_file_path.file_path))+U".adj" << endl;
	addonData.save(FileSystem::ParentPath(Unicode::Widen(m_addon_file_path.file_path))+FileSystem::BaseName(Unicode::Widen(m_addon_file_path.file_path))+U".adj");
}
