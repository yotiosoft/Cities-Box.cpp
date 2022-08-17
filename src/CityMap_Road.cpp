//
//  CityMap_Road.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/27.
//

#include "CityMap.hpp"

bool CityMap::m_build_connectable_type(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak) {
	cout << "into build connect type at " << cursor.coordinate.x << "," << cursor.coordinate.y << endl;
	
	// 同じアドオンが指定されたタイル上に存在するなら -> アドオンの変更は行わず、既存のアドオンのDirectionIDとTypeIDを変更
	if (m_tiles[cursor.coordinate.y][cursor.coordinate.x].hasAddon(selectedAddon)) {
		return m_update_connection_type(cursor, before_cursor, selectedAddon, needToBreak);
	}
	
	// ObjectIDの決定
	int objectID = m_get_next_objectID();

	if (m_can_build_road_here(cursor.coordinate)) {			// 設置可能なら...
        TypeID::Type type;
        DirectionID::Type direction;
        bool do_not_update_selected_addon_type = m_is_there_crossable_object(selectedAddon, cursor.coordinate);
        
        // typeとdirectionを設置
        type = m_set_road_type(cursor.coordinate, selectedAddon);
        direction = m_set_road_direction(cursor.coordinate, selectedAddon);
		
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
                    if (object_p->getAddonP()->isInCategories(m_get_connectable_CategoryID(selectedAddon))) {
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
                    m_break_only_category(connectable_category, CoordinateStruct{ x, y }, true, false, true);
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
			m_connect_objects(before_cursor.coordinate, cursor.coordinate, objectID);
		}

		// 効果を反映
		m_set_rate(m_objects[objectID], origin_coordinate, false);
	}
    

	return true;
}

bool CityMap::m_update_connection_type(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak) {
	// ObjectIDの決定
	cout << "update connection type at " << cursor.coordinate.x << "," << cursor.coordinate.y << endl;
	Object* object = m_tiles[cursor.coordinate.y][cursor.coordinate.x].getObjectP(selectedAddon->getName(NameMode::English), NameMode::English);

	TypeID::Type type = m_set_road_type(cursor.coordinate, selectedAddon);
	DirectionID::Type direction = m_set_road_direction(cursor.coordinate, selectedAddon);
	
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
		m_connect_objects(before_cursor.coordinate, cursor.coordinate, object->getObjectID());
	}

	// 効果を反映
	m_set_rate(object, origin_coordinate, false);
	
	return true;
}

void CityMap::m_connect_objects(CoordinateStruct from, CoordinateStruct to, int object_id) {
	for (auto from_coordinate_object_struct : m_tiles[from.y][from.x].getObjectStructs()) {
		if (from_coordinate_object_struct.object_p->getAddonP()->canConnect(m_objects[object_id]->getAddonP())) {
            from_coordinate_object_struct.object_p->connect(
                road_network,
                CoordinateStruct{ 0, 0 },            // ToDo 暫定
                m_objects[object_id]
            );
            
            // 橋や踏切を設置する必要があるか否か？
            bool other_crossable_object = false;
            TypeID::Type type = TypeID::Disabled;
            if (!(other_crossable_object = m_put_train_crossing(m_objects[object_id]->getAddonP(), to, type))) {
                other_crossable_object = m_put_bridge(m_objects[object_id]->getAddonP(), to, type);
            }
            
            // 橋や踏切を設置する必要がない場合は通常通り接続
            if (other_crossable_object) {
                m_objects[object_id]->connectWithSpecifiedType(
                    road_network,
                    CoordinateStruct{ 0, 0 },            // ToDo 暫定
                    from_coordinate_object_struct.object_p,
                    type
                );
            }
            // 橋や踏切を設置する必要がある場合はtypeとdirectionを指定して接続
            else {
                m_objects[object_id]->connect(
                    road_network,
                    CoordinateStruct{ 0, 0 },            // ToDo 暫定
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
bool CityMap::m_put_train_crossing(CBAddon* addon, CoordinateStruct coordinate, TypeID::Type &type) {
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
bool CityMap::m_put_bridge(CBAddon* addon, CoordinateStruct coordinate, TypeID::Type &type) {
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

bool CityMap::m_can_build_road_here(CoordinateStruct coordinate) {
	if (coordinate.x >= 0 && coordinate.y >= 0 && coordinate.x <= m_map_size.x-1 && coordinate.y <= m_map_size.y) {
		return true;
	}
	return false;
}

TypeID::Type CityMap::m_set_road_type(CoordinateStruct coordinate, CBAddon *addon) {
	// 対象物のカテゴリを取得
	CategoryID::Type object_category = CategoryID::Disabled;
	object_category = m_get_connectable_CategoryID(addon);
	
	// 既に道路が存在するなら、TypeIDはそのまま
	Array<Object*> current_objects = m_tiles[coordinate.y][coordinate.x].getObjectsP(object_category);
	if (current_objects.size() > 0) {
		cout << "type id: " << current_objects[0]->getTypeID() << endl;
		return current_objects[0]->getTypeID();
	}
	
	return TypeID::UnderConstruction;			// 標準で孤立点に
}

DirectionID::Type CityMap::m_set_road_direction(CoordinateStruct coordinate, CBAddon* addon) {
	// 対象物のカテゴリを取得
	CategoryID::Type object_category = CategoryID::Disabled;
	object_category = m_get_connectable_CategoryID(addon);
	
	// 既に同じタイプ or 接続できるタイプのアドオンがタイル上に存在するなら、DirectionIDはそのまま
    Object* object;
	if ((object = m_tiles[coordinate.y][coordinate.x].hasCategory(object_category)) != nullptr) {
		return object->getDirectionID();
	}
	
	return DirectionID::None;					// 標準で孤立点に
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
