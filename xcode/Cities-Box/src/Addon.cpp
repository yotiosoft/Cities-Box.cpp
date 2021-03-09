//
//  Addon.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "Addon.hpp"

Addon::Addon() {
}

bool Addon::getElement(String str, String searchElementName, String& ret) {
	string strUTF8 = str.toUTF8();
	
	if (strUTF8.find(searchElementName.toUTF8()) != string::npos && strUTF8.find("=") != string::npos) {
		ret = str.substr(strUTF8.find("\"") + 1, str.length() - (strUTF8.find("\"") + 2));
		return true;
	}
	return false;
}

bool Addon::getElement(String str, String searchElementName, int& ret) {
	string strUTF8 = str.toUTF8();
	
	if (strUTF8.find(searchElementName.toUTF8()) != string::npos && strUTF8.find("=") != string::npos) {
		ret = stoi(strUTF8.substr(strUTF8.find("\"") + 1, str.length() - (strUTF8.find("\"") + 2)));
		return true;
	}
	return false;
}

bool Addon::getTypes(String str, String searchElementName, Array<String>& ret) {
	String aRet;
	if (getElement(str, searchElementName, aRet)) {
		ret = split(aRet, U", ");
		return true;
	}
	return false;
}

void Addon::setAlphaColor(Image& imageTemp, Color transparentRGB) {
	for (int h=0; h<imageTemp.height(); h++) {
		for (int w=0; w<imageTemp.width(); w++) {
			if (imageTemp[h][w].r == transparentRGB.r && imageTemp[h][w].g == transparentRGB.g && imageTemp[h][w].b == transparentRGB.b) {
				imageTemp[h][w].a = 0;
			}
		}
	}
}

void Addon::blendColorAndImage(Image& imageTemp, Color blendColor) {
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
		return loadADAT(newFilePath, loadingAddonsSetName);
	}
	else if (FileSystem::Extension(Unicode::Widen(newFilePath.file_path)) == U"adj") {
		return loadADJ(newFilePath, loadingAddonsSetName);
	}
	return false;
}

bool Addon::loadADAT(FileStruct newFilePath, String loadingAddonsSetName) {
	// アドオンファイルの読み込み
	addonFilePath = newFilePath;
	
	TextReader addonData(Unicode::Widen(addonFilePath.file_path));
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
		getElement(strTemp, U"addon_name", addonName);
		getElement(strTemp, U"addon_jp_name", addonJPName);
		
		// 製作者名
		getElement(strTemp, U"addon_author", addonAuthor);
		
		// 説明文
		getElement(strTemp, U"addon_summary", addonSummary);
		
		// 所属するアドオンセットの名前
		String belongTemp;
		if (getElement(strTemp, U"belong_addons_set_name", belongTemp) && !belong) {
			belongAddonsSetName << belongTemp;
			
			if (belongTemp.length() > 0) {
				if (belongTemp != loadingAddonsSetName || !(loadingAddonsSetName.length() > 0)) {
					return false;
				}
			}
			
			belong = true;
		}
		
		// アイコン画像のパス
		getElement(strTemp, U"addon_icon", addonIcon);
		
		// アイコンを読み込み
		Image iconImage(Unicode::Widen(addonFilePath.folder_path)+U"/"+addonIcon);
		setAlphaColor(iconImage, Color(0, 0, 0));
		iconTexture = Texture(iconImage);
		
		// アドオンのtype
		String categoryNameTemp;
		getElement(strTemp, U"addon_type", categoryNameTemp);
		
		if (categoryNameTemp.length() > 0 && !categories[0]) {
			string categoryNameTempUTF8 = categoryNameTemp.toUTF8();
			
			transform(categoryNameTempUTF8.begin(), categoryNameTempUTF8.end(), categoryNameTempUTF8.begin(), ::tolower);
			
			if (categoryNameTempUTF8.find("road") != string::npos && categoryNameTempUTF8.find("railroad") == string::npos) {
				addonCategories << U"connectable_type";
				addonCategories << U"road";
				addonCategories << U"car";
			}
			else if (categoryNameTempUTF8.find("promenade") != string::npos) {
				addonCategories << U"connectable_type";
				addonCategories << U"road";
				addonCategories << U"promenade";
			}
			else if (categoryNameTempUTF8.find("railroad") != string::npos) {
				addonCategories << U"connectable_type";
				addonCategories << U"train";
				addonCategories << U"railroad";
			}
			else if (categoryNameTempUTF8.find("station") != string::npos) {
				addonCategories << U"connectable_type";
				addonCategories << U"train";
				addonCategories << U"station";
			}
			else if (categoryNameTempUTF8.find("residential") != string::npos) {
				addonCategories << U"object_type";
				addonCategories << U"residential";
			}
			else if (categoryNameTempUTF8.find("commercial") != string::npos) {
				addonCategories << U"object_type";
				addonCategories << U"commercial";
			}
			else if (categoryNameTempUTF8.find("office") != string::npos) {
				addonCategories << U"object_type";
				addonCategories << U"office";
			}
			else if (categoryNameTempUTF8.find("industrial") != string::npos) {
				addonCategories << U"object_type";
				addonCategories << U"industrial";
			}
			else if (categoryNameTempUTF8.find("farm") != string::npos) {
				addonCategories << U"object_type";
				addonCategories << U"farm";
			}
			else if (categoryNameTempUTF8.find("public") != string::npos) {
				addonCategories << U"object_type";
				addonCategories << U"public";
			}
			else if (categoryNameTempUTF8.find("park") != string::npos) {
				addonCategories << U"object_type";
				addonCategories << U"park";
			}
			else if (categoryNameTempUTF8.find("port") != string::npos) {
				addonCategories << U"on_waterway_type";
				addonCategories << U"ship";
				addonCategories << U"port";
			}
			else if (categoryNameTempUTF8.find("waterway") != string::npos) {
				addonCategories << U"connectable_type";
				addonCategories << U"ship";
				addonCategories << U"waterway";
			}
			else if (categoryNameTempUTF8.find("airport") != string::npos) {
				addonCategories << U"airport";
			}
			else if (categoryNameTempUTF8.find("tile") != string::npos) {
				addonCategories << U"put_type";
				addonCategories << U"tile";
			}
			
			if (categoryNameTempUTF8.find("low_density") != string::npos) {
				addonCategories << U"low_density";
			}
			else if (categoryNameTempUTF8.find("high_density") != string::npos) {
				addonCategories << U"high_density";
			}
			
			if (categoryNameTempUTF8.find("two_lane") != string::npos) {
				addonCategories << U"two_lane";
			}
			
			categories[0] = true;
		}
		
		String categoryNameTemp2;
		getElement(strTemp, U"addon_type_2", categoryNameTemp2);
		
		if (categoryNameTemp2.length() > 0 && !categories[1]) {
			string categoryNameTemp2UTF8 = categoryNameTemp2.toUTF8();
			
			transform(categoryNameTemp2UTF8.begin(), categoryNameTemp2UTF8.end(), categoryNameTemp2UTF8.begin(), ::tolower);
			
			if (categoryNameTemp2UTF8.find("city_hall") != string::npos) {
				addonCategories << U"city_hall";
			}
			else if (categoryNameTemp2UTF8.find("education") != string::npos) {
				addonCategories << U"education";
			}
			else if (categoryNameTemp2UTF8.find("post_office") != string::npos) {
				addonCategories << U"post_office";
			}
			else if (categoryNameTemp2UTF8.find("education") != string::npos) {
				addonCategories << U"education";
			}
			else if (categoryNameTemp2UTF8.find("fire_depertment") != string::npos) {
				addonCategories << U"fire_depertment";
			}
			else if (categoryNameTemp2UTF8.find("police_station") != string::npos) {
				addonCategories << U"police";
				addonCategories << U"police_station";
			}
			
			categories[1] = true;
		}
		
		String categoryNameTemp3;
		getElement(strTemp, U"addon_type_3", categoryNameTemp3);
		
		if (categoryNameTemp3.length() > 0 && !categories[2]) {
			string categoryNameTemp3UTF8 = categoryNameTemp3.toUTF8();
			
			transform(categoryNameTemp3UTF8.begin(), categoryNameTemp3UTF8.end(), categoryNameTemp3UTF8.begin(), ::tolower);
			
			if (categoryNameTemp3UTF8.find("elementary-school") != string::npos) {
				addonCategories << U"elementary_school";
			}
			else if (categoryNameTemp3UTF8.find("junior-high-school") != string::npos) {
				addonCategories << U"junior_high_school";
			}
			else if (categoryNameTemp3UTF8.find("high-school") != string::npos) {
				addonCategories << U"high_school";
			}
			else if (categoryNameTemp3UTF8.find("university") != string::npos) {
				addonCategories << U"university";
			}
			
			categories[2] = true;
		}
		
		// 最大収容人数
		getElement(strTemp, U"maximum_capacity", maximumCapacity);
		
		// 建物の効果
		getElement(strTemp, U"land_price_influence", effects[U"land_price"].influence);
		getElement(strTemp, U"land_price_influence_grid", effects[U"land_price"].grid);
		
		getElement(strTemp, U"education_rate_influence", effects[U"education_rate"].influence);
		getElement(strTemp, U"education_rate_influence_grid", effects[U"education_rate"].grid);
		
		getElement(strTemp, U"crime_rate_influence", effects[U"crime_rate"].influence);
		getElement(strTemp, U"crime_rate_influence_grid", effects[U"crime_rate"].grid);
		
		getElement(strTemp, U"noise_influence", effects[U"noise"].influence);
		getElement(strTemp, U"noise_influence_grid", effects[U"noise"].grid);
		
		// 使用するtype
		getTypes(strTemp, U"use_types", useTypes);
		
		// 各typeの内容を取得
		// 現在読込中のtypeを取得
		for (int i=0; i<useTypes.size(); i++) {
			if (strTempUTF8.find(useTypes[i].toUTF8() + " {") == 0 && !loadingType) {
				if (useTypes[i] == U"null") {
					useTypes[i] = U"normal";
				}
				
				currentLoadingType = useTypes[i];
				loadingType = true;
			}
		}
		if (strTempUTF8.find("}") == 0 && !loadingDirection) {
			// typeが切り替わるときにTextureの設定
			if (types[currentLoadingType].image.length() > 0) {
				Image iTemp(Unicode::Widen(addonFilePath.folder_path)+U"/"+types[currentLoadingType].image);
				setAlphaColor(iTemp, transparentColor);
<<<<<<< HEAD
				types[currentLoadingType].layers << AddonLayerStruct{Texture(iTemp), LayerType::Normal};
=======
				blendColorAndImage(iTemp, Color(0, 0, 0, 200));
				types[currentLoadingType].texture = Texture(iTemp);
>>>>>>> mac_develop
			}
			
			currentDirection = U"";
			loadingType = false;
		}
		
		if (currentLoadingType != U"") {
			// 画像のパス
			getElement(strTemp, U"image", types[currentLoadingType].image);
			
			// 透過色
			String RGBStr;
			getElement(strTemp, U"transparent_color", RGBStr);
			
			if (RGBStr.length() > 0) {
				Array<String> RGBStrV = split(RGBStr, U", ");
				
				if (RGBStrV.size() == 3) {
					transparentColor.r = stoi(RGBStrV[0].toUTF8());
					transparentColor.g = stoi(RGBStrV[1].toUTF8());
					transparentColor.b = stoi(RGBStrV[2].toUTF8());
				}
			}
			
			// ナイトマスク画像のパス
			getElement(strTemp, U"night_mask", types[currentLoadingType].nightMask);
			
			// typeに含まれる方向と各方向の情報を取得
			getTypes(strTemp, U"direction", types[currentLoadingType].directionNames);
			
			for (int i=0; i<types[currentLoadingType].directionNames.size(); i++) {
				if (strTempUTF8.find(types[currentLoadingType].directionNames[i].toUTF8()+" {") != string::npos && !loadingDirection) {
					if (types[currentLoadingType].directionNames[i] == U"null") {
						types[currentLoadingType].directionNames[i] = U"normal";
					}
					currentDirection = types[currentLoadingType].directionNames[i];
				}
			}
			if (strTempUTF8.find("}") != string::npos && loadingDirection) {
				currentDirection = U"";
				loadingDirection = false;
			}
			
			if (currentDirection != U"") {
				AddonDirectionStruct direction_temp;
				
				// アドオンの大きさ
				getElement(strTemp, U"size_x", direction_temp.sizeWidth);		// 横
				getElement(strTemp, U"size_y", direction_temp.sizeHeight);		// 縦
				
				// アドオンが占めるマスの数
				getElement(strTemp, U"chip_x", direction_temp.chipX);			// 横
				getElement(strTemp, U"chip_y", direction_temp.chipY);			// 縦
				
				// 画像上の左上の座標
				getElement(strTemp, U"top_left_x", direction_temp.topLeftX);
				getElement(strTemp, U"top_left_y", direction_temp.topLeftY);
				
				// 画面上の右下の座標
				getElement(strTemp, U"bottom_right_x", direction_temp.bottomRightX);
				getElement(strTemp, U"bottom_right_y", direction_temp.bottomRightY);
				
				types[currentLoadingType].directions[currentDirection] = direction_temp;
			}
		}
	}
	
	if (!belong) {
		belongAddonsSetName << U"Normal";
	}
	
	converter();
	
	return true;
}

bool Addon::loadADJ(FileStruct newFilePath, String loading_addons_set_name) {
	addonFilePath = newFilePath;
	JSONReader addonData(Unicode::Widen(addonFilePath.file_path));
	
	belongAddonsSetName = addonData[U"Belong_addon_set_name"].getArray<String>();
	bool belong = false;
	for (int i=0; i<belongAddonsSetName.size(); i++) {
		if (belongAddonsSetName[i].length() > 0) {
			if (belongAddonsSetName[i] == loading_addons_set_name) {
				belong = true;
			}
		}
	}
	if (!belong) {
		return false;
	}
	
	addonName = addonData[U"name"].getString();
	addonJPName = addonData[U"jp_name"].getString();
	
	addonAuthor = addonData[U"author"].getString();
	addonSummary = addonData[U"summary"].getString();
	
	addonIcon = addonData[U"icon"].getString();
	
	// アイコンを読み込み
	Image iconImage(Unicode::Widen(addonFilePath.folder_path)+U"/"+addonIcon);
	setAlphaColor(iconImage, Color(0, 0, 0));
	iconTexture = Texture(iconImage);
	
	// カテゴリ
	addonCategories = addonData[U"Categories"].getArray<String>();
	
	// 建物の効果
	for (const auto& effect : addonData[U"effects"].objectView()) {
		effects[effect.name].influence = effect.value[U"influence"].get<int>();
		effects[effect.name].grid = effect.value[U"grid"].get<int>();
	}
	
	useTypes = addonData[U"Use_types"].getArray<String>();
	
	for (const auto& type : addonData[U"Types"].arrayView()) {
		String typeName = type[U"type_name"].getString();
		
		types[typeName].image = type[U"image"].getString();
		
		types[typeName].transparentColor.r = type[U"transparent_color.R"].get<int>();
		types[typeName].transparentColor.g = type[U"transparent_color.G"].get<int>();
		types[typeName].transparentColor.b = type[U"transparent_color.B"].get<int>();
		
		Image iTemp(Unicode::Widen(addonFilePath.folder_path)+U"/"+types[typeName].image);
		setAlphaColor(iTemp, Color(types[typeName].transparentColor.r, types[typeName].transparentColor.g, types[typeName].transparentColor.b));
<<<<<<< HEAD
		types[typeName].layers << AddonLayerStruct{Texture(iTemp), LayerType::Normal};
=======
		blendColorAndImage(iTemp, Color(0, 0, 0, 200));
		types[typeName].texture = Texture(iTemp);
>>>>>>> mac_develop
		
		types[typeName].nightMask = type[U"night_mask"].getString();
		if (FileSystem::IsFile(Unicode::Widen(addonFilePath.folder_path)+U"/"+types[typeName].nightMask)) {
			Image iTempNM(Unicode::Widen(addonFilePath.folder_path)+U"/"+types[typeName].nightMask);
			setAlphaColor(iTempNM, Color(types[typeName].transparentColor.r, types[typeName].transparentColor.g, types[typeName].transparentColor.b));
			types[typeName].nightMaskTexture = Texture(iTempNM);
		}
		
		types[typeName].directionNames = type[U"direction_names"].getArray<String>();
		
		for (const auto& direction : type[U"Directions"].arrayView()) {
			String direction_name = direction[U"direction_name"].getString();
			
			types[typeName].directions[direction_name].sizeWidth = direction[U"size.width"].get<int>();
			types[typeName].directions[direction_name].sizeHeight = direction[U"size.height"].get<int>();
			
			types[typeName].directions[direction_name].chipX = direction[U"squares.width"].get<int>();
			types[typeName].directions[direction_name].chipY = direction[U"squares.height"].get<int>();
			
			types[typeName].directions[direction_name].topLeftX = direction[U"top_left.x"].get<int>();
			types[typeName].directions[direction_name].topLeftY = direction[U"top_left.y"].get<int>();
			
			types[typeName].directions[direction_name].bottomRightX = direction[U"bottom_right.x"].get<int>();
			types[typeName].directions[direction_name].bottomRightY = direction[U"bottom_right.y"].get<int>();
		}
	}
	
	return true;
}

String Addon::getName() {
	return addonName;
}

String Addon::getNameJP() {
	return addonJPName;
}

String Addon::getAuthorName() {
	return addonAuthor;
}

String Addon::getSummary() {
	return addonSummary;
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
	return useTypes[typeNum];
}

String Addon::getDirectionName(int typeNum, int directionNum) {
	return types[getTypeName(typeNum)].directionNames[directionNum];
}

String Addon::getDirectionName(String typeName, int directionNum) {
	return types[typeName].directionNames[directionNum];
}

Array<String> Addon::getCategories() {
	return addonCategories;
}

bool Addon::isInCategories(String searchCategory) {
	for (int i=0; i<addonCategories.size(); i++) {
		if (addonCategories[i] == searchCategory) {
			return true;
		}
	}
	return false;
}

bool Addon::isInCategories(Array<String> searchCategories) {
	for (int i=0; i<addonCategories.size(); i++) {
		for (int j=0; j<searchCategories.size(); j++) {
			if (addonCategories[i] == searchCategories[j]) {
				return true;
			}
		}
	}
	return false;
}

map<String, EffectStruct> Addon::getEffects() {
	return effects;
}

void Addon::drawIcon(PositionStruct position, PositionStruct leftTop, SizeStruct size) {
	iconTexture(leftTop.x, leftTop.y, size.width, size.height).draw(position.x, position.y);
}

CoordinateStruct Addon::getUseTiles(String typeName, String directionName) {
	return CoordinateStruct{types[typeName].directions[directionName].chipX, types[typeName].directions[directionName].chipY};
}

PositionStruct Addon::getPosition(String typeName, String directionName, PositionStruct position, CoordinateStruct useTiles, CoordinateStruct tilesCount) {
	AddonDirectionStruct* directionTemp = &(types[typeName].directions[directionName]);
	if (directionTemp != nullptr)
		position.y = position.y + CHIP_SIZE/2 - directionTemp->sizeHeight + CHIP_SIZE/4 * (max(1, useTiles.x) - 1 - tilesCount.x) + CHIP_SIZE*3/4 * tilesCount.y;
	
	return position;
}

void Addon::draw(String typeName, String directionName, PositionStruct position, CoordinateStruct useTiles, CoordinateStruct tilesCount, Color* addColor) {
	AddonDirectionStruct* directionTemp = &(types[typeName].directions[directionName]);
	
	/*
	if (cursor.coordinate.x == coordinate.x && cursor.coordinate.y == coordinate.y) {
		cout << coordinate.x << "," << coordinate.y << " :  " << tiles_count.x << "," << tiles_count.y << endl;
	}*/
	
	//position.x = position.x + tiles_count.x * CHIP_SIZE/8;
	position = getPosition(typeName, directionName, position, useTiles, tilesCount);
	
	unsigned short int topLeftX = directionTemp->topLeftX;
	topLeftX += CHIP_SIZE/2 * tilesCount.x + CHIP_SIZE/2 * tilesCount.y;
	
	unsigned short int topLeftY = directionTemp->topLeftY;
	topLeftY += CHIP_SIZE/2 * tilesCount.y;
	
	unsigned short int sizeWidth = directionTemp->sizeWidth;
	sizeWidth = CHIP_SIZE;
	
	unsigned short int sizeHeight = directionTemp->sizeHeight;
	
	if (addColor->a > 0) {
<<<<<<< HEAD
		types[typeName].layers[0].texture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y, *addColor);
	}
	else {
		types[typeName].layers[0].texture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y);
=======
		types[typeName].texture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y, *addColor);
		if (!types[typeName].nightMaskTexture.isEmpty()) {
			types[typeName].nightMaskTexture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y, *addColor);
		}
	}
	else {
		types[typeName].texture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y);
		if (!types[typeName].nightMaskTexture.isEmpty()) {
			types[typeName].nightMaskTexture(topLeftX, topLeftY, sizeWidth, sizeHeight).draw(position.x, position.y);
		}
>>>>>>> mac_develop
	}
}

void Addon::converter() {
	JSONWriter addonData;
	
	addonData.startObject();
	{
		addonData.key(U"name").write(addonName);
		addonData.key(U"jp_name").write(addonJPName);
		
		addonData.key(U"author").write(addonAuthor);
		addonData.key(U"summary").write(addonSummary);
		
		addonData.key(U"version").write(RELEASE_NUMBER);
		
		addonData.key(U"Belong_addon_set_name").startArray();
		{
			for (auto belong = belongAddonsSetName.begin(); belong!= belongAddonsSetName.end() ; belong++) {
				addonData.write(*belong);
			}
		}
		addonData.endArray();
		
		addonData.key(U"icon").write(addonIcon);
		
		addonData.key(U"Categories").startArray();
		{
			for (auto categoryName = addonCategories.begin(); categoryName != addonCategories.end(); categoryName++) {
				addonData.write(*categoryName);
			}
		}
		addonData.endArray();
		
		addonData.key(U"effects").startObject();
		{
			for (auto e = effects.begin(); e != effects.end() ; e++) {
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
		
		addonData.key(U"maximum_capacity").write(maximumCapacity);
		
		addonData.key(U"Use_types").startArray();
		{
			for (auto typeName = useTypes.begin(); typeName != useTypes.end() ; typeName++) {
				addonData.write(*typeName);
			}
		}
		addonData.endArray();
		
		addonData.key(U"Types").startArray();
		{
			for (auto type = types.begin(); type != types.end(); type++) {
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
									addonData.key(U"width").write(direction->second.sizeWidth);
									addonData.key(U"height").write(direction->second.sizeHeight);
								}
								addonData.endObject();
								addonData.key(U"squares").startObject();
								{
									addonData.key(U"width").write(direction->second.chipX);
									addonData.key(U"height").write(direction->second.chipY);
								}
								addonData.endObject();
								addonData.key(U"top_left").startObject();
								{
									addonData.key(U"x").write(direction->second.topLeftX);
									addonData.key(U"y").write(direction->second.topLeftY);
								}
								addonData.endObject();
								addonData.key(U"bottom_right").startObject();
								{
									addonData.key(U"x").write(direction->second.bottomRightX);
									addonData.key(U"y").write(direction->second.bottomRightY);
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
	
	addonData.save(FileSystem::ParentPath(Unicode::Widen(addonFilePath.file_path))+FileSystem::BaseName(Unicode::Widen(addonFilePath.file_path))+U".adj");
}
