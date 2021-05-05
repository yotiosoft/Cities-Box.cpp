//
//  CityMap.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "CityMap.hpp"


bool CityMap::m_get_element(String str, String searchElementName, String& ret) {
	string strUTF8 = str.toUTF8();
	string searchElementNameUTF8 = searchElementName.toUTF8();
	
	if (strUTF8.find(searchElementNameUTF8) != string::npos && strUTF8.find("=") != string::npos) {
		ret = Unicode::Widen(strUTF8.substr(strUTF8.find("\"") + 1, strUTF8.find(";") - (strUTF8.find("\"") + 2)));
		return true;
	}
	return false;
}

bool CityMap::m_get_element(String str, String searchElementName, int& ret) {
	string strUTF8 = str.toUTF8();
	string searchElementNameUTF8 = searchElementName.toUTF8();
	
	if (strUTF8.find(searchElementNameUTF8) != string::npos && strUTF8.find("=") != string::npos) {
		ret = stoi(strUTF8.substr(strUTF8.find("= ") + 2, strUTF8.find(";") - (strUTF8.find("= ") + 2)));
		return true;
	}
	return false;
}

bool CityMap::m_get_element(String str, String searchElementName, bool& ret) {
	string strUTF8 = str.toUTF8();
	string searchElementNameUTF8 = searchElementName.toUTF8();
	
	if (strUTF8.find(searchElementNameUTF8) != string::npos && strUTF8.find("=") != string::npos) {
		int intTemp = stoi(strUTF8.substr(strUTF8.find("= ") + 2, strUTF8.find(";") - (strUTF8.find("= ") + 2)));
		if (intTemp == 1) {
			ret = true;
			return true;
		}
		else if (intTemp == 0) {
			ret = false;
			return true;
		}
	}
	return false;
}

bool CityMap::m_get_types(String str, String searchElementName, Array<String>& ret) {
	String aRet;
	if (m_get_element(str, searchElementName, aRet)) {
		ret = split(aRet, U", ");
		return true;
	}
	return false;
}


void CityMap::load(String loadMapFilePath) {
	/*
	if (FileSystem::Extension(loadMapFilePath) == U"cbd") {
		loadCBD(loadMapFilePath);
	}
	else */if (FileSystem::Extension(loadMapFilePath) == U"cbj") {
		loadCBJ(loadMapFilePath);
	}
}

static s3d::String extracted(int i, Array<s3d::String> &workplaceStr) {
	String workplaceAndSerial = workplaceStr[i].substr(1, workplaceStr[i].length()-1);
	return workplaceAndSerial;
}

void CityMap::loadCBJ(String loadMapFilePath) {
	m_map_file_path = loadMapFilePath;
	
	ifstream ifs(m_map_file_path.toUTF8().c_str(), ios::in | ios::binary);
	
	std::string mapXOR((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	string mapDataStr = stringXOR(mapXOR, "citiesboxmapdatafilexor");
	saveTextFile("./data/map_temp.cbj_unxor", mapDataStr);
	saveTextFile("./data/map_temp.cbj_temp", mapDataStr);
	
	JSONReader mapData(U"./data/map_temp.cbj_temp");
	//remove("./data/map_temp.cbj_temp");
	
	m_saved_version = mapData[U"Version"].get<int>();
	
	m_addon_set_name = mapData[U"Addon_Set"].getString();
	// -> アドオン読み込み
	loadAddons(m_addon_set_name);
	
	m_city_name = mapData[U"City_Name"].getString();
	
	m_mayor_name = mapData[U"Mayor_Name"].getString();
	
	m_total_population = mapData[U"Total_Population"].get<int>();
	
	m_change_weather = mapData[U"Change_Weather"].get<bool>();
	
	m_temperature = mapData[U"Temperature"].get<int>();
	
	m_dark_on_night = mapData[U"Dark_on_Night"].get<bool>();
	
	m_map_size.x = mapData[U"Map_size.width"].get<int>();
	m_map_size.y = mapData[U"Map_size.height"].get<int>();
	
	m_time_now.year = mapData[U"Time.year"].get<int>();
	m_time_now.month = mapData[U"Time.month"].get<int>();
	m_time_now.date = mapData[U"Time.date"].get<int>();
	m_time_now.hour = mapData[U"Time.hour"].get<int>();
	m_time_now.minutes = mapData[U"Time.minutes"].get<int>();
	
	m_demand.residential = mapData[U"Demand.residential"].get<int>();
	m_demand.commercial = mapData[U"Demand.commercial"].get<int>();
	m_demand.office = mapData[U"Demand.office"].get<int>();
	m_demand.industrial = mapData[U"Demand.industrial"].get<int>();
	m_demand.farm = mapData[U"Demand.farm"].get<int>();
	
	m_money = mapData[U"Money"].get<int>();
	
	m_budget.police = mapData[U"Budget.police"].get<int>();
	m_budget.fireDepertment = mapData[U"Budget.fire_depertment"].get<int>();
	m_budget.postOffice = mapData[U"Budget.post_office"].get<int>();
	m_budget.education = mapData[U"Budget.education"].get<int>();
	
	m_tax.residential = mapData[U"Tax.residential"].get<int>();
	m_tax.commercial = mapData[U"Tax.commercial"].get<int>();
	m_tax.office = mapData[U"Tax.office"].get<int>();
	m_tax.industrial = mapData[U"Tax.industrial"].get<int>();
	m_tax.farm = mapData[U"Tax.farm"].get<int>();
	
	// オブジェクトの読み込み(r142以降)
	if (m_saved_version >= 142) {
		m_max_object_id = 0;
		for (const auto& object : mapData[U"Objects"].arrayView()) {
			// オブジェクトID
			int object_id = object[U"objectID"].get<int>();
			
			// アドオン名
			String addon_name = object[U"addon_name"].getString();
			
			// 固有名称
			String original_name = object[U"original_name"].getString();
			
			// TypeID
			TypeID::Type type_id = typeNameToTypeID(object[U"typeID"].getString());
			
			// DirectionID
			DirectionID::Type direction_id = directionNameToDirectionID(object[U"directionID"].getString());
			
			// 原点
			CoordinateStruct origin_coordinate;
			origin_coordinate.x = object[U"origin_coordinate.x"].get<int>();
			origin_coordinate.y = object[U"origin_coordinate.y"].get<int>();
			
			// オブジェクトをリストに登録
			m_objects[object_id] = Object(object_id, m_addons[addon_name], original_name, type_id, direction_id, origin_coordinate);
			
			if (object_id > m_max_object_id) {
				m_max_object_id = object_id;
			}
		}
	}
	
	int y = 0;
	for (const auto& mapTiles : mapData[U"Map"].arrayView()) {
		m_tiles.push_back(Array<Tile>());
		int x = 0;
		for (const auto& tile : mapTiles.arrayView()) {
			m_tiles[y].push_back(Tile());
			
			CoordinateStruct tiles_count;
			tiles_count.x = tile[U"tiles_count.x"].get<int>();
			tiles_count.y = tile[U"tiles_count.y"].get<int>();
			
			if (m_saved_version <= 141) {			// r141以前なら内容を修正
				m_max_object_id = 0;
				
				int serial_number = tile[U"serial_number"].get<int>();
				int object_count = 0;
				for (const auto& jAddons : tile[U"addons"].arrayView()) {
					TypeID::Type type_id = typeNameToTypeID(jAddons[U"type_number"].getString());
					DirectionID::Type direction_id = directionNameToDirectionID(jAddons[U"direction_number"].getString());
					
					if (direction_id != DirectionID::West) {
						String addon_name = jAddons[U"name"].getString();
						if (m_addons[addon_name]->getUseTiles(type_id, direction_id).y > 1) {
							tiles_count.y = m_addons[addon_name]->getUseTiles(type_id, direction_id).y - 1 - tiles_count.y;
						}
					}
					
					//tiles[y][x].category.push_back(j_addons[U"category"].getString());
					m_tiles[y][x].addType(type_id);
					m_tiles[y][x].addDirection(direction_id);
					
					// アドオンのポインタを登録
					if (m_addons.find(jAddons[U"name"].getString()) != m_addons.end()) {
						m_tiles[y][x].addons.push_back(m_addons[jAddons[U"name"].getString()]);
						
						// 0x0の位置でオブジェクトを生成しオブジェクトリストに追加
						if (tiles_count.x == 0 && tiles_count.y == 0) {
							// old_version_serial_num = 0なら空き番号に振り直す
							// あるいはObjectIDが被った場合に振り直す
							if (serial_number == 0 || m_objects.count(serial_number) > 0) {
								serial_number = m_max_object_id + 1;
								m_max_object_id ++;
							}
							
							// オブジェクトをリストに登録
							m_objects[serial_number] = Object(serial_number, m_addons[jAddons[U"name"].getString()], tile[U"original_name"].getString(), type_id, direction_id, CoordinateStruct{x, y});
						}
						else {
							CoordinateStruct origin_coordinate;
							origin_coordinate.x = x - tiles_count.x;
							origin_coordinate.y = y - tiles_count.y;
							
							// 原点とObjectIDが一致しない -> ObjectIDを原点のものに修正
							if (m_objects[serial_number].getOriginCoordinate().x != origin_coordinate.x ||
								m_objects[serial_number].getOriginCoordinate().y != origin_coordinate.y) {
								
								cout << "at " << x << "," << y << " from " << origin_coordinate.x << "," << origin_coordinate.y << ":" << serial_number << " to " << m_tiles[origin_coordinate.y][origin_coordinate.x].getObjectP(jAddons[U"name"].getString(), NameMode::English)->getObjectID() << endl;
								
								serial_number = m_tiles[origin_coordinate.y][origin_coordinate.x].getObjectP(jAddons[U"name"].getString(), NameMode::English)->getObjectID();
							}
						}
						
						// RelativeCoordinateStructを作成
						RelativeCoordinateStruct relarive_coordinate;
						relarive_coordinate.origin = m_objects[serial_number].getOriginCoordinate();
						relarive_coordinate.relative = tiles_count;
						
						// オブジェクトをタイルに格納
						m_tiles[y][x].addObject(&(m_objects[serial_number]), relarive_coordinate);
						
						if (serial_number > m_max_object_id) {
							m_max_object_id = serial_number;
						}
					}
					else {
						cout << "Cant't find " << jAddons[U"name"].getString() << endl;
					}
					
					//cout << m_objects.size() << endl;
					
					object_count ++;
				}
			}
			else {
				for (const auto& jObject : tile[U"objects"].arrayView()) {
					// オブジェクトIDを取得
					int object_id = jObject[U"objectID"].get<int>();
					
					// RelativeCoordinateStructを作成
					RelativeCoordinateStruct relarive_coordinate;
					relarive_coordinate.origin = m_objects[object_id].getOriginCoordinate();
					relarive_coordinate.relative.x = jObject[U"relative_coordinate.x"].get<int>();
					relarive_coordinate.relative.y = jObject[U"relative_coordinate.y"].get<int>();
					
					m_tiles[y][x].addObject(&(m_objects[object_id]), relarive_coordinate);
				}
			}
			
			m_tiles[y][x].residents = tile[U"residents"].get<int>();
			
			m_tiles[y][x].workers.commercial = tile[U"workers.commercial"].get<int>();
			m_tiles[y][x].workers.office = tile[U"workers.office"].get<int>();
			m_tiles[y][x].workers.industrial = tile[U"workers.industrial"].get<int>();
			m_tiles[y][x].workers.farm = tile[U"workers.farm"].get<int>();
			m_tiles[y][x].workers.publicFacility = tile[U"workers.public"].get<int>();
			
			m_tiles[y][x].students = tile[U"students"].get<int>();
			
			m_tiles[y][x].happinessRate = tile[U"happiness_rate"].get<int>();
			
			// 各率の読み込み
			for (const auto& rate : tile[U"rate"].objectView()) {
				m_tiles[y][x].rate[rateNameToRateID(rate.name)] = rate.value.get<int>();
			}
			
			/*
			tiles[y][x].crop.name = square[U"crop.name"].getString();
			tiles[y][x].crop.amount = square[U"crop.amount"].get<int>();
			*/
			
			m_tiles[y][x].age = tile[U"age"].getArray<int>();
			
			m_tiles[y][x].gender = tile[U"gender"].getArray<String>();
			
			for (const auto& workPlaces : tile[U"work_places"].arrayView()) {
				m_tiles[y][x].workPlaces.push_back(WorkPlaceStruct());
				
				m_tiles[y][x].workPlaces.back().workPlace = getRCOIFP(workPlaces[U"work_kind"].get<int>());
				m_tiles[y][x].workPlaces.back().workPlacesSerialNumber = workPlaces[U"serial_number"].get<int>();
			}
			
			for (const auto& schools : tile[U"school"].arrayView()) {
				m_tiles[y][x].schools.push_back(SchoolStruct());
				
				m_tiles[y][x].schools.back().school = getSchool(schools[U"school_kind"].get<int>());
				m_tiles[y][x].schools.back().schoolSerialNumber = schools[U"serial_number"].get<int>();
			}
			
			m_tiles[y][x].reservation = getRCOIFP(tile[U"reservation"].get<int>());
			
			m_tiles[y][x].setOriginalName(tile[U"original_name"].getString());
			
			x++;
		}
		y++;
	}
}

void CityMap::loadAddons(String addonSetName) {
	//Array<FileStruct> addons_path = specific::getAllFilesName("./addons", "adat");
	Array<FileStruct> addonsPath = specific::getAllFilesName("./addons", "adj");
	
	for (int i=0; i<addonsPath.size(); i++) {
		cout << "from: " << addonsPath[i].file_path << endl;
		FileStruct fileTemp = addonsPath[i];
		
		Addon* loadingAddon = new Addon();
		if (loadingAddon->load(addonsPath[i], addonSetName)) {
			m_addons[loadingAddon->getName(NameMode::English)] = loadingAddon;
		}
		else {
			delete(loadingAddon);
		}
		
		System::Sleep(20);
	}
}

Array<Addon*> CityMap::getFitAddons(Array<String> selectedCategories) {
	Array<Addon*> retAddons;
	
	for (auto addon = m_addons.begin(); addon != m_addons.end(); addon++) {
		
		Array<String> anAddonCategories = addon->second->getCategories();
		vector<bool> fit = vector<bool>(selectedCategories.size(), false);
		
		for (auto categoryName = anAddonCategories.begin(); categoryName != anAddonCategories.end(); categoryName++) {
			for (int i=0; i<selectedCategories.size(); i++) {
				if (selectedCategories[i] == *categoryName) {
					fit[i] = true;
					break;
				}
			}
		}
		
		bool fitted = true;
		for (int i=0; i<fit.size(); i++) {
			if (!fit[i]) {
				fitted = false;
			}
		}
		if (fitted) {
			retAddons << addon->second;
		}
	}
	
	return retAddons;
}

map<String, Addon*> CityMap::getAllAddons() {
	return m_addons;
}

void CityMap::draw(CameraStruct camera, CursorStruct& cursor) {
	// マップを描画
	for (short int y=getDrawArea(camera).first.y; y<getDrawArea(camera).second.y; y++) {
		for (short int x=getDrawArea(camera).first.x; x<getDrawArea(camera).second.x; x++) {
			PositionStruct drawPos = coordinateToPosition(CoordinateStruct{x, y}, camera);
			
			// 一マス分描画
			if (drawPos.x >= -CHIP_SIZE && drawPos.y >= -CHIP_SIZE/2 && drawPos.x <= Scene::Width() && drawPos.y <= Scene::Height() + CHIP_SIZE*2) {
				m_tiles[y][x].draw(m_show_rate, drawPos, m_time_now);
			}
			
			// カーソルの描画
			if (x == cursor.coordinate.x && y == cursor.coordinate.y) {
				cursor.texture->draw(cursor.position_per_tiles.x, cursor.position_per_tiles.y, Alpha(128));
			}
		}
	}
}

Array<Addon> CityMap::getAddon(CoordinateStruct coordinate) {
	Array<Addon> retAddons;
	
	for (int i=0; i<m_tiles[coordinate.y][coordinate.x].addons.size(); i++) {
		retAddons << *(m_tiles[coordinate.y][coordinate.x].addons[i]);
	}
	
	return retAddons;
}

RCOIFstruct CityMap::getDemand() {
	return m_demand;
}

int CityMap::getPopulation() {
	return m_total_population;
}

int CityMap::getMoney() {
	return m_money;
}

int CityMap::getTemperature() {
	return m_temperature;
}

Size CityMap::getMapSize() {
	return m_map_size;
}

PositionStruct CityMap::coordinateToPosition(CoordinateStruct coordinate, CameraStruct camera) {
	// カメラの座標がデフォルト値（64*mapsize.width/2-Scene::Width()/2, 0）のときの描画位置を算出
	CameraStruct defaultCamera;
	defaultCamera.position = PositionStruct{0, 0};
	defaultCamera.center = {0, 0};
	
	// 基準点となるx:0, y:0のマスの表示位置を算出する
	PositionStruct tile0x0Position = PositionStruct{defaultCamera.position.x - camera.position.x,
		defaultCamera.position.y - camera.position.y};
	
	PositionStruct tilePosition = PositionStruct{tile0x0Position.x+coordinate.x*32-coordinate.y*32, tile0x0Position.y+coordinate.y*16+coordinate.x*16};
	
	return tilePosition;
}

CoordinateStruct CityMap::positionToCoordinate(PositionStruct position, CameraStruct camera) {
	int tempMX = position.x+camera.position.x-CHIP_SIZE/2;
	int tempMY = position.y+camera.position.y;
	
	CoordinateStruct ret;
	ret.x = (tempMY + tempMX/2) / (CHIP_SIZE/2);
	ret.y = (-tempMX + tempMY*2) / CHIP_SIZE;
	
	if (ret.x < 0) {
		ret.x = 0;
	}
	if (ret.y < 0) {
		ret.y = 0;
	}
	
	if (ret.x >= m_map_size.x) {
		ret.x = m_map_size.x-1;
	}
	if (ret.y >= m_map_size.y) {
		ret.y = m_map_size.y-1;
	}
	
	return ret;
}

pair<CoordinateStruct, CoordinateStruct> CityMap::getDrawArea(CameraStruct camera) {
	// カメラの座標が変わっていなければrangeを返す
	if (camera.position.x == m_camera_before.position.x && camera.position.y == m_camera_before.position.y) {
		return m_range;
	}
	m_camera_before = camera;
	
	// 描画できる範囲
	int rangeOneDirection = (sqrt(powf(Scene::Width(), 2)+powf(Scene::Height(), 2))/sqrt(powf(CHIP_SIZE/2, 2)+powf(CHIP_SIZE/4, 2)));
	
	// 画面中央の座標
	CoordinateStruct centerCoordinate = positionToCoordinate(PositionStruct{Scene::Width()/2, Scene::Height()/2}, camera);
	
	pair<CoordinateStruct, CoordinateStruct> ret;
	// 左上の座標
	ret.first = CoordinateStruct{centerCoordinate.x-rangeOneDirection, centerCoordinate.y-rangeOneDirection};
	
	if (ret.first.x < 0) {
		ret.first.x = 0;
	}
	if (ret.first.y < 0) {
		ret.first.y = 0;
	}
	if (ret.first.x >= m_map_size.x) {
		ret.first.x = m_map_size.x-1;
	}
	if (ret.first.y >= m_map_size.y) {
		ret.first.y = m_map_size.y-1;
	}
	
	// 右下の座標
	ret.second = CoordinateStruct{centerCoordinate.x+rangeOneDirection, centerCoordinate.y+rangeOneDirection};
	
	if (ret.second.x < 0) {
		ret.second.x = 0;
	}
	if (ret.second.y < 0) {
		ret.second.y = 0;
	}
	if (ret.second.x >= m_map_size.x) {
		ret.second.x = m_map_size.x-1;
	}
	if (ret.second.y >= m_map_size.y) {
		ret.second.y = m_map_size.y-1;
	}
	
	m_range = ret;
	
	return ret;
}

// いずれかのアドオンがカテゴリに含まれているか
bool CityMap::hasCategory(String searchCategory, CoordinateStruct coordinate) {
	Tile* currentTile = &m_tiles[coordinate.y][coordinate.x];
	
	for (int i=0; i<currentTile->addons.size(); i++) {
		if (currentTile->addons[i]->isInCategories(searchCategory)) {
			return true;
		}
	}
	
	return false;
}

// アドオンの設置
bool CityMap::build(CursorStruct cursor, CursorStruct before_cursor, Addon* selectedAddon, bool needToBreak) {
	// ObjectIDの決定
	int objectID = m_get_next_objectID();
	
	TypeID::Type type;
	DirectionID::Type direction;
	
	Array<CoordinateStruct> needUpdate;
	if (getBuildTypeAndDirection(cursor.coordinate, selectedAddon, type, direction, needUpdate)) {		// 設置可能なら...
		CoordinateStruct useTiles = selectedAddon->getUseTiles(type, direction);
		
		CoordinateStruct origin_coordinate = cursor.coordinate;
		if (direction == DirectionID::West) {
			origin_coordinate.y -= useTiles.y-1;
		}
		if (direction == DirectionID::East) {
			origin_coordinate.x -= useTiles.x-1;
			origin_coordinate.y -= useTiles.y-1;
		}
		else if (direction == DirectionID::South) {
			origin_coordinate.y -= useTiles.y-1;
		}
		
		// オブジェクトの生成
		m_objects[objectID] = Object(objectID, selectedAddon, U"", type, direction, origin_coordinate);
		
		// ConnectableTypeの場合 -> カーソルが移動前の座標から連続して押し続けて移動していれば、そのタイルと接続する
		Array<CoordinateStruct> needUpdate;
		if (before_cursor.pressed && (cursor.coordinate.x != before_cursor.coordinate.x || cursor.coordinate.y != before_cursor.coordinate.y)) {
			if (selectedAddon->isInCategories(U"connectable_type")) {
				for (auto from_coordinate_object_struct : m_tiles[before_cursor.coordinate.y][before_cursor.coordinate.x].getObjectStructs()) {
					if (from_coordinate_object_struct.object_p->getAddonP()->isInCategories(U"road") && selectedAddon->isInCategories(U"road")) {
						from_coordinate_object_struct.object_p->connect(
							CoordinateStruct{0, 0},			// 暫定
							getDirectionIDfromDifference(cursor.coordinate, before_cursor.coordinate),
							&(m_objects[objectID])
						);
						
						m_objects[objectID].connect(
							CoordinateStruct{0, 0},			// 暫定
							getDirectionIDfromDifference(before_cursor.coordinate, cursor.coordinate),
							from_coordinate_object_struct.object_p
						);
						
						needUpdate << CoordinateStruct{cursor.coordinate.x - before_cursor.coordinate.x, cursor.coordinate.y - before_cursor.coordinate.y};
					}
				}
			}
		}
		
		// 建設するタイル上の既存のオブジェクトを削除
		for (int y = origin_coordinate.y; y < origin_coordinate.y + useTiles.y; y++) {
			for (int x = origin_coordinate.x; x < origin_coordinate.x + useTiles.x; x++) {
				breaking(CoordinateStruct{x, y}, true);
			}
		}
		
		// 各タイルにオブジェクトを追加
		for (int y = origin_coordinate.y; y < origin_coordinate.y + useTiles.y; y++) {
			for (int x = origin_coordinate.x; x < origin_coordinate.x + useTiles.x; x++) {
				// RelativeCoordinateStructを作成
				RelativeCoordinateStruct relative_coordinate;
				relative_coordinate.origin = origin_coordinate;
				relative_coordinate.relative.y = y - origin_coordinate.y;
				relative_coordinate.relative.x = x - origin_coordinate.x;
				
				cout << "build at " << x << "," << y << " : " << m_objects[objectID].getAddonName(NameMode::English) << " " << objectID << endl;
				m_tiles[y][x].addObject(&(m_objects[objectID]), relative_coordinate);
			}
		}
		
		// 効果を反映
		setRate(&(m_objects[objectID]), origin_coordinate, false);
		
		// (道路などで)周囲のアドオンの修正が必要な場合は修正する
		if (needUpdate.size() > 0) {
			Array<String> searchCategories = {U"road", U"train", U"waterway"};
			
			for (int i=0; i<needUpdate.size(); i++) {
				
				if (needUpdate[i].x < 0 || needUpdate[i].y < 0) {
					continue;
				}
				
				for (auto object_struct : m_tiles[needUpdate[i].y][needUpdate[i].x].getObjectStructs()) {
					if (object_struct.object_p->getAddonP()->isInCategories(searchCategories)) {
						if (!(needUpdate[i].x == -1 && needUpdate[i].y == -1)) {
							cout << "update for " << needUpdate[i].x << "," << needUpdate[i].y << " " << needUpdate.size() << endl;
							//update(needUpdate[i], object_struct, needUpdate);
						}
					}
				}
			}
		}
	}
	
	return true;
}

void CityMap::setRate(Object* arg_object, CoordinateStruct arg_origin_coordinate, bool will_be_deleted) {
	// 効果を取得
	map<RateID::Type, EffectStruct> effects = arg_object->getAddonP()->getEffects();
	
	// 中央座標を取得
	int center_x = arg_origin_coordinate.x + round(arg_object->getAddonDirectionStruct().requiredTiles.x / 2);
	int center_y = arg_origin_coordinate.y + round(arg_object->getAddonDirectionStruct().requiredTiles.y / 2);
	
	// それぞれの効果を反映
	for (auto effect_map : effects) {
		EffectStruct effect = effect_map.second;
		int effect_per_tile = effect.influence / effect.grid;
		
		for (int relative_y = -effect.grid; relative_y <= effect.grid; relative_y++) {
			int y = center_y + relative_y;
			if (y < 0 || y >= m_map_size.y) {
				continue;
			}
			
			for (int relative_x = -effect.grid; relative_x < effect.grid; relative_x++) {
				int x = center_x + relative_x;
				if (x < 0 || x >= m_map_size.x) {
					continue;
				}
				
				int rate = effect_per_tile * max(abs(effect.grid-1-relative_y), abs(effect.grid-1-relative_x));
				if (will_be_deleted) {
					rate *= -1;
				}
				m_tiles[y][x].setRate(effect_map.first, rate);
			}
		}
	}
}
/*
void CityMap::update(CoordinateStruct position, ObjectStruct* object_struct, Array<CoordinateStruct>& needUpdate) {
	Tile* currentTile = &m_tiles[position.y][position.x];
	
	// 踏切と橋の場合は更新不要
	for (int i=0; i<currentTile->getTypes().size(); i++) {
		if (currentTile->getType(i) == TypeID::UnderConstruction || currentTile->getType(i) == TypeID::Bridge) {
			return;
		}
	}
	
	TypeID::Type type;
	DirectionID::Type direction;
	if (getBuildTypeAndDirection(position, object_struct->object_p->getAddonP(), type, direction, needUpdate)) {
		currentTile->clearAddons();						// 一旦クリアしてもう一度設置
		
		currentTile->addType(type);
		currentTile->addDirection(direction);
		currentTile->addons << selectedAddon;
	}
}
*/
void CityMap::breaking(CoordinateStruct coordinate, bool isTemporaryDelete) {
	// オブジェクトの除去
	for (ObjectStruct object_struct : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
		// 効果を削除
		setRate(object_struct.object_p, object_struct.relative_coordinate.origin, true);
		
		int delete_object_id = object_struct.object_p->getObjectID();
		
		Size delete_object_required_tiles = object_struct.object_p->getAddonDirectionStruct().requiredTiles;
		for (int y = object_struct.relative_coordinate.origin.y; y < object_struct.relative_coordinate.origin.y + delete_object_required_tiles.y; y++) {
			for (int x = object_struct.relative_coordinate.origin.x; x < object_struct.relative_coordinate.origin.x + delete_object_required_tiles.x; x++) {
				m_tiles[y][x].deleteObject(delete_object_id);
				
				// 更地になったら芝生を置く
				CoordinateStruct current_coordinate = CoordinateStruct{x, y};
				if ((!isTemporaryDelete || current_coordinate.x != coordinate.x || current_coordinate.y != coordinate.y) && m_tiles[y][x].getObjectStructs().size() == 0) {
					debugLog(U"before put");
					m_put_grass(CoordinateStruct{x, y});
					debugLog(U"after put");
				}
			}
		}
		
		debugLog(U"before erase");
		m_objects.erase(delete_object_id);
		debugLog(U"after erase");
	}
	/*
	Tile* currentTile = &m_tiles[coordinate.y][coordinate.x];
	Array<Addon*> breakAddons = currentTile->addons;
	
	for (int i=0; i<breakAddons.size(); i++) {
		currentTile = &m_tiles[coordinate.y][coordinate.x];
		CoordinateStruct useTiles = breakAddons[i]->getUseTiles(currentTile->getType(i), currentTile->getDirection(i));
		
		CoordinateStruct startPoint = moveToAddonStartTile(coordinate, i);
		currentTile = &m_tiles[startPoint.y][startPoint.x];
		
		// 効果を取得
		map<RateID::Type, EffectStruct> effects = breakAddons[i]->getEffects();
		
		// 中央となる座標を取得
		int centerX = useTiles.x/2;
		int centerY = useTiles.y/2;
		
		for (int y=0; abs(y)<useTiles.y; y--) {
			for (int x=0; x<useTiles.x; x++) {
				Tile beforeBreak = m_tiles[startPoint.y+y][startPoint.x+x];
				cout << "from breaking: " << endl;
				build(CoordinateStruct{startPoint.x+x, startPoint.y+y}, m_addons[U"tile_greenfield"], false);
				
				// 効果を除去
				if (abs(x) == centerX && abs(y) == centerY) {
					for (auto effect = effects.begin(); effect != effects.end(); effect++) {
						double effectPerGrid = effect->second.influence / effect->second.grid;
						for (int ey=-effect->second.grid; ey<=effect->second.grid; ey++) {
							for (int ex=-effect->second.grid; ex<=effect->second.grid; ex++) {
								if (isPositionAvailable(CoordinateStruct{startPoint.x+x+ex, startPoint.y+y+ey})) {
									m_tiles[startPoint.y+y+ey][startPoint.x+x+ex].rate[effect->first] -= effectPerGrid*max(abs(effect->second.grid-1-ey), abs(effect->second.grid-1-ex));
								}
							}
						}
					}
				}
				
				if (beforeBreak.addons[i]->isInCategories(U"connectable_type")) {
					Array<CoordinateStruct> needUpdate;
					
					for (int j=0; j<AROUND_TILES; j++) {
						for (int k=0; k<m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons.size(); k++) {
							if ((beforeBreak.addons[i]->isInCategories(U"road") && m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"road")) ||
								(beforeBreak.addons[i]->isInCategories(U"train") && m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"train")) ||
								(beforeBreak.addons[i]->isInCategories(U"waterway") && m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"waterway")) ||
								(beforeBreak.addons[i]->isInCategories(U"airport") && m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons[k]->isInCategories(U"airport"))) {
								needUpdate << CoordinateStruct{startPoint.x+x+AroundTiles[j].second.x, startPoint.y+y+AroundTiles[j].second.y};
							}
						}
					}
					
					if (breakAddons[i]->isInCategories(U"road")) {
						for (int j=0; j<needUpdate.size(); j++) {
							if (m_tiles[needUpdate[j].y][needUpdate[j].x].addons[i]->isInCategories(U"railroad")) {
								needUpdate[j] = {-1, -1};
							}
						}
					}
					
					for (int j=0; j<needUpdate.size(); j++) {
						for (int k=0; k<m_tiles[startPoint.y+y+AroundTiles[j].second.y][startPoint.x+x+AroundTiles[j].second.x].addons.size(); k++) {
							if (!(needUpdate[j].x == -1 && needUpdate[j].y == -1)) {
								cout << "breaking update: " << needUpdate[j].x << "," << needUpdate[j].y << endl;
								update(needUpdate[j], m_tiles[needUpdate[j].y][needUpdate[j].x].addons[k], needUpdate);
							}
						}
					}
					
				}
			}
		}
	}*/
}

CoordinateStruct CityMap::moveToAddonStartTile(CoordinateStruct searchCoordinate, int addonNumber) {
	Tile* searchTile = &m_tiles[searchCoordinate.y][searchCoordinate.x];
	
	searchCoordinate.x -= searchTile->tilesCount.x;
	searchCoordinate.y += searchTile->tilesCount.y;
	
	return CoordinateStruct{searchCoordinate.x, searchCoordinate.y};
}

// 設置する場所に合うTypeとDirectionを取得
pair<TypeID::Type, DirectionID::Type> CityMap::setConnectableTypeProfile(Addon* selectedAddon, CoordinateStruct coordinate, CoordinateStruct before_coordinate) {
	TypeID::Type retType;
	DirectionID::Type retDirection;
	
	return pair<TypeID::Type, DirectionID::Type>{retType, retDirection};
}

bool CityMap::getBuildTypeAndDirection(CoordinateStruct coordinate, Addon* selectedAddon, TypeID::Type& retType, DirectionID::Type& retDirection,
									   Array<CoordinateStruct>& needUpdate) {
	// 接続タイプ（道路など）アドオンの場合
	if (selectedAddon->isInCategories(U"connectable_type")) {
		// 周囲に道路があるか（建設可能か）確認する
		int totalAroundRoad = 0;
		Array<pair<DirectionID::Type, CoordinateStruct>> aroundRoadCoordinate;
		
		bool needUpdateMore = true;
		if (needUpdate.size() > 0) {
			needUpdateMore = false;
		}
		
		for (int i=0; i<AROUND_TILES; i++) {
			CoordinateStruct currentTile = {coordinate.x+AroundTiles[i].second.x, coordinate.y+AroundTiles[i].second.y};
			
			if (currentTile.x < 0 || currentTile.y < 0 || currentTile.x >= m_map_size.x || currentTile.y >= m_map_size.y) {
				continue;
			}
			
			for (ObjectStruct object_struct : m_tiles[currentTile.y][currentTile.x].getObjectStructs()) {
				// 道路の場合
				if ((selectedAddon->isInCategories(U"road") && object_struct.object_p->getAddonP()->isInCategories(U"road")) ||
					(selectedAddon->isInCategories(U"train") && object_struct.object_p->getAddonP()->isInCategories(U"train")) ||
					(selectedAddon->isInCategories(U"waterway") && object_struct.object_p->getAddonP()->isInCategories(U"waterway")) ||
					(selectedAddon->isInCategories(U"airport") && object_struct.object_p->getAddonP()->isInCategories(U"airport"))) {
					totalAroundRoad ++;
					aroundRoadCoordinate << AroundTiles[i];
					
					if (needUpdateMore) {
						needUpdate << currentTile;
					}
					
					break;
				}
			}
		}
		
		// 踏切を設置する必要がある場合
		if (selectedAddon->isInCategories(U"road")) {
			for (ObjectStruct object_struct : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
				if (object_struct.object_p->getAddonP()->isInCategories(U"railroad")) {
					retType = TypeID::TrainCrossing;
					
					if (object_struct.object_p->getDirectionID() == DirectionID::NorthSouth) {
						retDirection = DirectionID::NorthSouth;
					}
					else {
						retDirection = DirectionID::EastWest;
					}
					
					for (int j=0; j<needUpdate.size(); j++) {
						if (object_struct.object_p->getAddonP()->isInCategories(U"railroad")) {
							needUpdate[j] = {-1, -1};
						}
					}
					
					return true;
				}
			}
		}
		
		// 橋を設置する必要がある場合
		if (selectedAddon->isInCategories(U"road")) {
			for (ObjectStruct object_struct : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
				if (object_struct.object_p->getAddonP()->isInCategories(U"waterway")) {
					retType = TypeID::Bridge;
					
					if (object_struct.object_p->getDirectionID() == DirectionID::NorthSouth) {
						retDirection = DirectionID::EastWest;
					}
					else {
						retDirection = DirectionID::NorthSouth;
					}
					
					for (int j=0; j<needUpdate.size(); j++) {
						if (object_struct.object_p->getAddonP()->isInCategories(U"waterway")) {
							needUpdate[j] = {-1, -1};
						}
					}
					
					return true;
				}
			}
		}
		
		// その他
		if (totalAroundRoad == 0) {
			retType = TypeID::IntersectionCross;
			retDirection = DirectionID::None;
			return true;
		}
		if (totalAroundRoad == 1) {
			retType = TypeID::DeadEnd;
			retDirection = aroundRoadCoordinate[0].first;
			return true;
		}
		if (totalAroundRoad == 2) {
			CoordinateStruct roadDelta = {0, 0};
			
			for (int i=0; i<2; i++) {
				roadDelta.x += aroundRoadCoordinate[i].second.x;
				roadDelta.y += aroundRoadCoordinate[i].second.y;
			}
			
			// 縦横方向
			if (roadDelta.x == 0 && roadDelta.y == 0) {
				retType = TypeID::Default;
				
				if (aroundRoadCoordinate[0].second.x != 0) {
					retDirection = DirectionID::EastWest;
				}
				else {
					retDirection = DirectionID::NorthSouth;
				}
				
				return true;
			}
			// 曲がり角
			else {
				retType = TypeID::Turn;
				
				// DirectionIDを取得して設定
				int direction_id_temp = 0;
				for(int i=0; i<aroundRoadCoordinate.size(); i++) {
					direction_id_temp += (int)aroundRoadCoordinate[i].first;
				}
				retDirection = (DirectionID::Type)direction_id_temp;
				
				return true;
			}
		}
		if (totalAroundRoad == 3) {
			retType = TypeID::IntersectionT;
			
			// DirectionIDを取得して設定
			int direction_id_temp = 0;
			for(int i=0; i<aroundRoadCoordinate.size(); i++) {
				direction_id_temp += (int)aroundRoadCoordinate[i].first;
			}
			retDirection = (DirectionID::Type)direction_id_temp;
			
			return true;
		}
		if (totalAroundRoad == 4) {
			retType = TypeID::IntersectionCross;
			retDirection = DirectionID::None;
			return true;
		}
	}
	
	// オブジェクトタイプの場合
	if (selectedAddon->isInCategories(U"object_type")) {
		// 周囲に道路があるか確認する
		for (int i=0; i<AROUND_TILES; i++) {
			CoordinateStruct currentTile = {coordinate.x+AroundTiles[i].second.x, coordinate.y+AroundTiles[i].second.y};
			
			for (ObjectStruct object_struct : m_tiles[currentTile.y][currentTile.x].getObjectStructs()) {
				if (object_struct.object_p->getAddonP()->isInCategories(U"road")) {
					TypeID::Type retTypeTemp = TypeID::Normal;
					DirectionID::Type retDirectionTemp = AroundTiles[i].first;
					
					// 複数のタイルを使う場合、建てる方向に障害物などがないか確認する
					bool cannotBuild = false;
					
					int addX = 1, addY = 1;
					if (retDirectionTemp == DirectionID::East) {
						addX = -1;
					}
					else if (retDirectionTemp == DirectionID::South) {
						addY = -1;
					}
					
					for (int y=0; y<selectedAddon->getUseTiles(retType, retDirection).y; y+=addY) {
						for (int x=0; y<selectedAddon->getUseTiles(retType, retDirection).x; x+=addX) {
							for (ObjectStruct object_struct_sub : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
								if (object_struct_sub.object_p->getAddonP()->isInCategories(U"connectable_type")) {
									cannotBuild = true;
									break;
								}
							}
							if (cannotBuild) break;
						}
						if (cannotBuild) break;
					}
					
					if (!cannotBuild) {
						retType = retTypeTemp;
						retDirection = retDirectionTemp;
						return true;
					}
				}
			}
		}
	}
	
	// タイルの場合
	if (selectedAddon->isInCategories(U"put_type")) {
		retType = TypeID::Normal;
		retDirection = DirectionID::None;
		return true;
	}
	cout << "return false" << endl;
	return false;					// 存在しない or 設置不可能な場合
}

// アドオンを削除
void CityMap::clear(CoordinateStruct position) {
	Tile* currentTile = &m_tiles[position.y][position.x];
	Addon* selectedAddon = m_addons[U"tile_greenfield"];
	
	currentTile->clearAll();
	
	currentTile->addType(TypeID::Normal);
	currentTile->addDirection(DirectionID::None);
	currentTile->addons << selectedAddon;
	
	// 幸福度を戻す
}

bool CityMap::isPositionAvailable(CoordinateStruct coordinate) {
	if (coordinate.x >= 0 && coordinate.x <= m_map_size.x-1 && coordinate.y >= 0 && coordinate.y <= m_map_size.y-1) {
		return true;
	}
	else {
		return false;
	}
}

TimeStruct CityMap::cityTime(int minutesDelta) {
	m_time_now.minutes += minutesDelta;
	
	if (m_time_now.minutes >= 60) {
		m_time_now.hour ++;
		m_time_now.minutes -= 60;
		
		if (m_time_now.hour >= 24) {
			m_time_now.date ++;
			m_time_now.hour -= 24;
			
			if (m_time_now.month == 4 || m_time_now.month == 6 || m_time_now.month == 9 || m_time_now.month == 11) {
				if (m_time_now.date >= 31) {
					m_time_now.month ++;
					m_time_now.date -= 30;
				}
			}
			else if (m_time_now.month == 2) {
				if (m_time_now.year % 4 == 0 && m_time_now.date >= 30) {
					m_time_now.month ++;
					m_time_now.date -= 29;
				}
				else if (m_time_now.date >= 29) {
					m_time_now.month ++;
					m_time_now.date -= 28;
				}
			}
			else if (m_time_now.date >= 32) {
				m_time_now.month ++;
				m_time_now.date -= 31;
				
				if (m_time_now.month >= 13) {
					m_time_now.year ++;
					m_time_now.month -= 12;
				}
			}
		}
	}
	
	
	return m_time_now;
}

void CityMap::setShowRate(RateID::Type rate_id) {
	m_show_rate = rate_id;
}

bool CityMap::save() {
	// JsonWriterを宣言
	JSONWriter mapData;
	
	mapData.startObject();
	{
		mapData.key(U"Version").write(RELEASE_NUMBER);
		mapData.key(U"Addon_Set").write(m_addon_set_name);
		mapData.key(U"City_Name").write(m_city_name);
		mapData.key(U"Mayor_Name").write(m_mayor_name);
		mapData.key(U"Total_Population").write(m_total_population);
		mapData.key(U"Change_Weather").write(m_change_weather);
		mapData.key(U"Temperature").write(m_temperature);
		mapData.key(U"Dark_on_Night").write(m_dark_on_night);
		
		mapData.key(U"Map_size").startObject();
		{
			mapData.key(U"width").write(m_map_size.x);
			mapData.key(U"height").write(m_map_size.y);
		}
		mapData.endObject();
		
		mapData.key(U"Time").startObject();
		{
			mapData.key(U"year").write(m_time_now.year);
			mapData.key(U"month").write(m_time_now.month);
			mapData.key(U"date").write(m_time_now.date);
			mapData.key(U"hour").write(m_time_now.hour);
			mapData.key(U"minutes").write(m_time_now.minutes);
		}
		mapData.endObject();
		
		mapData.key(U"Demand").startObject();
		{
			mapData.key(U"residential").write(m_demand.residential);
			mapData.key(U"commercial").write(m_demand.commercial);
			mapData.key(U"office").write(m_demand.office);
			mapData.key(U"industrial").write(m_demand.industrial);
			mapData.key(U"farm").write(m_demand.farm);
		}
		mapData.endObject();
		
		mapData.key(U"Money").write(m_money);
		
		mapData.key(U"Budget").startObject();
		{
			mapData.key(U"police").write(m_budget.police);
			mapData.key(U"fire_depertment").write(m_budget.fireDepertment);
			mapData.key(U"post_office").write(m_budget.postOffice);
			mapData.key(U"education").write(m_budget.education);
		}
		mapData.endObject();
		
		mapData.key(U"Tax").startObject();
		{
			mapData.key(U"residential").write(m_tax.residential);
			mapData.key(U"commercial").write(m_tax.commercial);
			mapData.key(U"office").write(m_tax.office);
			mapData.key(U"industrial").write(m_tax.industrial);
			mapData.key(U"farm").write(m_tax.farm);
		}
		mapData.endObject();
		
		/*
		 // オブジェクトID
		 int object_id = object[U"objectID"].get<int>();
		 
		 // アドオン名
		 String addon_name = object[U"addon_name"].getString();
		 
		 // 固有名称
		 String original_name = object[U"original_name"].getString();
		 
		 // TypeID
		 TypeID::Type type_id = typeNameToTypeID(object[U"typeID"].getString());
		 
		 // DirectionID
		 DirectionID::Type direction_id = directionNameToDirectionID(object[U"directionID"].getString());
		 
		 // 原点
		 CoordinateStruct origin_coordinate;
		 origin_coordinate.x = object[U"origin_coordinate.x"].get<int>();
		 origin_coordinate.y = object[U"origin_coordinate.y"].get<int>();
		 */
		
		mapData.key(U"Objects").startArray();
		{
			for (auto object : m_objects) {
				mapData.startObject();
				{
					mapData.key(U"objectID").write(object.second.getObjectID());
					mapData.key(U"addon_name").write(object.second.getAddonName(NameMode::English));
					mapData.key(U"original_name").write(object.second.getOriginalName());
					mapData.key(U"typeID").write(typeIDToTypeName(object.second.getTypeID()));
					mapData.key(U"directionID").write(directionIDToDirectionName(object.second.getDirectionID()));
					mapData.key(U"origin_coordinate").startObject();
					{
						mapData.key(U"x").write(object.second.getOriginCoordinate().x);
						mapData.key(U"y").write(object.second.getOriginCoordinate().y);
					}
					mapData.endObject();
				}
				mapData.endObject();
			}
		}
		mapData.endArray();
		
		mapData.key(U"Map").startArray();
		{
			for (int y=0; y<m_map_size.y; y++) {
				mapData.startArray();
				{
					for (int x=0; x<m_map_size.x; x++) {
						mapData.startObject();
						{
							mapData.key(U"objects").startArray();
							{
								for (ObjectStruct object_struct : m_tiles[y][x].getObjectStructs()) {
									mapData.startObject();
									{
										mapData.key(U"objectID").write(object_struct.object_p->getObjectID());
										
										mapData.key(U"relative_coordinate").startObject();
										{
											mapData.key(U"x").write(object_struct.relative_coordinate.relative.x);
											mapData.key(U"y").write(object_struct.relative_coordinate.relative.y);
										}
										mapData.endObject();
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							
							mapData.key(U"residents").write(m_tiles[y][x].residents);
							
							mapData.key(U"workers").startObject();
							{
								mapData.key(U"commercial").write(m_tiles[y][x].workers.commercial);
								mapData.key(U"office").write(m_tiles[y][x].workers.office);
								mapData.key(U"industrial").write(m_tiles[y][x].workers.industrial);
								mapData.key(U"farm").write(m_tiles[y][x].workers.farm);
								mapData.key(U"public").write(m_tiles[y][x].workers.publicFacility);
							}
							mapData.endObject();
							
							mapData.key(U"students").write(m_tiles[y][x].students);
							
							mapData.key(U"rate").startObject();
							{
								for (auto rate = m_tiles[y][x].rate.begin(); rate != m_tiles[y][x].rate.end() ; rate++) {
									if (rate->second != 0) {
										mapData.key(rateIDToRateName(rate->first)).write(rate->second);
									}
								}
							}
							mapData.endObject();
							
							/*
							map_file.key(U"crop").startObject();
							{
								map_file.key(U"name").write(tiles[y][x].crop.name);
								map_file.key(U"amount").write(tiles[y][x].crop.amount);
							}
							map_file.endObject();
							 */
							
							mapData.key(U"age").startArray();
							{
								for (int i=0; i<m_tiles[y][x].age.size(); i++) {
									mapData.write(m_tiles[y][x].age[i]);
								}
							}
							mapData.endArray();
							
							mapData.key(U"gender").startArray();
							{
								for (int i=0; i<m_tiles[y][x].gender.size(); i++) {
									mapData.write(m_tiles[y][x].gender[i]);
								}
							}
							mapData.endArray();
							
							mapData.key(U"work_places").startArray();
							{
								for (int i=0; i<m_tiles[y][x].workPlaces.size(); i++) {
									mapData.startObject();
									{
										mapData.key(U"work_kind").write(m_tiles[y][x].workPlaces[i].workPlace);
										mapData.key(U"serial_number").write(m_tiles[y][x].workPlaces[i].workPlacesSerialNumber);
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							
							mapData.key(U"school").startArray();
							{
								for (int i=0; i<m_tiles[y][x].schools.size(); i++) {
									mapData.startObject();
									{
										mapData.key(U"school_kind").write(m_tiles[y][x].schools[i].school);
										mapData.key(U"serial_number").write(m_tiles[y][x].schools[i].schoolSerialNumber);
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							
							mapData.key(U"reservation").write(m_tiles[y][x].reservation);
							
							mapData.key(U"original_name").write(m_tiles[y][x].getOriginalName());
						}
						mapData.endObject();
					}
				}
				mapData.endArray();
			}
		}
		mapData.endArray();
	}
	mapData.endObject();
	
	saveTextFile(m_map_file_path.toUTF8(), stringXOR(mapData.get().toUTF8(), "citiesboxmapdatafilexor"));
	//saveTextFile(map_file_path.toUTF8()+".cbj", map_file.get().toUTF8());
	//map_file.save(map_file_path+U".cbj");
	
	return true;
}

void CityMap::freeMapAndAddons() {
	for (auto i = m_addons.begin(); i != m_addons.end() ; i++) {
		delete(i->second);
	}
	
	Array<Array<Tile>>().swap(m_tiles);
}

int CityMap::m_get_next_objectID() {
	m_max_object_id ++;
	int next_objectID = m_max_object_id;
	return next_objectID;
}

void CityMap::m_put_grass(CoordinateStruct arg_coordinate) {
	int objectID = m_get_next_objectID();
	cout << "put grass: " << objectID << endl;
	m_objects[objectID] = Object(objectID, m_addons[U"tile_greenfield"], U"", TypeID::Normal, DirectionID::None, arg_coordinate);
	
	RelativeCoordinateStruct relative_coordinate;
	relative_coordinate.origin = arg_coordinate;
	relative_coordinate.relative = CoordinateStruct{0, 0};
	
	m_tiles[arg_coordinate.y][arg_coordinate.x].addObject(&(m_objects[objectID]), relative_coordinate);
}
