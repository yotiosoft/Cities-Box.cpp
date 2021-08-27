//
//  CityMap_Road.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/27.
//

#include "CityMap.hpp"

bool CityMap::buildConnectableType(CursorStruct cursor, CursorStruct before_cursor, Addon* selectedAddon, bool needToBreak) {
	// ObjectIDの決定
	int objectID = m_get_next_objectID();

	if (canBuildRoadHere(cursor.coordinate)) {			// 設置可能なら...
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

		// オブジェクトの生成
		m_objects[objectID] = new ConnectableObject(objectID, selectedAddon, U"", type, direction, origin_coordinate);

		// 建設するタイル上の既存のオブジェクトを削除
		for (int y = origin_coordinate.y; y < origin_coordinate.y + useTiles.y; y++) {
			for (int x = origin_coordinate.x; x < origin_coordinate.x + useTiles.x; x++) {
				// もともとのアドオンと同種のアドオンなら：周囲の切断は行わない
				bool unconnect = true;
				for (auto object_p : m_tiles[y][x].getObjectsP(CategoryID::Connectable)) {
					if (object_p->getAddonP()->isInCategories(getConnectableCategoryID(selectedAddon))) {
						unconnect = false;
					}
				}
				
				breaking(CoordinateStruct{ x, y }, true, unconnect);
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
		
		// ConnectableTypeの場合 -> カーソルが移動前の座標から連続して押し続けて移動していれば、そのタイルと接続する
		if (before_cursor.pressed && cursor.coordinate != before_cursor.coordinate) {
			connectObjects(before_cursor.coordinate, cursor.coordinate, objectID);;
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

void CityMap::connectObjects(CoordinateStruct from, CoordinateStruct to, int object_id) {
	for (auto from_coordinate_object_struct : m_tiles[from.y][from.x].getObjectStructs()) {
		if (from_coordinate_object_struct.object_p->getAddonP()->isInCategories(CategoryID::Road) && m_objects[object_id]->getAddonP()->isInCategories(CategoryID::Road)) {
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
		}
	}
}

bool CityMap::canBuildRoadHere(CoordinateStruct coordinate) {
	if (coordinate.x >= 0 && coordinate.y >= 0 && coordinate.x <= m_map_size.x-1 && coordinate.y <= m_map_size.y) {
		return true;
	}
	return false;
}

TypeID::Type CityMap::setRoadType(CoordinateStruct coordinate, Addon *addon) {
	// 対象物のカテゴリを取得
	CategoryID::Type object_category = CategoryID::Disabled;
	Array<CategoryID::Type> object_categories = addon->getCategories();
	for (auto object_category_single : object_categories) {
		if (object_category_single == CategoryID::Road || object_category_single == CategoryID::Railroad || object_category_single == CategoryID::Waterway || object_category_single == CategoryID::Taxiway || object_category_single == CategoryID::Runway) {
			object_category = object_category_single;
			break;
		}
	}
	
	// 既に道路が存在するなら、TypeIDはそのまま
	cout << "oc: " << object_category << endl;
	Array<Object*> current_objects = m_tiles[coordinate.y][coordinate.x].getObjectsP(object_category);
	
	if (current_objects.size() > 0) {
		cout << "exist a " << endl;
		return current_objects[0]->getTypeID();
	}
	
	return TypeID::UnderConstruction;			// 標準で孤立点に
}

DirectionID::Type CityMap::setRoadDirection(CoordinateStruct coordinate, Addon* addon) {
	// 対象物のカテゴリを取得
	CategoryID::Type object_category = CategoryID::Disabled;
	Array<CategoryID::Type> object_categories = addon->getCategories();
	for (auto object_category_single : object_categories) {
		if (object_category_single == CategoryID::Road || object_category_single == CategoryID::Railroad || object_category_single == CategoryID::Waterway || object_category_single == CategoryID::Taxiway || object_category_single == CategoryID::Runway) {
			object_category = object_category_single;
			break;
		}
	}
	
	// 既に道路が存在するなら、TypeIDはそのまま
	cout << "oc: " << object_category << endl;
	Array<Object*> current_objects = m_tiles[coordinate.y][coordinate.x].getObjectsP(object_category);
	
	if (current_objects.size() > 0) {
		cout << "exist" << endl;
		return current_objects[0]->getDirectionID();
	}
	
	return DirectionID::None;					// 標準で孤立点に
}
