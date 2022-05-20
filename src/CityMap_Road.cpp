//
//  CityMap_Road.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/27.
//

#include "CityMap.hpp"

bool CityMap::buildConnectableType(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak) {
	cout << "into build connect type at " << cursor.coordinate.x << "," << cursor.coordinate.y << endl;
	
	// 同じアドオンが指定されたタイル上に存在するなら -> アドオンの変更は行わず、既存のアドオンのDirectionIDとTypeIDを変更
	if (m_tiles[cursor.coordinate.y][cursor.coordinate.x].hasAddon(selectedAddon)) {
		return updateConnectionType(cursor, before_cursor, selectedAddon, needToBreak);
	}
	
	// ObjectIDの決定
	int objectID = m_get_next_objectID();

	if (canBuildRoadHere(cursor.coordinate)) {			// 設置可能なら...
        TypeID::Type type;
        DirectionID::Type direction;
        bool do_not_update_selected_addon_type = m_is_there_crossable_object(selectedAddon, cursor.coordinate);
        Console << U"dnusat: " << do_not_update_selected_addon_type;
        
        // typeとdirectionを設置
        type = setRoadType(cursor.coordinate, selectedAddon);
        direction = setRoadDirection(cursor.coordinate, selectedAddon);
		
		CoordinateStruct useTiles = selectedAddon->getUseTiles(type, direction);
		cout << "use tiles: " << useTiles.x << "," << useTiles.y << endl;

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
        cout << "oc:" << origin_coordinate.x << "," << origin_coordinate.y << endl;
		m_objects[objectID] = new ConnectableObject(objectID, selectedAddon, U"", type, direction, origin_coordinate);
        
        // 工事中の状態に指定
        m_constructing_connectable_objects << m_objects[objectID];

		// 建設するタイル上の既存のオブジェクトを削除
        for (int y = origin_coordinate.y; y < origin_coordinate.y + useTiles.y; y++) {
            for (int x = origin_coordinate.x; x < origin_coordinate.x + useTiles.x; x++) {
                // もともとのアドオンと同種のアドオンなら：周囲の切断は行わない
                bool disconnect = true;
                for (auto object_p : m_tiles[y][x].getObjectsP(CategoryID::Connectable)) {
                    if (object_p->getAddonP()->isInCategories(getConnectableCategoryID(selectedAddon))) {
                        disconnect = false;
                    }
                }
                cout << "break at " << x << "," << y << endl;
                
                // 異なる接続可能オブジェクトが交差する場合 -> 同じタイプのオブジェクトのみ除去
                if (do_not_update_selected_addon_type) {
                    CategoryID::Type connectable_category = selectedAddon->getMainConnectableCategory();
                    if (connectable_category == CategoryID::Disabled) {
                        return false;
                    }
                    breakOnlyCategory(connectable_category, CoordinateStruct{ x, y }, true, false, true);
                }
                // その他 -> タイル上のすべてのオブジェクトを除去
                else {
                    breaking(CoordinateStruct{ x, y }, true, disconnect, disconnect);
                }
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

				cout << "build at " << x << "," << y << " : " << m_objects[objectID]->getAddonName(NameMode::English) << " " << objectID << endl;
				m_tiles[y][x].addObject(m_objects[objectID], relative_coordinate);
			}
		}
		
		// カーソルが移動前の座標から連続して押し続けて移動していれば、そのタイルと接続する
		if (before_cursor.pressed && cursor.coordinate != before_cursor.coordinate) {
			connectObjects(before_cursor.coordinate, cursor.coordinate, objectID);
		}

		// 効果を反映
		setRate(m_objects[objectID], origin_coordinate, false);

		// 周囲9マスを更新
		/*
		for (int y = max(origin_coordinate.y - 1, 0); y < min(origin_coordinate.y + 2, m_map_size.y); y++) {
			for (int x = max(origin_coordinate.x - 1, 0); x < min(origin_coordinate.x + 2, m_map_size.x); x++) {
				if (x == origin_coordinate.x && y == origin_coordinate.y) {
					continue;		// 自分自身は更新しない
				}

				m_tiles[y][x].updateConnections(m_tiles);
			}
		}*/
	}
    

	return true;
}

bool CityMap::updateConnectionType(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak) {
	// ObjectIDの決定
	cout << "update connection type at " << cursor.coordinate.x << "," << cursor.coordinate.y << endl;
	Object* object = m_tiles[cursor.coordinate.y][cursor.coordinate.x].getObjectP(selectedAddon->getName(NameMode::English), NameMode::English);

	TypeID::Type type = setRoadType(cursor.coordinate, selectedAddon);
	DirectionID::Type direction = setRoadDirection(cursor.coordinate, selectedAddon);
	
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
	
	// カーソルが移動前の座標から連続して押し続けて移動していれば、そのタイルと接続する
	if (before_cursor.pressed && cursor.coordinate != before_cursor.coordinate) {
		connectObjects(before_cursor.coordinate, cursor.coordinate, object->getObjectID());
	}

	// 効果を反映
	setRate(object, origin_coordinate, false);
	
	return true;
}

void CityMap::connectObjects(CoordinateStruct from, CoordinateStruct to, int object_id) {
	for (auto from_coordinate_object_struct : m_tiles[from.y][from.x].getObjectStructs()) {
		if (from_coordinate_object_struct.object_p->getAddonP()->canConnect(m_objects[object_id]->getAddonP())) {
            if (from_coordinate_object_struct.object_p->getTypeID() != TypeID::TrainCrossing
            && from_coordinate_object_struct.object_p->getTypeID() != TypeID::Bridge) {
                from_coordinate_object_struct.object_p->connect(
                    road_network,
                    CoordinateStruct{ 0, 0 },            // 暫定
                    m_objects[object_id]
                );
            }
            
            // 橋や踏切を設置する必要があるか否か？
            bool other_crossable_object = false;
            TypeID::Type type = TypeID::Disabled;
            if (!(other_crossable_object = putTrainCrossing(m_objects[object_id]->getAddonP(), to, type))) {
                other_crossable_object = putBridge(m_objects[object_id]->getAddonP(), to, type);
            }
            
            // 橋や踏切を設置する必要がない場合は通常通り接続
            if (other_crossable_object) {
                m_objects[object_id]->connectWithSpecifiedType(
                    road_network,
                    CoordinateStruct{ 0, 0 },            // 暫定
                    from_coordinate_object_struct.object_p,
                    type
                );
            }
            // 橋や踏切を設置する必要がある場合はtypeとdirectionを指定して接続
            else {
                m_objects[object_id]->connect(
                    road_network,
                    CoordinateStruct{ 0, 0 },            // 暫定
                    from_coordinate_object_struct.object_p
                );
            }
            
            // 工事中状態を撤回
            m_constructing_connectable_objects.remove(m_objects[object_id]);
            m_constructing_connectable_objects.remove(from_coordinate_object_struct.object_p);
		}
	}
}

// 踏切を設置（道路と線路が交差していれば）
bool CityMap::putTrainCrossing(CBAddon* addon, CoordinateStruct coordinate, TypeID::Type &type) {
    // 異なる種類の接続可能オブジェクトが存在する場合
    // -> 可能であれば橋や踏切を設置することで交差
    if (addon->isInCategories(CategoryID::Road)) {
        Object *other_type_connectable_object;
        // 道路＆線路 -> 踏切
        if ((other_type_connectable_object = m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Railroad)) != nullptr) {
            type = TypeID::TrainCrossing;
            other_type_connectable_object->setVisible(false);
            return true;
        }
    }
    if (addon->isInCategories(CategoryID::Railroad)) {
        Object *other_type_connectable_object;
        // 線路＆道路 -> 踏切
        if ((other_type_connectable_object = m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Road)) != nullptr) {
            type = TypeID::TrainCrossing;
            other_type_connectable_object->setVisible(false);
            return true;
        }
    }
    
    return false;
}

// 橋を設置（道路/線路と水路が交差していれば）
bool CityMap::putBridge(CBAddon* addon, CoordinateStruct coordinate, TypeID::Type &type) {
    // 異なる種類の接続可能オブジェクトが存在する場合
    // -> 可能であれば橋や踏切を設置することで交差
    if (addon->isInCategories(CategoryID::Road)) {
        Object *other_type_connectable_object;
        // 道路＆水路 -> 橋
        if ((other_type_connectable_object = m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Waterway)) != nullptr) {
            type = TypeID::Bridge;
            
            return true;
        }
    }
    if (addon->isInCategories(CategoryID::Railroad)) {
        Object *other_type_connectable_object;
        // 線路＆水路 -> 橋
        if ((other_type_connectable_object = m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Waterway)) != nullptr) {
            type = TypeID::Bridge;
            
            return true;
        }
    }
    if (addon->isInCategories(CategoryID::Waterway)) {
        Object *other_type_connectable_object;
        // 水路＆道路 -> 橋
        if ((other_type_connectable_object = m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Road)) != nullptr) {
            type = TypeID::Bridge;
            
            return true;
        }
        // 水路＆線路 -> 橋
        if ((other_type_connectable_object = m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Railroad)) != nullptr) {
            type = TypeID::Bridge;
            
            return true;
        }
    }
    
    return false;
}

bool CityMap::canBuildRoadHere(CoordinateStruct coordinate) {
	if (coordinate.x >= 0 && coordinate.y >= 0 && coordinate.x <= m_map_size.x-1 && coordinate.y <= m_map_size.y) {
		return true;
	}
	return false;
}

TypeID::Type CityMap::setRoadType(CoordinateStruct coordinate, CBAddon *addon) {
	// 対象物のカテゴリを取得
	CategoryID::Type object_category = CategoryID::Disabled;
	object_category = getConnectableCategoryID(addon);
	
	// 既に道路が存在するなら、TypeIDはそのまま
	Array<Object*> current_objects = m_tiles[coordinate.y][coordinate.x].getObjectsP(object_category);
	if (current_objects.size() > 0) {
		cout << "type id: " << current_objects[0]->getTypeID() << endl;
		return current_objects[0]->getTypeID();
	}
	
	return TypeID::UnderConstruction;			// 標準で孤立点に
}

DirectionID::Type CityMap::setRoadDirection(CoordinateStruct coordinate, CBAddon* addon) {
	// 対象物のカテゴリを取得
	CategoryID::Type object_category = CategoryID::Disabled;
	object_category = getConnectableCategoryID(addon);
	
	// 既に同じタイプ or 接続できるタイプのアドオンがタイル上に存在するなら、DirectionIDはそのまま
    Object* object;
	if ((object = m_tiles[coordinate.y][coordinate.x].hasCategory(object_category)) != nullptr) {
		return object->getDirectionID();
	}
	
	return DirectionID::None;					// 標準で孤立点に
}

// 交差する方向を取得（踏切・橋用）
DirectionID::Type CityMap::getCrossDirection(DirectionID::Type origin_direction) {
    switch (origin_direction) {
        case DirectionID::EastWest:
            return DirectionID::NorthSouth;
        case DirectionID::NorthSouth:
            return DirectionID::EastWest;
        case DirectionID::East:
        case DirectionID::West:
            return DirectionID::NorthWest;
        case DirectionID::North:
        case DirectionID::South:
            return DirectionID::EastWest;
        default:
            return DirectionID::None;
    }
}

// 道路建設メニューを閉じたとき、どのタイルとも接続されていない道路(線路)は除去
void CityMap::breakUnconnectedRoads() {
    for (auto obj : m_constructing_connectable_objects) {
        breaking(obj->getOriginCoordinate(), false, true, true);
    }
    
    // リストをクリア
    m_constructing_connectable_objects.clear();
}

// 交差可能なオブジェクトが存在するか否か
bool CityMap::m_is_there_crossable_object(CBAddon *addon, CoordinateStruct coordinate) {
    if (addon->isInCategories(CategoryID::Road)) {
        if (m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Railroad) != nullptr) {
            return true;
        }
        if (m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Waterway) != nullptr) {
            return true;
        }
    }
    if (addon->isInCategories(CategoryID::Railroad)) {
        if (m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Road) != nullptr) {
            return true;
        }
        if (m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Waterway) != nullptr) {
            return true;
        }
    }
    if (addon->isInCategories(CategoryID::Waterway)) {
        if (m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Road) != nullptr) {
            return true;
        }
        if (m_tiles[coordinate.y][coordinate.x].hasCategory(CategoryID::Railroad) != nullptr) {
            return true;
        }
    }
    
    return false;
}
