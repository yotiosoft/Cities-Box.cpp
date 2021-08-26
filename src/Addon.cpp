//
//  Addon.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "Addon.hpp"

Addon::Addon() {
}

bool Addon::m_get_element(String str, String searchElementName, String& ret) {
	string strUTF8 = str.toUTF8();
	
	if (strUTF8.find(searchElementName.toUTF8()) != string::npos && strUTF8.find("=") != string::npos) {
		ret = str.substr(strUTF8.find("\"") + 1, str.length() - (strUTF8.find("\"") + 2));
		return true;
	}
	return false;
}

bool Addon::m_get_element(String str, String searchElementName, int& ret) {
	string strUTF8 = str.toUTF8();
	
	if (strUTF8.find(searchElementName.toUTF8()) != string::npos && strUTF8.find("=") != string::npos) {
		ret = stoi(strUTF8.substr(strUTF8.find("\"") + 1, str.length() - (strUTF8.find("\"") + 2)));
		return true;
	}
	return false;
}

bool Addon::m_get_types(String str, String searchElementName, Array<String>& ret) {
	String aRet;
	if (m_get_element(str, searchElementName, aRet)) {
		ret = UnitaryTools::split(aRet, U", ");
		return true;
	}
	return false;
}

bool Addon::load(FileStruct newFilePath, String loadingAddonsSetName) {
	if (FileSystem::Extension(Unicode::Widen(newFilePath.file_path)) == U"adat") {
		//return m_load_adat(newFilePath, loadingAddonsSetName);
	}
	else if (FileSystem::Extension(Unicode::Widen(newFilePath.file_path)) == U"adj") {
		return m_load_adj(newFilePath, loadingAddonsSetName);
	}
	return false;
}

bool Addon::m_load_adj(FileStruct newFilePath, String loading_addons_set_name) {
	bool need_to_convert = false;
	
	m_addon_file_path = newFilePath;
	JSONReader addonData(Unicode::Widen(m_addon_file_path.file_path));
	
	m_belong_addons_set_name = addonData[U"Belong_addon_set_name"].getArray<String>();
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
	Array<String> addon_categories_strarray = addonData[U"Categories"].getArray<String>();
	for (auto addon_category_str : addon_categories_strarray) {
		m_addon_categories << UnitaryTools::categoryNameToCategoryID(addon_category_str);
	}
	
	// 建物の効果
	for (const auto& effect : addonData[U"effects"].objectView()) {
		RateID::Type rate_id = UnitaryTools::rateNameToRateID(effect.name);
		m_effects[rate_id].influence = effect.value[U"influence"].get<int>();
		m_effects[rate_id].grid = effect.value[U"grid"].get<int>();
	}
	
	for (const auto& type : addonData[U"Types"].arrayView()) {					// AddonType
		TypeID::Type typeID = UnitaryTools::typeNameToTypeID(type[U"type_name"].getString());
		
		m_use_types << UnitaryTools::typeIDToTypeName(typeID);
		
		for (const auto& direction : type[U"Directions"].arrayView()) {			// AddonDirectionStruct
			DirectionID::Type direction_id = UnitaryTools::directionNameToDirectionID(direction[U"direction_name"].getString());
			
			if (direction_id == DirectionID::None && typeID == TypeID::IntersectionCross) {
				direction_id = DirectionID::All;
			}
			
			// 新たなAddonDirectionStructを作成
			AddonDirectionStruct direction_struct;
			
			direction_struct.directionID = direction_id;
			
			direction_struct.size.x = direction[U"size.width"].get<int>();
			direction_struct.size.y = direction[U"size.height"].get<int>();
			
			direction_struct.requiredTiles.x = direction[U"squares.width"].get<int>();
			direction_struct.requiredTiles.y = direction[U"squares.height"].get<int>();
			
			// requiredTiles = (0, 0)の場合->(1, 1)に修正
			if (direction_struct.requiredTiles.x == 0) {
				direction_struct.requiredTiles.x = 1;
				need_to_convert = true;
			}
			if (direction_struct.requiredTiles.y == 0) {
				direction_struct.requiredTiles.y = 1;
				need_to_convert = true;
			}
			
			direction_struct.topLeft.x = direction[U"top_left.x"].get<int>();
			direction_struct.topLeft.y = direction[U"top_left.y"].get<int>();
			
			direction_struct.bottomRight.x = direction[U"bottom_right.x"].get<int>();
			direction_struct.bottomRight.y = direction[U"bottom_right.y"].get<int>();
			
			// direction_structをlayerに追加
			m_types[typeID].addAddonDirectionStruct(direction_struct);
		}
		
		// r141以前のadjの場合
		if (version <= 141) {
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
		}
		else {
			Array<AddonLayer> layers;
			for (const auto& layer : type[U"Layers"].arrayView()) {								// AddonLayer r142以降
				String image_path = layer[U"image"].getString();
				
				Color transparent_color;
				transparent_color.r = layer[U"transparent_color.R"].get<int>();
				transparent_color.g = layer[U"transparent_color.G"].get<int>();
				transparent_color.b = layer[U"transparent_color.B"].get<int>();
				
				Array<LayerType::Type> layer_types;
				for (const auto& layer_type_str : layer[U"layer_types"].arrayView()) {
					layer_types << UnitaryTools::layerNameToLayerType(layer_type_str.getString());
				}
				
				layers << AddonLayer(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+image_path, transparent_color, layer_types);
			}
			m_types[typeID].setLayers(layers);
		}
	}
	
	// アドオンデータの修正の必要がある場合は修正
	if (need_to_convert) {
		cout << "update addon file: " << m_addon_file_path.file_path << endl;
		m_converter();
	}
	
	return true;
}

String Addon::getName(NameMode::Type mode) {
	if (mode == NameMode::English) {
		return m_addon_name;
	}
	else {
		return m_addon_jp_name;
	}
}

String Addon::getAuthorName() {
	return m_addon_author;
}

String Addon::getSummary() {
	return m_addon_summary;
}
/*
String Addon::getTypeName(int type_num) {
	map<String, AddonTypeStruct>::iterator ite = types.begin();
	advance(ite, type_num);
	return ite->first;
}

String Addon::getDirectionName(int type_num, int direction_num) {
	map<String, AddonDirectionStruct>::iterator ite = types[getTypeName(type_num)].directions.begin();
	advance(ite, direction_num);
	return ite->first;
}
*/
TypeID::Type Addon::getTypeID(int typeNum) {
	return UnitaryTools::typeNameToTypeID(m_use_types[typeNum]);
}

DirectionID::Type Addon::getDirectionID(int typeNum, int directionNum) {
	return m_types[getTypeID(typeNum)].getDirectionID(directionNum);
}

DirectionID::Type Addon::getDirectionID(String typeName, int directionNum) {
	return m_types[UnitaryTools::typeNameToTypeID(typeName)].getDirectionID(directionNum);
}

AddonDirectionStruct Addon::getDirectionStruct(TypeID::Type arg_type_id, DirectionID::Type arg_direction_id) {
	return m_types[arg_type_id].getDirectionStruct(arg_direction_id);
}

Array<CategoryID::Type> Addon::getCategories() {
	return m_addon_categories;
}

bool Addon::isInCategories(CategoryID::Type searchCategory) {
	for (int i=0; i<m_addon_categories.size(); i++) {
		if (m_addon_categories[i] == searchCategory) {
			return true;
		}
	}
	return false;
}

bool Addon::isInCategories(Array<CategoryID::Type> searchCategories) {
	for (int i=0; i<m_addon_categories.size(); i++) {
		for (int j=0; j<searchCategories.size(); j++) {
			if (m_addon_categories[i] == searchCategories[j]) {
				return true;
			}
		}
	}
	return false;
}

map<RateID::Type, EffectStruct> Addon::getEffects() {
	return m_effects;
}

void Addon::drawIcon(PositionStruct position, PositionStruct leftTop, Size size) {
	m_icon_texture(leftTop.x, leftTop.y, size.x, size.y).draw(position.x, position.y);
}

CoordinateStruct Addon::getUseTiles(TypeID::Type typeID, DirectionID::Type directionID) {
	cout << "Return: " << m_types[typeID].getDirectionStruct(directionID).requiredTiles.x << "," << m_types[typeID].getDirectionStruct(directionID).requiredTiles.y << endl;
	return CoordinateStruct{m_types[typeID].getDirectionStruct(directionID).requiredTiles.x, m_types[typeID].getDirectionStruct(directionID).requiredTiles.y};
}

void Addon::draw(TypeID::Type typeID, DirectionID::Type directionID, PositionStruct position, RelativeCoordinateStruct tilesCount, Color addColor, TimeStruct time) {
	m_types[typeID].draw(time, directionID, position, tilesCount, addColor);
}

void Addon::m_load_layer_before141(int layer_num, JSONValue type, AddonType& arg_addon_type, Array<AddonLayer>& arg_layers) {
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
	*/
	
	// layerをlayersに追加
	arg_layers << layer;
}

void Addon::m_converter() {
	JSONWriter addonData;
	
	addonData.startObject();
	{
		addonData.key(U"name").write(m_addon_name);
		addonData.key(U"jp_name").write(m_addon_jp_name);
		
		addonData.key(U"author").write(m_addon_author);
		addonData.key(U"summary").write(m_addon_summary);
		
		addonData.key(U"version").write(RELEASE_NUMBER);
		
		addonData.key(U"Belong_addon_set_name").startArray();
		{
			for (auto belong = m_belong_addons_set_name.begin(); belong!= m_belong_addons_set_name.end() ; belong++) {
				addonData.write(*belong);
			}
		}
		addonData.endArray();
		
		addonData.key(U"icon").write(m_addon_icon);
		
		addonData.key(U"Categories").startArray();
		{
			for (auto categoryName = m_addon_categories.begin(); categoryName != m_addon_categories.end(); categoryName++) {
				addonData.write(*categoryName);
			}
		}
		addonData.endArray();
		
		addonData.key(U"effects").startObject();
		{
			for (auto e = m_effects.begin(); e != m_effects.end() ; e++) {
				if (e->second.influence != 0) {
					addonData.key(UnitaryTools::rateIDToRateName(e->first)).startObject();
					{
						addonData.key(U"influence").write(e->second.influence);
						addonData.key(U"grid").write(e->second.grid);
					}
					addonData.endObject();
				}
			}
		}
		addonData.endObject();
		
		addonData.key(U"maximum_capacity").write(m_maximum_capacity);
		
		addonData.key(U"Types").startArray();
		{
			for (auto type = m_types.begin(); type != m_types.end(); type++) {
				addonData.startObject();
				{
					addonData.key(U"type_name").write(UnitaryTools::typeIDToTypeName(type->first));
					
					map<DirectionID::Type, AddonDirectionStruct> direction_structs = type->second.getDirectionStructs();
					addonData.key(U"direction_names").startArray();
					{
						for (auto direction = direction_structs.begin(); direction != direction_structs.end(); direction++) {
							addonData.write(UnitaryTools::directionIDToDirectionName(direction->first));
						}
					}
					addonData.endArray();
					
					addonData.key(U"Directions").startArray();
					{
						for (auto direction = direction_structs.begin(); direction != direction_structs.end(); direction++) {
							addonData.startObject();
							{
								addonData.key(U"direction_name").write(UnitaryTools::directionIDToDirectionName(direction->first));
								
								addonData.key(U"size").startObject();
								{
									addonData.key(U"width").write(direction->second.size.x);
									addonData.key(U"height").write(direction->second.size.y);
								}
								addonData.endObject();
								addonData.key(U"squares").startObject();
								{
									addonData.key(U"width").write(direction->second.requiredTiles.x);
									addonData.key(U"height").write(direction->second.requiredTiles.y);
								}
								addonData.endObject();
								addonData.key(U"top_left").startObject();
								{
									addonData.key(U"x").write(direction->second.topLeft.x);
									addonData.key(U"y").write(direction->second.topLeft.y);
								}
								addonData.endObject();
								addonData.key(U"bottom_right").startObject();
								{
									addonData.key(U"x").write(direction->second.bottomRight.x);
									addonData.key(U"y").write(direction->second.bottomRight.y);
								}
								addonData.endObject();
							}
							addonData.endObject();
						}
						addonData.endArray();
					}
					
					addonData.key(U"Layers").startArray();
					{
						Array<AddonLayer>& layers = type->second.getLayers();
						for (auto layer = layers.begin(); layer != layers.end(); layer++) {
							addonData.startObject();
							{
								addonData.key(U"image").write(FileSystem::FileName(layer->getImagePath()));
								
								addonData.key(U"transparent_color").startObject();
								{
									Color transparennt_color = layer->getTransparentColor();
									addonData.key(U"R").write(transparennt_color.r);
									addonData.key(U"G").write(transparennt_color.g);
									addonData.key(U"B").write(transparennt_color.b);
								}
								addonData.endObject();
								
								addonData.key(U"layer_types").startArray();
								{
									Array<LayerType::Type> layer_types = layer->getLayerTypesInit();
									for (int layer_type_num = 0; layer_type_num < layer_types.size(); layer_type_num++) {
										addonData.write(UnitaryTools::layerTypeToLayerName(layer_types[layer_type_num]));
									}
								}
								addonData.endArray();
							}
							addonData.endObject();
						}
						addonData.endArray();
					}
				}
				addonData.endObject();
			}
		}
		addonData.endArray();
	}
	addonData.endObject();
	
	cout << FileSystem::ParentPath(Unicode::Widen(m_addon_file_path.file_path))+FileSystem::BaseName(Unicode::Widen(m_addon_file_path.file_path))+U".adj" << endl;
	addonData.save(FileSystem::ParentPath(Unicode::Widen(m_addon_file_path.file_path))+FileSystem::BaseName(Unicode::Widen(m_addon_file_path.file_path))+U".adj");
}
