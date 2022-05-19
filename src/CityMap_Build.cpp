//
//  CityMap_Build.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/26.
//

#include "CityMap.hpp"

// アドオンの設置
bool CityMap::build(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak) {
	if (selectedAddon->isInCategories(CategoryID::Connectable)) {
		return buildConnectableType(cursor, before_cursor, selectedAddon, needToBreak);
	}
	else {
		return buildBuilding(cursor, before_cursor, selectedAddon, needToBreak);
	}
}

bool CityMap::buildBuilding(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak) {
	// ObjectIDの決定
	int objectID = m_get_next_objectID();
	
	tuple<bool, TypeID::Type, DirectionID::Type> build_tuple = canBuildBuildingHere(cursor.coordinate, selectedAddon);
	TypeID::Type type = get<1>(build_tuple);
	DirectionID::Type direction = get<2>(build_tuple);
	
	if (get<0>(build_tuple)) {		// 設置可能なら...
		CoordinateStruct useTiles = selectedAddon->getUseTiles(type, direction);

		CoordinateStruct origin_coordinate = cursor.coordinate;
		if (direction == DirectionID::West) {
			origin_coordinate.y -= useTiles.y - 1;
		}
		if (direction == DirectionID::East) {
			origin_coordinate.x -= useTiles.x - 1;
			origin_coordinate.y -= useTiles.y - 1;
		}
		else if (direction == DirectionID::South) {
			origin_coordinate.y -= useTiles.y - 1;
		}

		// オブジェクトの生成
		m_objects[objectID] = new NormalObject(objectID, selectedAddon, U"", type, direction, origin_coordinate);

		// 建設するタイル上の既存のオブジェクトを削除
		for (int y = origin_coordinate.y; y < origin_coordinate.y + useTiles.y; y++) {
			for (int x = origin_coordinate.x; x < origin_coordinate.x + useTiles.x; x++) {
				breaking(CoordinateStruct{ x, y }, true, false, true);
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

				m_tiles[y][x].addObject(m_objects[objectID], relative_coordinate);
			}
		}

		// 効果を反映
		setRate(m_objects[objectID], origin_coordinate, false);
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

				int rate = effect_per_tile * max(abs(effect.grid - 1 - relative_y), abs(effect.grid - 1 - relative_x));
				if (will_be_deleted) {
					rate *= -1;
				}
				m_tiles[y][x].setRate(effect_map.first, rate);
			}
		}
	}
}

void CityMap::breaking(CoordinateStruct coordinate, bool isTemporaryDelete, bool updateAroundTiles, bool deleteThis) {
	// オブジェクトの除去
	for (ObjectStruct object_struct : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
        breakOnce(object_struct, coordinate, isTemporaryDelete, updateAroundTiles, deleteThis);
	}
}
void CityMap::breakOnlyCategory(CategoryID::Type category, CoordinateStruct coordinate, bool isTemporaryDelete, bool updateAroundTiles, bool deleteThis) {
    // オブジェクトの除去
    for (ObjectStruct object_struct : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
        if (object_struct.object_p->getAddonP()->isInCategories(category)) {
            breakOnce(object_struct, coordinate, isTemporaryDelete, updateAroundTiles, deleteThis);
        }
    }
}
void CityMap::breakOnce(ObjectStruct &object_struct,CoordinateStruct coordinate, bool isTemporaryDelete, bool updateAroundTiles, bool deleteThis) {
    // 効果を削除
    setRate(object_struct.object_p, object_struct.relative_coordinate.origin, true);

    int delete_object_id = object_struct.object_p->getObjectID();

    Size delete_object_required_tiles = object_struct.object_p->getAddonDirectionStruct().requiredTiles;
    for (int y = object_struct.relative_coordinate.origin.y; y < object_struct.relative_coordinate.origin.y + delete_object_required_tiles.y; y++) {
        for (int x = object_struct.relative_coordinate.origin.x; x < object_struct.relative_coordinate.origin.x + delete_object_required_tiles.x; x++) {
            // クリア処理
            if (updateAroundTiles)
                updateConnectedTiles(CoordinateStruct{x, y});
            
            m_tiles[y][x].deleteObject(delete_object_id);

            // 更地になったら芝生を置く
            // 要修正 : 共通の動作は一つの関数にまとめること
            if ((!isTemporaryDelete || x != coordinate.x || y != coordinate.y) && m_tiles[y][x].getObjectStructs().size() == 0) {
                UnitaryTools::debugLog(U"before put");
                m_put_grass(CoordinateStruct{ x, y });
                UnitaryTools::debugLog(U"after put");
            }
        }
    }
    // クリア処理
    // オブジェクト自体を除去
    if (deleteThis)
        delete(object_struct.object_p);

    UnitaryTools::debugLog(U"before erase");
    m_objects.erase(delete_object_id);
    UnitaryTools::debugLog(U"after erase");
}

CoordinateStruct CityMap::moveToAddonStartTile(CoordinateStruct searchCoordinate, int addonNumber) {
	Tile* searchTile = &m_tiles[searchCoordinate.y][searchCoordinate.x];

	searchCoordinate.x -= searchTile->tilesCount.x;
	searchCoordinate.y += searchTile->tilesCount.y;

	return CoordinateStruct{ searchCoordinate.x, searchCoordinate.y };
}

// 建物の建設の可否
tuple<bool, TypeID::Type, DirectionID::Type> CityMap::canBuildBuildingHere(CoordinateStruct coordinate, CBAddon* addon) {
	TypeID::Type type_id = TypeID::Disabled;
	DirectionID::Type direction_id = DirectionID::Disabled;
	
	// タイルの場合
	if (addon->isInCategories(CategoryID::PutType)) {
		type_id = TypeID::Normal;
		direction_id = DirectionID::None;
		return tuple<bool, TypeID::Type, DirectionID::Type>{true, type_id, direction_id};
	}
	
	// 建物の場合
	for (int i = 0; i < AROUND_TILES; i++) {
		CoordinateStruct currentTile = { coordinate.x + AroundTiles[i].second.x, coordinate.y + AroundTiles[i].second.y };

		for (ObjectStruct object_struct : m_tiles[currentTile.y][currentTile.x].getObjectStructs()) {
			if (object_struct.object_p->getAddonP()->isInCategories(CategoryID::Road)) {
				TypeID::Type ret_type_temp = TypeID::Normal;
				DirectionID::Type ret_direction_temp = AroundTiles[i].first;

				// 複数のタイルを使う場合、建てる方向に障害物などがないか確認する
				bool cannotBuild = false;

				int addX = 1, addY = 1;
				if (ret_direction_temp == DirectionID::East) {
					addX = -1;
				}
				else if (ret_direction_temp == DirectionID::South) {
					addY = -1;
				}

				for (int y = 0; y < addon->getUseTiles(type_id, direction_id).y; y += addY) {
					for (int x = 0; y < addon->getUseTiles(type_id, direction_id).x; x += addX) {
						for (ObjectStruct object_struct_sub : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
							if (object_struct_sub.object_p->getAddonP()->isInCategories(CategoryID::Connectable)) {
								cannotBuild = true;
								break;
							}
						}
						if (cannotBuild) break;
					}
					if (cannotBuild) break;
				}

				if (!cannotBuild) {
					type_id = ret_type_temp;
					direction_id = ret_direction_temp;
					return tuple<bool, TypeID::Type, DirectionID::Type>{true, type_id, direction_id};
				}
			}
		}
	}
	
	return tuple<bool, TypeID::Type, DirectionID::Type>{false, type_id, direction_id};
}

// アドオンを削除
// 要修正 : タイルを直接弄らないこと
void CityMap::updateConnectedTiles(CoordinateStruct position) {
	Tile* currentTile = &m_tiles[position.y][position.x];
	
	// 道路なら：disconnectする
	for (auto current_object : currentTile->getObjectsP(CategoryID::Connectable)) {
		current_object->del(road_network);
	}
	
	// 更地化
	/*
	Addon* selectedAddon = m_addons[U"tile_greenfield"];

	currentTile->clearAll();

	currentTile->addType(TypeID::Normal);
	currentTile->addDirection(DirectionID::None);
	currentTile->addons << selectedAddon;
*/
	// 幸福度を戻す
}

bool CityMap::isPositionAvailable(CoordinateStruct coordinate) {
	if (coordinate.x >= 0 && coordinate.x <= m_map_size.x - 1 && coordinate.y >= 0 && coordinate.y <= m_map_size.y - 1) {
		return true;
	}
	else {
		return false;
	}
}

void CityMap::m_put_grass(CoordinateStruct arg_coordinate) {
	int objectID = m_get_next_objectID();
	cout << "put grass: " << objectID << endl;
	m_objects[objectID] = new NormalObject(objectID, m_addons[U"tile_greenfield"], U"", TypeID::Normal, DirectionID::None, arg_coordinate);

	RelativeCoordinateStruct relative_coordinate;
	relative_coordinate.origin = arg_coordinate;
	relative_coordinate.relative = CoordinateStruct{ 0, 0 };

	m_tiles[arg_coordinate.y][arg_coordinate.x].addObject(m_objects[objectID], relative_coordinate);
}



/*
 
 bool CityMap::getBuildTypeAndDirection(CoordinateStruct coordinate, Addon* selectedAddon, TypeID::Type& retType, DirectionID::Type& retDirection) {
	 // 接続タイプ（道路など）アドオンの場合
	 if (selectedAddon->isInCategories(CategoryID::Connectable)) {
		 // 周囲に道路があるか（建設可能か）確認する
		 int totalAroundRoad = 0;
		 Array<pair<DirectionID::Type, CoordinateStruct>> aroundRoadCoordinate;

		 for (int i = 0; i < AROUND_TILES; i++) {
			 CoordinateStruct currentTile = { coordinate.x + AroundTiles[i].second.x, coordinate.y + AroundTiles[i].second.y };

			 if (currentTile.x < 0 || currentTile.y < 0 || currentTile.x >= m_map_size.x || currentTile.y >= m_map_size.y) {
				 continue;
			 }

			 for (ObjectStruct object_struct : m_tiles[currentTile.y][currentTile.x].getObjectStructs()) {
				 // 道路の場合
				 if ((selectedAddon->isInCategories(CategoryID::Road) && object_struct.object_p->getAddonP()->isInCategories(CategoryID::Road)) ||
					 (selectedAddon->isInCategories(CategoryID::Train) && object_struct.object_p->getAddonP()->isInCategories(CategoryID::Train)) ||
					 (selectedAddon->isInCategories(CategoryID::Waterway) && object_struct.object_p->getAddonP()->isInCategories(CategoryID::Waterway)) ||
					 (selectedAddon->isInCategories(CategoryID::Airport) && object_struct.object_p->getAddonP()->isInCategories(CategoryID::Airport))) {
					 totalAroundRoad++;
					 aroundRoadCoordinate << AroundTiles[i];

					 break;
				 }
			 }
		 }

		 // 踏切を設置する必要がある場合
		 if (selectedAddon->isInCategories(CategoryID::Road)) {
			 for (ObjectStruct object_struct : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
				 if (object_struct.object_p->getAddonP()->isInCategories(CategoryID::Railroad)) {
					 retType = TypeID::TrainCrossing;

					 if (object_struct.object_p->getDirectionID() == DirectionID::NorthSouth) {
						 retDirection = DirectionID::NorthSouth;
					 }
					 else {
						 retDirection = DirectionID::EastWest;
					 }

					 return true;
				 }
			 }
		 }

		 // 橋を設置する必要がある場合
		 if (selectedAddon->isInCategories(CategoryID::Road)) {
			 for (ObjectStruct object_struct : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
				 if (object_struct.object_p->getAddonP()->isInCategories(CategoryID::Waterway)) {
					 retType = TypeID::Bridge;

					 if (object_struct.object_p->getDirectionID() == DirectionID::NorthSouth) {
						 retDirection = DirectionID::EastWest;
					 }
					 else {
						 retDirection = DirectionID::NorthSouth;
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
			 CoordinateStruct roadDelta = { 0, 0 };

			 for (int i = 0; i < 2; i++) {
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
				 for (int i = 0; i < aroundRoadCoordinate.size(); i++) {
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
			 for (int i = 0; i < aroundRoadCoordinate.size(); i++) {
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
	 if (selectedAddon->isInCategories(CategoryID::ObjectType)) {
		 // 周囲に道路があるか確認する
		 for (int i = 0; i < AROUND_TILES; i++) {
			 CoordinateStruct currentTile = { coordinate.x + AroundTiles[i].second.x, coordinate.y + AroundTiles[i].second.y };

			 for (ObjectStruct object_struct : m_tiles[currentTile.y][currentTile.x].getObjectStructs()) {
				 if (object_struct.object_p->getAddonP()->isInCategories(CategoryID::Road)) {
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

					 for (int y = 0; y < selectedAddon->getUseTiles(retType, retDirection).y; y += addY) {
						 for (int x = 0; y < selectedAddon->getUseTiles(retType, retDirection).x; x += addX) {
							 for (ObjectStruct object_struct_sub : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
								 if (object_struct_sub.object_p->getAddonP()->isInCategories(CategoryID::Connectable)) {
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
	 if (selectedAddon->isInCategories(CategoryID::PutType)) {
		 retType = TypeID::Normal;
		 retDirection = DirectionID::None;
		 return true;
	 }
	 cout << "return false" << endl;
	 return false;					// 存在しない or 設置不可能な場合
 }
 */
