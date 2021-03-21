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
		ret = split(aRet, U", ");
		return true;
	}
	return false;
}

void Addon::m_set_alpha_color(Image& imageTemp, Color transparentRGB) {
	for (int h=0; h<imageTemp.height(); h++) {
		for (int w=0; w<imageTemp.width(); w++) {
			if (imageTemp[h][w].r == transparentRGB.r && imageTemp[h][w].g == transparentRGB.g && imageTemp[h][w].b == transparentRGB.b) {
				imageTemp[h][w].a = 0;
			}
		}
	}
}

void Addon::m_blend_color_and_image(Image& imageTemp, Color blendColor) {
	double outA, outR, outG, outB;
	for (int h=0; h<imageTemp.height(); h++) {
		for (int w=0; w<imageTemp.width(); w++) {
			double bcA = blendColor.a / 255.0;
			double itA = imageTemp[h][w].a / 255.0;
			
			outA = bcA+itA*(1.0-bcA);
			if (outA < 1.0) {
				imageTemp[h][w].a = 0;
				imageTemp[h][w].r = 0;
				imageTemp[h][w].g = 0;
				imageTemp[h][w].b = 0;
			}
			else {
				outR = (blendColor.r*bcA+imageTemp[h][w].r*itA*(1.0-bcA))/outA;
				outG = (blendColor.g*itA+imageTemp[h][w].g*itA*(1.0-bcA))/outA;
				outB = (blendColor.b*itA+imageTemp[h][w].b*itA*(1.0-bcA))/outA;
				
				imageTemp[h][w].a = outA*255;
				imageTemp[h][w].r = outR;
				imageTemp[h][w].g = outG;
				imageTemp[h][w].b = outB;
			}
		}
	}
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
	
	m_addon_name = addonData[U"name"].getString();
	m_addon_jp_name = addonData[U"jp_name"].getString();
	
	m_addon_author = addonData[U"author"].getString();
	m_addon_summary = addonData[U"summary"].getString();
	
	m_addon_icon = addonData[U"icon"].getString();
	
	// アイコンを読み込み
	Image iconImage(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+m_addon_icon);
	m_set_alpha_color(iconImage, Color(0, 0, 0));
	m_icon_texture = Texture(iconImage);
	
	// カテゴリ
	m_addon_categories = addonData[U"Categories"].getArray<String>();
	
	// 建物の効果
	for (const auto& effect : addonData[U"effects"].objectView()) {
		m_effects[effect.name].influence = effect.value[U"influence"].get<int>();
		m_effects[effect.name].grid = effect.value[U"grid"].get<int>();
	}
	
	m_use_types = addonData[U"Use_types"].getArray<String>();
	
	for (const auto& type : addonData[U"Types"].arrayView()) {					// AddonType
		TypeID::Type typeID = typeNameToTypeID(type[U"type_name"].getString());
		
		m_types[typeID].directionNames = directionNameToDirectionID(type[U"direction_names"].getArray<String>());
		
		for (const auto& direction : type[U"Directions"].arrayView()) {			// AddonDirectionStruct
			DirectionID::Type direction_id = directionNameToDirectionID(direction[U"direction_name"].getString());
			
			// 新たなAddonDirectionStructを作成
			AddonDirectionStruct direction_struct;
			
			direction_struct.directionID = direction_id;
			
			direction_struct.size.x = direction[U"size.width"].get<int>();
			direction_struct.size.y = direction[U"size.height"].get<int>();
			
			direction_struct.requiredTiles.x = direction[U"squares.width"].get<int>();
			direction_struct.requiredTiles.y = direction[U"squares.height"].get<int>();
			
			direction_struct.topLeft.x = direction[U"top_left.x"].get<int>();
			direction_struct.topLeft.y = direction[U"top_left.y"].get<int>();
			
			direction_struct.bottomRight.x = direction[U"bottom_right.x"].get<int>();
			direction_struct.bottomRight.y = direction[U"bottom_right.y"].get<int>();
			
			// direction_structをlayerに追加
			m_types[typeID].addAddonDirectionStruct(direction_struct);
		}
		
		for (int layer_num=0; layer_num<1; layer_num++) {						// AddonLayer Todo: 複数のレイヤに対応する
			String image_filename = type[U"image"].getString();
			
			m_types[typeID].transparentColor.r = type[U"transparent_color.R"].get<int>();
			m_types[typeID].transparentColor.g = type[U"transparent_color.G"].get<int>();
			m_types[typeID].transparentColor.b = type[U"transparent_color.B"].get<int>();
			
			Image iTemp(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+image_filename);
			m_set_alpha_color(iTemp, Color(m_types[typeID].transparentColor.r, m_types[typeID].transparentColor.g, m_types[typeID].transparentColor.b));
			m_blend_color_and_image(iTemp, Color(0, 0, 0, 200));
			
			// AddonLayerを作成（暫定）
			// Todo: 正式に対応する
			Array<LayerType::Type> layer_types;
			layer_types << LayerType::Normal;
			AddonLayer layer(iTemp, layer_types);
			
			/*
			String night_mask_filename = type[U"night_mask"].getString();
			if (FileSystem::IsFile(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+night_mask_filename)) {
				Image iTempNM(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+night_mask_filename);
				m_set_alpha_color(iTempNM, Color(m_types[typeID].transparentColor.r, m_types[typeID].transparentColor.g, m_types[typeID].transparentColor.b));
				m_types[typeID].nightMaskTexture = Texture(iTempNM);
			}
			*/
			
			// layerをtypeに追加
			m_types[typeID].addAddonLayer(layer);
		}
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
	return typeNameToTypeID(m_use_types[typeNum]);
}

DirectionID::Type Addon::getDirectionID(int typeNum, int directionNum) {
	return m_types[getTypeID(typeNum)].directionNames[directionNum];
}

DirectionID::Type Addon::getDirectionID(String typeName, int directionNum) {
	return m_types[typeNameToTypeID(typeName)].directionNames[directionNum];
}

Array<String> Addon::getCategories() {
	return m_addon_categories;
}

bool Addon::isInCategories(String searchCategory) {
	for (int i=0; i<m_addon_categories.size(); i++) {
		if (m_addon_categories[i] == searchCategory) {
			return true;
		}
	}
	return false;
}

bool Addon::isInCategories(Array<String> searchCategories) {
	for (int i=0; i<m_addon_categories.size(); i++) {
		for (int j=0; j<searchCategories.size(); j++) {
			if (m_addon_categories[i] == searchCategories[j]) {
				return true;
			}
		}
	}
	return false;
}

map<String, EffectStruct> Addon::getEffects() {
	return m_effects;
}

void Addon::drawIcon(PositionStruct position, PositionStruct leftTop, Size size) {
	m_icon_texture(leftTop.x, leftTop.y, size.x, size.y).draw(position.x, position.y);
}

CoordinateStruct Addon::getUseTiles(TypeID::Type typeID, DirectionID::Type directionID) {
	return CoordinateStruct{m_types[typeID].getDirectionStruct(directionID).requiredTiles.x, m_types[typeID].getDirectionStruct(directionID).requiredTiles.y};
}

PositionStruct Addon::getPosition(TypeID::Type typeID, DirectionID::Type directionID, PositionStruct position, CoordinateStruct useTiles, CoordinateStruct tilesCount) {
	AddonDirectionStruct directionTemp = m_types[typeID].getDirectionStruct(directionID);
	position.y = position.y + CHIP_SIZE/2 - directionTemp.size.y + CHIP_SIZE/4 * (max(1, useTiles.x) - 1 - tilesCount.x) + CHIP_SIZE*3/4 * tilesCount.y;
	
	return position;
}

void Addon::draw(TypeID::Type typeID, DirectionID::Type directionID, PositionStruct position, CoordinateStruct useTiles, CoordinateStruct tilesCount, Color* addColor,
				 TimeStruct time) {
	AddonDirectionStruct directionTemp = m_types[typeID].getDirectionStruct(directionID);
	
	/*
	if (cursor.coordinate.x == coordinate.x && cursor.coordinate.y == coordinate.y) {
		cout << coordinate.x << "," << coordinate.y << " :  " << tiles_count.x << "," << tiles_count.y << endl;
	}*/
	
	//position.x = position.x + tiles_count.x * CHIP_SIZE/8;
	position = getPosition(typeID, directionID, position, useTiles, tilesCount);
	
	unsigned short int topLeftX = directionTemp.topLeft.x;
	topLeftX += CHIP_SIZE/2 * tilesCount.x + CHIP_SIZE/2 * tilesCount.y;
	
	unsigned short int topLeftY = directionTemp.topLeft.y;
	topLeftY += CHIP_SIZE/2 * tilesCount.y;
	
	unsigned short int sizeWidth = directionTemp.size.x;
	sizeWidth = CHIP_SIZE;
	
	unsigned short int sizeHeight = directionTemp.size.y;
	
	// オブジェクトの描画
	for (int i=0; i<m_types[typeID].countLayers(); i++) {
		m_types[typeID].tempGetTexture(time)(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y/*, *addColor*/);
	}
	/*
	if (addColor->a > 0) {
		m_types[typeID].tempGetTexture(time)(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y, *addColor);
		/*if (!m_types[typeID].nightMaskTexture.isEmpty()) {
			m_types[typeID].nightMaskTexture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y, *addColor);
		}*/
	/*}
	else {
		m_types[typeID].tempGetTexture(time)(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y);
		/*if (!m_types[typeID].nightMaskTexture.isEmpty()) {
			m_types[typeID].nightMaskTexture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y);
		}*/
	//}
}
