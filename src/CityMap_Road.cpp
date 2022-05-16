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
		TypeID::Type type = setRoadType(cursor.coordinate, selectedAddon);
		DirectionID::Type direction = setRoadDirection(cursor.coordinate, selectedAddon);
		
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
				breaking(CoordinateStruct{ x, y }, true, disconnect, disconnect);
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
        
        // 建設開始
        m_constructings[origin_coordinate.y][origin_coordinate.x] = 1;

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
	
	// ConnectableTypeの場合 -> カーソルが移動前の座標から連続して押し続けて移動していれば、そのタイルと接続する
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
			from_coordinate_object_struct.object_p->connect(
				road_network,
				CoordinateStruct{ 0, 0 },			// 暫定
				m_objects[object_id]
			);

			m_objects[object_id]->connect(
				road_network,
				CoordinateStruct{ 0, 0 },			// 暫定
				from_coordinate_object_struct.object_p
			);
			
			if (m_constructings[from.y][from.x] > 0) {
				m_constructings[from.y][from.x] = 0;
			}
			if (m_constructings[to.y][to.x] > 0) {
				m_constructings[to.y][to.x] = 0;
			}
            
            // 工事中状態を撤回
            m_constructing_connectable_objects.remove(m_objects[object_id]);
            m_constructing_connectable_objects.remove(from_coordinate_object_struct.object_p);
		}
	}
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
	
	// 既に道路が存在するなら、DirectionIDはそのまま
	cout << "oc: " << object_category << endl;
	Array<Object*> current_objects = m_tiles[coordinate.y][coordinate.x].getObjectsP(object_category);
	
	if (current_objects.size() > 0) {
		cout << "direction id: " << current_objects[0]->getDirectionID() << endl;
		return current_objects[0]->getDirectionID();
	}
	
	return DirectionID::None;					// 標準で孤立点に
}

// 道路建設メニューを閉じたとき、どのタイルとも接続されていない道路(線路)は除去
void CityMap::breakUnconnectedRoads() {
    for (auto obj : m_constructing_connectable_objects) {
        breaking(obj->getOriginCoordinate(), false, true, true);
    }
}
