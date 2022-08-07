//
//  CityMap_LoadData.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/25.
//

#include "CityMap.hpp"

void CityMap::load(String loadMapFilePath) {
	/*
	if (FileSystem::Extension(loadMapFilePath) == U"cbd") {
		loadCBD(loadMapFilePath);
	}
	else */if (FileSystem::Extension(loadMapFilePath) == U"cbj") {
		m_load_CBJ(loadMapFilePath);
	}
}

static s3d::String extracted(int i, Array<s3d::String> &workplaceStr) {
	String workplaceAndSerial = workplaceStr[i].substr(1, workplaceStr[i].length()-1);
	return workplaceAndSerial;
}

void CityMap::m_load_CBJ(String loadMapFilePath) {
	m_map_file_path = loadMapFilePath;
	
	ifstream ifs(m_map_file_path.toUTF8().c_str(), ios::in | ios::binary);
	
	std::string mapXOR((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	string mapDataStr = UnitaryTools::stringXOR(mapXOR, "citiesboxmapdatafilexor");
	UnitaryTools::saveTextFile("./data/map_temp.cbj_unxor", mapDataStr);
	UnitaryTools::saveTextFile("./data/map_temp.cbj_temp", mapDataStr);
	
	JSON mapData = JSON::Load(U"./data/map_temp.cbj_temp");
	//remove("./data/map_temp.cbj_temp");
	
	m_saved_version = mapData[U"Version"].get<int>();
	
	m_addon_set_name = mapData[U"Addon_Set"].getString();
	// -> アドオン読み込み
	m_load_addons(m_addon_set_name);
	
	m_city_name = mapData[U"City_Name"].getString();
	
	m_mayor_name = mapData[U"Mayor_Name"].getString();
	
	m_total_population = mapData[U"Total_Population"].get<int>();
	
	m_change_weather = UnitaryTools::getBoolFromJson(mapData[U"Change_Weather"]);
	
	m_temperature = mapData[U"Temperature"].get<int>();
	
	m_dark_on_night = UnitaryTools::getBoolFromJson(mapData[U"Dark_on_Night"]);
	
	m_map_size.x = mapData[U"Map_size"][U"width"].get<int>();
	m_map_size.y = mapData[U"Map_size"][U"height"].get<int>();
	
	m_time_now.year = mapData[U"Time"][U"year"].get<int>();
	m_time_now.month = mapData[U"Time"][U"month"].get<int>();
	m_time_now.date = mapData[U"Time"][U"date"].get<int>();
	m_time_now.hour = mapData[U"Time"][U"hour"].get<int>();
	m_time_now.minutes = mapData[U"Time"][U"minutes"].get<int>();
	
	m_demand.residential = mapData[U"Demand"][U"residential"].get<int>();
	m_demand.commercial = mapData[U"Demand"][U"commercial"].get<int>();
	m_demand.office = mapData[U"Demand"][U"office"].get<int>();
	m_demand.industrial = mapData[U"Demand"][U"industrial"].get<int>();
	m_demand.farm = mapData[U"Demand"][U"farm"].get<int>();
	
	m_money = mapData[U"Money"].get<int>();
	
	m_budget.police = mapData[U"Budget"][U"police"].get<int>();
	m_budget.fireDepertment = mapData[U"Budget"][U"fire_depertment"].get<int>();
	m_budget.postOffice = mapData[U"Budget"][U"post_office"].get<int>();
	m_budget.education = mapData[U"Budget"][U"education"].get<int>();
	
	m_tax.residential = mapData[U"Tax"][U"residential"].get<int>();
	m_tax.commercial = mapData[U"Tax"][U"commercial"].get<int>();
	m_tax.office = mapData[U"Tax"][U"office"].get<int>();
	m_tax.industrial = mapData[U"Tax"][U"industrial"].get<int>();
	m_tax.farm = mapData[U"Tax"][U"farm"].get<int>();
	
	// オブジェクトの読み込み(r142以降)
	if (m_saved_version >= 142) {
		m_max_object_id = 0;
		if (mapData[U"Objects"].getType() == JSONValueType::Array) {
			for (const auto& object : mapData[U"Objects"].arrayView()) {
				// オブジェクトID
				int object_id = object[U"objectID"].get<int>();
				
				// アドオン名
				String addon_name = object[U"addon_name"].getString();
				
				// 固有名称
				String original_name = object[U"original_name"].getString();

				// 描画の有無
				bool visible = true;
				if (object.hasElement(U"visible")) {
					visible = object[U"visible"].get<bool>();
				}
				
				TypeID::Type type_id;
				DirectionID::Type direction_id;
				
				if (m_saved_version <= 142 && m_addons[addon_name]->isInCategories(CategoryID::Waterway)) {
					String type_id_str = object[U"typeID"].getString();
					
					direction_id = UnitaryTools::directionNameToDirectionID(object[U"directionID"].getString());
					
					if (type_id_str == U"IntersectionCross" || type_id_str == U"intersection_cross") {
						type_id = TypeID::WaterOffshore;
						direction_id = DirectionID::Offshore;
					}
					else if (type_id_str == U"IntersectionT" || type_id_str == U"intersection_T") {
						type_id = TypeID::WaterIntersectionT;
					}
					else if (type_id_str == U"Turn" || type_id_str == U"turn") {
						type_id = TypeID::WaterTurn;
					}
					else {
						type_id = UnitaryTools::typeNameToTypeID(object[U"typeID"].getString());
					}
					
					// DirectionID
					if (type_id == TypeID::WaterTurn) {
						if (direction_id == DirectionID::NorthEast) {
							direction_id = DirectionID::WithoutNorthNortheastEast;
						}
						if (direction_id == DirectionID::SouthEast) {
							direction_id = DirectionID::WithoutEastSoutheastSouth;
						}
						if (direction_id == DirectionID::NorthWest) {
							direction_id = DirectionID::WithoutSouthSouthwestWest;
						}
						if (direction_id == DirectionID::SouthWest) {
							direction_id = DirectionID::WithoutNorthWestNorthwest;
						}
					}
					else if (type_id == TypeID::WaterIntersectionT) {
						if (direction_id == DirectionID::NorthSouthEast) {
							direction_id = DirectionID::WithoutWest;
						}
						if (direction_id == DirectionID::NorthSouthWest) {
							direction_id = DirectionID::WithoutEast;
						}
						if (direction_id == DirectionID::NorthEastWest) {
							direction_id = DirectionID::WithoutSouth;
						}
						if (direction_id == DirectionID::SouthEastWest) {
							direction_id = DirectionID::WithoutNorth;
						}
					}
					else if (type_id == TypeID::IntersectionCross) {
						if (direction_id == DirectionID::None) {
							direction_id = DirectionID::Offshore;
						}
					}
					else if (type_id == TypeID::WaterEstuary) {
						if (direction_id == DirectionID::NorthSouthEast) {
							direction_id = DirectionID::WithoutNortheastSoutheast;
						}
						if (direction_id == DirectionID::NorthSouthWest) {
							direction_id = DirectionID::WithoutSouthwestNorthwest;
						}
						if (direction_id == DirectionID::NorthEastWest) {
							direction_id = DirectionID::WithoutNortheastNorthwest;
						}
						if (direction_id == DirectionID::SouthEastWest) {
							direction_id = DirectionID::WithoutSoutheastSouthwest;
						}
					}
					else if (type_id == TypeID::WaterIntersectionCrossWithoutOneCorner) {
						if (direction_id == DirectionID::NorthEast) {
							direction_id = DirectionID::WithoutNortheast;
						}
						if (direction_id == DirectionID::SouthEast) {
							direction_id = DirectionID::WithoutSoutheast;
						}
						if (direction_id == DirectionID::NorthWest) {
							direction_id = DirectionID::WithoutNorthwest;
						}
						if (direction_id == DirectionID::SouthWest) {
							direction_id = DirectionID::WithoutSouthwest;
						}
					}
					
					//cout << "type: " << type_id << " / direction: " << direction_id << endl;
				}
				else {
					// TypeID
					type_id = UnitaryTools::typeNameToTypeID(object[U"typeID"].getString());
					
					// DirectionID
					direction_id = UnitaryTools::directionNameToDirectionID(object[U"directionID"].getString());
					
					if (direction_id == DirectionID::None && type_id == TypeID::IntersectionCross) {
						direction_id = DirectionID::All;
					}
				}
				
				// 原点
				CoordinateStruct origin_coordinate;
				origin_coordinate.x = object[U"origin_coordinate"][U"x"].get<int>();
				origin_coordinate.y = object[U"origin_coordinate"][U"y"].get<int>();
				
				// オブジェクトをリストに登録
				// 道路や線路などConnectableなオブジェクトならConnectavleObjectに
				if (m_addons[addon_name]->isInCategories(CategoryID::Connectable)) {
					m_objects[object_id] = new ConnectableObject(object_id, m_addons[addon_name], original_name, type_id, direction_id, origin_coordinate);
					m_objects[object_id]->setVisible(visible);
				}
				// その他建物などはNormalObjectに
				else {
                    // タイルアドオンなら共通オブジェクトにまとめる
                    bool object_loaded = false;
                    if (m_addons[addon_name]->isInCategories(CategoryID::Tile)) {
                        if (m_common_objects.find(addon_name) == m_common_objects.end()) {
                            UnitaryTools::debugLog(U"common object: name={} object_id={}"_fmt(addon_name, object_id));
                            
                            // m_common_objectsに共通オブジェクトが登録されてない場合は追加
                            m_common_objects[addon_name] = new NormalObject(object_id, m_addons[addon_name], original_name, type_id, direction_id, origin_coordinate);
                            m_common_objects[addon_name]->setVisible(visible);
                            m_common_objects[addon_name]->setCommonObject();
                            
                            m_objects[object_id] = m_common_objects[addon_name];
                            object_loaded = true;
                        }
                    }
                    
                    if (!object_loaded) {
                        m_objects[object_id] = new NormalObject(object_id, m_addons[addon_name], original_name, type_id, direction_id, origin_coordinate);
                        m_objects[object_id]->setVisible(visible);
                    }
				}
				
				if (object_id > m_max_object_id) {
					m_max_object_id = object_id;
				}
			}
		}
	}
	
	int y = 0;
	if (mapData[U"Map"].getType() == JSONValueType::Array) {
		for (const auto& mapTiles : mapData[U"Map"].arrayView()) {
			m_tiles.push_back(Array<Tile>());
			int x = 0;
			if (mapTiles.getType() == JSONValueType::Array) {
				for (const auto& tile : mapTiles.arrayView()) {
					m_tiles[y].push_back(Tile());
					
					if (m_saved_version <= 141) {			// r141以前なら内容を修正
						CoordinateStruct tiles_count;
						tiles_count.x = tile[U"tiles_count"][U"x"].get<int>();
						tiles_count.y = tile[U"tiles_count"][U"y"].get<int>();
						
						int serial_number = tile[U"serial_number"].get<int>();
						int object_count = 0;
						if (tile[U"addons"].getType() == JSONValueType::Array) {
							for (const auto& jAddons : tile[U"addons"].arrayView()) {
								TypeID::Type type_id = UnitaryTools::typeNameToTypeID(jAddons[U"type_number"].getString());
								DirectionID::Type direction_id = UnitaryTools::directionNameToDirectionID(jAddons[U"direction_number"].getString());
								
								// 水路用
								if (direction_id == DirectionID::None && type_id == TypeID::IntersectionCross) {
									direction_id = DirectionID::All;
								}
								
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
										// 元々のserial_num == 0なら空き番号に振り直す <- 道路やタイルなど
										// あるいはObjectIDが被った場合に振り直す
										if (serial_number == 0 || m_objects.count(serial_number) > 0) {
											serial_number = m_max_object_id + 1;
											m_max_object_id ++;
										}
										
										// オブジェクトをリストに登録
										if (m_addons[jAddons[U"name"].getString()]->isInCategories(CategoryID::Connectable)) {
											m_objects[serial_number] = new ConnectableObject(serial_number, m_addons[jAddons[U"name"].getString()], tile[U"original_name"].getString(), type_id, direction_id, CoordinateStruct{x, y});
										}
										else {
											m_objects[serial_number] = new NormalObject(serial_number, m_addons[jAddons[U"name"].getString()], tile[U"original_name"].getString(), type_id, direction_id, CoordinateStruct{x, y});
										}
									}
									else {
										CoordinateStruct origin_coordinate;
										origin_coordinate.x = x - tiles_count.x;
										origin_coordinate.y = y - tiles_count.y;
										
										// 原点とObjectIDが一致しない -> ObjectIDを原点のものに修正
										if (m_objects[serial_number]->getOriginCoordinate().x != origin_coordinate.x ||
											m_objects[serial_number]->getOriginCoordinate().y != origin_coordinate.y) {
											
											//cout << "at " << x << "," << y << " from " << origin_coordinate.x << "," << origin_coordinate.y << ":" << serial_number << " to " << m_tiles[origin_coordinate.y][origin_coordinate.x].getObjectP(jAddons[U"name"].getString(), NameMode::English)->getObjectID() << endl;
											
											serial_number = m_tiles[origin_coordinate.y][origin_coordinate.x].getObjectP(jAddons[U"name"].getString(), NameMode::English)->getObjectID();
										}
									}
									
									// RelativeCoordinateStructを作成
									RelativeCoordinateStruct relarive_coordinate;
									relarive_coordinate.origin = m_objects[serial_number]->getOriginCoordinate();
									relarive_coordinate.relative = tiles_count;
									
									// オブジェクトをタイルに格納
									m_tiles[y][x].addObject(m_objects[serial_number], relarive_coordinate);
									
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
					}
					else {
						if (tile[U"objects"].getType() == JSONValueType::Array) {
							for (const auto& jObject : tile[U"objects"].arrayView()) {
								// オブジェクトIDを取得
								int object_id = jObject[U"objectID"].get<int>();
                                
                                Object *obj;
                                RelativeCoordinateStruct relarive_coordinate;
                                
                                // もし共通オブジェクトに登録されたアドオンであれば、共通オブジェクトに置き換え（タイルのみ）
                                if (m_objects[object_id]->getAddonP()->isInCategories(CategoryID::Tile)) {
                                    auto itr = m_common_objects.find(m_objects[object_id]->getAddonP()->getName(NameMode::English));
                                    if (itr != m_common_objects.end()) {
                                        obj = itr->second;
                                        relarive_coordinate.origin = m_objects[object_id]->getOriginCoordinate();
										
										if (itr->second->getObjectID() != object_id) {
											m_objects[object_id]->setDeleted();
											m_objects.erase(object_id);
										}
                                    }
                                    else {
                                        obj = m_objects[object_id];
                                        relarive_coordinate.origin = obj->getOriginCoordinate();
                                    }
                                }
                                else {
                                    obj = m_objects[object_id];
                                    relarive_coordinate.origin = obj->getOriginCoordinate();
                                }
								
								// RelativeCoordinateStructを作成
								relarive_coordinate.relative.x = jObject[U"relative_coordinate"][U"x"].get<int>();
								relarive_coordinate.relative.y = jObject[U"relative_coordinate"][U"y"].get<int>();
								
								m_tiles[y][x].addObject(obj, relarive_coordinate);
							}
						}
					}
					
					m_tiles[y][x].residents = tile[U"residents"].get<int>();
					
					m_tiles[y][x].workers.commercial = tile[U"workers"][U"commercial"].get<int>();
					m_tiles[y][x].workers.office = tile[U"workers"][U"office"].get<int>();
					m_tiles[y][x].workers.industrial = tile[U"workers"][U"industrial"].get<int>();
					m_tiles[y][x].workers.farm = tile[U"workers"][U"farm"].get<int>();
					m_tiles[y][x].workers.publicFacility = tile[U"workers"][U"public"].get<int>();
					
					m_tiles[y][x].students = tile[U"students"].get<int>();
					
					// 各率の読み込み
					for (const auto& rate : tile[U"rate"]) {
						m_tiles[y][x].rate[UnitaryTools::rateNameToRateID(rate.key)] = rate.value.get<int>();
					}
					
					/*
					tiles[y][x].crop.name = square[U"crop.name"].getString();
					tiles[y][x].crop.amount = square[U"crop.amount"].get<int>();
					*/
					
					
					
					m_tiles[y][x].age = UnitaryTools::getIntArrayFromJsonArray(tile[U"age"]);
					
					m_tiles[y][x].gender = UnitaryTools::getStrArrayFromJsonArray(tile[U"gender"]);
					
					if (tile[U"work_places"].getType() == JSONValueType::Array) {
						for (const auto& workPlaces : tile[U"work_places"].arrayView()) {
							m_tiles[y][x].workPlaces.push_back(WorkPlaceStruct());
							
                            if (workPlaces[U"work_kind"].getType() == JSONValueType::Number)
                                m_tiles[y][x].workPlaces.back().workPlace = UnitaryTools::getRCOIFP(workPlaces[U"work_kind"].get<int>());
                            if (workPlaces[U"serial_number"].getType() == JSONValueType::Number)
                                m_tiles[y][x].workPlaces.back().workPlacesSerialNumber = workPlaces[U"serial_number"].get<int>();
						}
					}
					
					if (tile[U"school"].getType() == JSONValueType::Array) {
						for (const auto& schools : tile[U"school"].arrayView()) {
							m_tiles[y][x].schools.push_back(SchoolStruct());
							
                            if (schools[U"school_kind"].getType() == JSONValueType::Number)
                                m_tiles[y][x].schools.back().school = UnitaryTools::getSchool(schools[U"school_kind"].get<int>());
							if (schools[U"serial_number"].getType() == JSONValueType::Number)
                                m_tiles[y][x].schools.back().schoolSerialNumber = schools[U"serial_number"].get<int>();
						}
					}
					
					//m_tiles[y][x].reservation = UnitaryTools::getRCOIFP(tile[U"reservation"].get<int>());
					
					m_tiles[y][x].setOriginalName(tile[U"original_name"].getString());
					
					x++;
				}
			}
			y++;
		}
	}
}

void CityMap::m_load_addons(String addonSetName) {
	//Array<FileStruct> addons_path = specific::getAllFilesName("./addons", "adat");
	Array<FileStruct> addonsPath = specific::getAllFilesName("./addons", "adj");
	
	for (int i=0; i<(int)addonsPath.size(); i++) {
		UnitaryTools::debugLog(U"m_load_addons", U"from " + Unicode::Widen(addonsPath[i].file_path));
		FileStruct fileTemp = addonsPath[i];
		
		CBAddon* loadingAddon = new CBAddon();
		if (loadingAddon->load(addonsPath[i], addonSetName)) {
			m_addons[loadingAddon->getName(NameMode::English)] = loadingAddon;
		}
		else {
			delete(loadingAddon);
		}
		
		System::Sleep(20);
	}
}
