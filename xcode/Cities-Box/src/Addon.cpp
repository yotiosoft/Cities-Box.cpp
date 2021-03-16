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
		return m_load_adat(newFilePath, loadingAddonsSetName);
	}
	else if (FileSystem::Extension(Unicode::Widen(newFilePath.file_path)) == U"adj") {
		return m_load_adj(newFilePath, loadingAddonsSetName);
	}
	return false;
}

bool Addon::m_load_adat(FileStruct newFilePath, String loadingAddonsSetName) {
	// アドオンファイルの読み込み
	m_addon_file_path = newFilePath;
	
	TextReader addonData(Unicode::Widen(m_addon_file_path.file_path));
	string strTempUTF8;
	String strTemp;
	
	// 各要素の読み出し
	String currentLoadingType = U"";
	String currentDirection = U"";
	
	bool loadingType = false;
	bool loadingDirection = false;
	
	Color transparentColor;
	transparentColor.r = 0;
	transparentColor.g = 0;
	transparentColor.b = 0;
	
	bool categories[3] = {false, false, false};
	bool belong = false;
	
	while (addonData.readLine(strTemp)) {
		strTemp = strTemp.substr(0, strTemp.length()-LINE_FEED_CODE);
		strTempUTF8 = strTemp.toUTF8();
		
		// 名前
		m_get_element(strTemp, U"addon_name", m_addon_name);
		m_get_element(strTemp, U"addon_jp_name", m_addon_jp_name);
		
		// 製作者名
		m_get_element(strTemp, U"addon_author", m_addon_author);
		
		// 説明文
		m_get_element(strTemp, U"addon_summary", m_addon_summary);
		
		// 所属するアドオンセットの名前
		String belongTemp;
		if (m_get_element(strTemp, U"belong_addons_set_name", belongTemp) && !belong) {
			m_belong_addons_set_name << belongTemp;
			
			if (belongTemp.length() > 0) {
				if (belongTemp != loadingAddonsSetName || !(loadingAddonsSetName.length() > 0)) {
					return false;
				}
			}
			
			belong = true;
		}
		
		// アイコン画像のパス
		m_get_element(strTemp, U"addon_icon", m_addon_icon);
		
		// アイコンを読み込み
		Image iconImage(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+m_addon_icon);
		m_set_alpha_color(iconImage, Color(0, 0, 0));
		m_icon_texture = Texture(iconImage);
		
		// アドオンのtype
		String categoryNameTemp;
		m_get_element(strTemp, U"addon_type", categoryNameTemp);
		
		if (categoryNameTemp.length() > 0 && !categories[0]) {
			string categoryNameTempUTF8 = categoryNameTemp.toUTF8();
			
			transform(categoryNameTempUTF8.begin(), categoryNameTempUTF8.end(), categoryNameTempUTF8.begin(), ::tolower);
			
			if (categoryNameTempUTF8.find("road") != string::npos && categoryNameTempUTF8.find("railroad") == string::npos) {
				m_addon_categories << U"connectable_type";
				m_addon_categories << U"road";
				m_addon_categories << U"car";
			}
			else if (categoryNameTempUTF8.find("promenade") != string::npos) {
				m_addon_categories << U"connectable_type";
				m_addon_categories << U"road";
				m_addon_categories << U"promenade";
			}
			else if (categoryNameTempUTF8.find("railroad") != string::npos) {
				m_addon_categories << U"connectable_type";
				m_addon_categories << U"train";
				m_addon_categories << U"railroad";
			}
			else if (categoryNameTempUTF8.find("station") != string::npos) {
				m_addon_categories << U"connectable_type";
				m_addon_categories << U"train";
				m_addon_categories << U"station";
			}
			else if (categoryNameTempUTF8.find("residential") != string::npos) {
				m_addon_categories << U"object_type";
				m_addon_categories << U"residential";
			}
			else if (categoryNameTempUTF8.find("commercial") != string::npos) {
				m_addon_categories << U"object_type";
				m_addon_categories << U"commercial";
			}
			else if (categoryNameTempUTF8.find("office") != string::npos) {
				m_addon_categories << U"object_type";
				m_addon_categories << U"office";
			}
			else if (categoryNameTempUTF8.find("industrial") != string::npos) {
				m_addon_categories << U"object_type";
				m_addon_categories << U"industrial";
			}
			else if (categoryNameTempUTF8.find("farm") != string::npos) {
				m_addon_categories << U"object_type";
				m_addon_categories << U"farm";
			}
			else if (categoryNameTempUTF8.find("public") != string::npos) {
				m_addon_categories << U"object_type";
				m_addon_categories << U"public";
			}
			else if (categoryNameTempUTF8.find("park") != string::npos) {
				m_addon_categories << U"object_type";
				m_addon_categories << U"park";
			}
			else if (categoryNameTempUTF8.find("port") != string::npos) {
				m_addon_categories << U"on_waterway_type";
				m_addon_categories << U"ship";
				m_addon_categories << U"port";
			}
			else if (categoryNameTempUTF8.find("waterway") != string::npos) {
				m_addon_categories << U"connectable_type";
				m_addon_categories << U"ship";
				m_addon_categories << U"waterway";
			}
			else if (categoryNameTempUTF8.find("airport") != string::npos) {
				m_addon_categories << U"airport";
			}
			else if (categoryNameTempUTF8.find("tile") != string::npos) {
				m_addon_categories << U"put_type";
				m_addon_categories << U"tile";
			}
			
			if (categoryNameTempUTF8.find("low_density") != string::npos) {
				m_addon_categories << U"low_density";
			}
			else if (categoryNameTempUTF8.find("high_density") != string::npos) {
				m_addon_categories << U"high_density";
			}
			
			if (categoryNameTempUTF8.find("two_lane") != string::npos) {
				m_addon_categories << U"two_lane";
			}
			
			categories[0] = true;
		}
		
		String categoryNameTemp2;
		m_get_element(strTemp, U"addon_type_2", categoryNameTemp2);
		
		if (categoryNameTemp2.length() > 0 && !categories[1]) {
			string categoryNameTemp2UTF8 = categoryNameTemp2.toUTF8();
			
			transform(categoryNameTemp2UTF8.begin(), categoryNameTemp2UTF8.end(), categoryNameTemp2UTF8.begin(), ::tolower);
			
			if (categoryNameTemp2UTF8.find("city_hall") != string::npos) {
				m_addon_categories << U"city_hall";
			}
			else if (categoryNameTemp2UTF8.find("education") != string::npos) {
				m_addon_categories << U"education";
			}
			else if (categoryNameTemp2UTF8.find("post_office") != string::npos) {
				m_addon_categories << U"post_office";
			}
			else if (categoryNameTemp2UTF8.find("education") != string::npos) {
				m_addon_categories << U"education";
			}
			else if (categoryNameTemp2UTF8.find("fire_depertment") != string::npos) {
				m_addon_categories << U"fire_depertment";
			}
			else if (categoryNameTemp2UTF8.find("police_station") != string::npos) {
				m_addon_categories << U"police";
				m_addon_categories << U"police_station";
			}
			
			categories[1] = true;
		}
		
		String categoryNameTemp3;
		m_get_element(strTemp, U"addon_type_3", categoryNameTemp3);
		
		if (categoryNameTemp3.length() > 0 && !categories[2]) {
			string categoryNameTemp3UTF8 = categoryNameTemp3.toUTF8();
			
			transform(categoryNameTemp3UTF8.begin(), categoryNameTemp3UTF8.end(), categoryNameTemp3UTF8.begin(), ::tolower);
			
			if (categoryNameTemp3UTF8.find("elementary-school") != string::npos) {
				m_addon_categories << U"elementary_school";
			}
			else if (categoryNameTemp3UTF8.find("junior-high-school") != string::npos) {
				m_addon_categories << U"junior_high_school";
			}
			else if (categoryNameTemp3UTF8.find("high-school") != string::npos) {
				m_addon_categories << U"high_school";
			}
			else if (categoryNameTemp3UTF8.find("university") != string::npos) {
				m_addon_categories << U"university";
			}
			
			categories[2] = true;
		}
		
		// 最大収容人数
		m_get_element(strTemp, U"maximum_capacity", m_maximum_capacity);
		
		// 建物の効果
		m_get_element(strTemp, U"land_price_influence", m_effects[U"land_price"].influence);
		m_get_element(strTemp, U"land_price_influence_grid", m_effects[U"land_price"].grid);
		
		m_get_element(strTemp, U"education_rate_influence", m_effects[U"education_rate"].influence);
		m_get_element(strTemp, U"education_rate_influence_grid", m_effects[U"education_rate"].grid);
		
		m_get_element(strTemp, U"crime_rate_influence", m_effects[U"crime_rate"].influence);
		m_get_element(strTemp, U"crime_rate_influence_grid", m_effects[U"crime_rate"].grid);
		
		m_get_element(strTemp, U"noise_influence", m_effects[U"noise"].influence);
		m_get_element(strTemp, U"noise_influence_grid", m_effects[U"noise"].grid);
		
		// 使用するtype
		m_get_types(strTemp, U"use_types", m_use_types);
		
		// 各typeの内容を取得
		// 現在読込中のtypeを取得
		for (int i=0; i<m_use_types.size(); i++) {
			if (strTempUTF8.find(m_use_types[i].toUTF8() + " {") == 0 && !loadingType) {
				if (m_use_types[i] == U"null") {
					m_use_types[i] = U"normal";
				}
				
				currentLoadingType = m_use_types[i];
				loadingType = true;
			}
		}
		if (strTempUTF8.find("}") == 0 && !loadingDirection) {
			// typeが切り替わるときにTextureの設定
			if (m_types[currentLoadingType].image.length() > 0) {
				Image iTemp(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+m_types[currentLoadingType].image);
				m_set_alpha_color(iTemp, transparentColor);
				m_blend_color_and_image(iTemp, Color(0, 0, 0, 200));
				m_types[currentLoadingType].texture = Texture(iTemp);
			}
			
			currentDirection = U"";
			loadingType = false;
		}
		
		if (currentLoadingType != U"") {
			// 画像のパス
			m_get_element(strTemp, U"image", m_types[currentLoadingType].image);
			
			// 透過色
			String RGBStr;
			m_get_element(strTemp, U"transparent_color", RGBStr);
			
			if (RGBStr.length() > 0) {
				Array<String> RGBStrV = split(RGBStr, U", ");
				
				if (RGBStrV.size() == 3) {
					transparentColor.r = stoi(RGBStrV[0].toUTF8());
					transparentColor.g = stoi(RGBStrV[1].toUTF8());
					transparentColor.b = stoi(RGBStrV[2].toUTF8());
				}
			}
			
			// ナイトマスク画像のパス
			m_get_element(strTemp, U"night_mask", m_types[currentLoadingType].nightMask);
			
			// typeに含まれる方向と各方向の情報を取得
			m_get_types(strTemp, U"direction", m_types[currentLoadingType].directionNames);
			
			for (int i=0; i<m_types[currentLoadingType].directionNames.size(); i++) {
				if (strTempUTF8.find(m_types[currentLoadingType].directionNames[i].toUTF8()+" {") != string::npos && !loadingDirection) {
					if (m_types[currentLoadingType].directionNames[i] == U"null") {
						m_types[currentLoadingType].directionNames[i] = U"normal";
					}
					currentDirection = m_types[currentLoadingType].directionNames[i];
				}
			}
			if (strTempUTF8.find("}") != string::npos && loadingDirection) {
				currentDirection = U"";
				loadingDirection = false;
			}
			
			if (currentDirection != U"") {
				AddonDirectionStruct direction_temp;
				
				// アドオンの大きさ
				m_get_element(strTemp, U"size_x", direction_temp.size.x);		// 横
				m_get_element(strTemp, U"size_y", direction_temp.size.y);		// 縦
				
				// アドオンが占めるマスの数
				m_get_element(strTemp, U"chip_x", direction_temp.requiredTiles.x);			// 横
				m_get_element(strTemp, U"chip_y", direction_temp.requiredTiles.y);			// 縦
				
				// 画像上の左上の座標
				m_get_element(strTemp, U"top_left_x", direction_temp.topLeft.x);
				m_get_element(strTemp, U"top_left_y", direction_temp.topLeft.y);
				
				// 画面上の右下の座標
				m_get_element(strTemp, U"bottom_right_x", direction_temp.bottomRight.x);
				m_get_element(strTemp, U"bottom_right_y", direction_temp.bottomRight.y);
				
				m_types[currentLoadingType].directions[currentDirection] = direction_temp;
			}
		}
	}
	
	if (!belong) {
		m_belong_addons_set_name << U"Normal";
	}
	
	converter();
	
	return true;
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
	
	for (const auto& type : addonData[U"Types"].arrayView()) {
		String typeName = type[U"type_name"].getString();
		
		m_types[typeName].image = type[U"image"].getString();
		
		m_types[typeName].transparentColor.r = type[U"transparent_color.R"].get<int>();
		m_types[typeName].transparentColor.g = type[U"transparent_color.G"].get<int>();
		m_types[typeName].transparentColor.b = type[U"transparent_color.B"].get<int>();
		
		Image iTemp(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+m_types[typeName].image);
		m_set_alpha_color(iTemp, Color(m_types[typeName].transparentColor.r, m_types[typeName].transparentColor.g, m_types[typeName].transparentColor.b));
		m_blend_color_and_image(iTemp, Color(0, 0, 0, 200));
		m_types[typeName].texture = Texture(iTemp);
		
		m_types[typeName].nightMask = type[U"night_mask"].getString();
		if (FileSystem::IsFile(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+m_types[typeName].nightMask)) {
			Image iTempNM(Unicode::Widen(m_addon_file_path.folder_path)+U"/"+m_types[typeName].nightMask);
			m_set_alpha_color(iTempNM, Color(m_types[typeName].transparentColor.r, m_types[typeName].transparentColor.g, m_types[typeName].transparentColor.b));
			m_types[typeName].nightMaskTexture = Texture(iTempNM);
		}
		
		m_types[typeName].directionNames = type[U"direction_names"].getArray<String>();
		
		for (const auto& direction : type[U"Directions"].arrayView()) {
			String direction_name = direction[U"direction_name"].getString();
			
			m_types[typeName].directions[direction_name].size.x = direction[U"size.width"].get<int>();
			m_types[typeName].directions[direction_name].size.y = direction[U"size.height"].get<int>();
			
			m_types[typeName].directions[direction_name].requiredTiles.x = direction[U"squares.width"].get<int>();
			m_types[typeName].directions[direction_name].requiredTiles.y = direction[U"squares.height"].get<int>();
			
			m_types[typeName].directions[direction_name].topLeft.x = direction[U"top_left.x"].get<int>();
			m_types[typeName].directions[direction_name].topLeft.y = direction[U"top_left.y"].get<int>();
			
			m_types[typeName].directions[direction_name].bottomRight.x = direction[U"bottom_right.x"].get<int>();
			m_types[typeName].directions[direction_name].bottomRight.y = direction[U"bottom_right.y"].get<int>();
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
String Addon::getTypeName(int typeNum) {
	return m_use_types[typeNum];
}

String Addon::getDirectionName(int typeNum, int directionNum) {
	return m_types[getTypeName(typeNum)].directionNames[directionNum];
}

String Addon::getDirectionName(String typeName, int directionNum) {
	return m_types[typeName].directionNames[directionNum];
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

void Addon::drawIcon(PositionStruct position, PositionStruct leftTop, SizeStruct size) {
	m_icon_texture(leftTop.x, leftTop.y, size.width, size.height).draw(position.x, position.y);
}

CoordinateStruct Addon::getUseTiles(String typeName, String directionName) {
	return CoordinateStruct{m_types[typeName].directions[directionName].requiredTiles.x, m_types[typeName].directions[directionName].requiredTiles.y};
}

PositionStruct Addon::getPosition(String typeName, String directionName, PositionStruct position, CoordinateStruct useTiles, CoordinateStruct tilesCount) {
	AddonDirectionStruct* directionTemp = &(m_types[typeName].directions[directionName]);
	if (directionTemp != nullptr)
		position.y = position.y + CHIP_SIZE/2 - directionTemp->size.y + CHIP_SIZE/4 * (max(1, useTiles.x) - 1 - tilesCount.x) + CHIP_SIZE*3/4 * tilesCount.y;
	
	return position;
}

void Addon::draw(String typeName, String directionName, PositionStruct position, CoordinateStruct useTiles, CoordinateStruct tilesCount, Color* addColor) {
	AddonDirectionStruct* directionTemp = &(m_types[typeName].directions[directionName]);
	
	/*
	if (cursor.coordinate.x == coordinate.x && cursor.coordinate.y == coordinate.y) {
		cout << coordinate.x << "," << coordinate.y << " :  " << tiles_count.x << "," << tiles_count.y << endl;
	}*/
	
	//position.x = position.x + tiles_count.x * CHIP_SIZE/8;
	position = getPosition(typeName, directionName, position, useTiles, tilesCount);
	
	unsigned short int topLeftX = directionTemp->topLeft.x;
	topLeftX += CHIP_SIZE/2 * tilesCount.x + CHIP_SIZE/2 * tilesCount.y;
	
	unsigned short int topLeftY = directionTemp->topLeft.y;
	topLeftY += CHIP_SIZE/2 * tilesCount.y;
	
	unsigned short int sizeWidth = directionTemp->size.x;
	sizeWidth = CHIP_SIZE;
	
	unsigned short int sizeHeight = directionTemp->size.y;
	
	if (addColor->a > 0) {
		m_types[typeName].texture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y, *addColor);
		if (!m_types[typeName].nightMaskTexture.isEmpty()) {
			m_types[typeName].nightMaskTexture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y, *addColor);
		}
	}
	else {
		m_types[typeName].texture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y);
		if (!m_types[typeName].nightMaskTexture.isEmpty()) {
			m_types[typeName].nightMaskTexture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y);
		}
	}
}

void Addon::converter() {
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
					addonData.key(e->first).startObject();
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
		
		addonData.key(U"Use_types").startArray();
		{
			for (auto typeName = m_use_types.begin(); typeName != m_use_types.end() ; typeName++) {
				addonData.write(*typeName);
			}
		}
		addonData.endArray();
		
		addonData.key(U"Types").startArray();
		{
			for (auto type = m_types.begin(); type != m_types.end(); type++) {
				addonData.startObject();
				{
					addonData.key(U"type_name").write(type->first);
					addonData.key(U"image").write(type->second.image);
					addonData.key(U"night_mask").write(type->second.nightMask);
					addonData.key(U"transparent_color").startObject();
					{
						addonData.key(U"R").write(type->second.transparentColor.r);
						addonData.key(U"G").write(type->second.transparentColor.g);
						addonData.key(U"B").write(type->second.transparentColor.b);
					}
					addonData.endObject();
					
					addonData.key(U"direction_names").startArray();
					{
						for (auto directionName = type->second.directionNames.begin(); directionName != type->second.directionNames.end(); directionName++) {
							addonData.write(*directionName);
						}
					}
					addonData.endArray();
					
					addonData.key(U"Directions").startArray();
					{
						for (auto direction = type->second.directions.begin(); direction != type->second.directions.end(); direction++) {
							addonData.startObject();
							{
								addonData.key(U"direction_name").write(direction->first);
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
				}
				addonData.endObject();
			}
		}
		addonData.endArray();
	}
	addonData.endObject();
	
	addonData.save(FileSystem::ParentPath(Unicode::Widen(m_addon_file_path.file_path))+FileSystem::BaseName(Unicode::Widen(m_addon_file_path.file_path))+U".adj");
}
