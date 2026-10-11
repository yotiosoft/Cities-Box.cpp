//
//  CityMap_Build.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/26.
//

#include "CityMap.hpp"

// アドオンの設置
bool CityMap::build(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak) {
	m_last_build_error.clear();
	const auto bounds = m_plan_build(cursor.coordinate, selectedAddon, false, TypeID::Disabled, DirectionID::Disabled);
	if (bounds.error_code == 4) {
		m_last_build_error = Unicode::FromUTF8(std::string(bounds.error_message.data(), bounds.error_message.size()));
		return false;
	}
	if (selectedAddon->isInCategories(CategoryID::Connectable)) {
		return m_build_connectable_type(cursor, before_cursor, selectedAddon, needToBreak);
	}
	else {
		return m_build_building(cursor, before_cursor, selectedAddon, needToBreak);
	}
}

rust::citymap::BuildDecision CityMap::m_plan_build(CoordinateStruct coordinate, CBAddon* addon, bool connectable, TypeID::Type type, DirectionID::Type direction, bool crossable) {
	rust::citymap::BuildRequest request;
	request.x = coordinate.x;
	request.y = coordinate.y;
	request.map_width = m_map_size.x;
	request.map_height = m_map_size.y;
	request.connectable = connectable;
	request.put_type = addon->isInCategories(CategoryID::PutType);
	request.crossable = crossable;
	int radius = 1;
	if (connectable) {
		rust::citymap::BuildOption option;
		option.direction = static_cast<int>(direction);
		option.available = addon->isCorrectCondition(type, direction);
		if (option.available) {
			const auto size = addon->getUseTiles(type, direction);
			option.width = size.x;
			option.height = size.y;
			radius = Max(radius, Max(size.x, size.y));
		}
		request.options.push_back(option);
	} else if (!request.put_type) {
		for (int i = 0; i < AROUND_TILES; ++i) {
			rust::citymap::BuildOption option;
			option.direction = static_cast<int>(AroundTiles[i].first);
			option.available = addon->isCorrectCondition(TypeID::Normal, AroundTiles[i].first);
			if (option.available) {
				const auto size = addon->getUseTiles(TypeID::Normal, AroundTiles[i].first);
				option.width = size.x;
				option.height = size.y;
				radius = Max(radius, Max(size.x, size.y));
			}
			request.options.push_back(option);
		}
	}
	for (int y = Max(0, coordinate.y - radius); y <= Min(m_map_size.y - 1, coordinate.y + radius); ++y) {
		for (int x = Max(0, coordinate.x - radius); x <= Min(m_map_size.x - 1, coordinate.x + radius); ++x) {
			rust::citymap::BuildTile tile;
			tile.x = x;
			tile.y = y;
			tile.has_road = false;
			tile.has_structure = false;
			tile.has_connectable = false;
			tile.has_nonconnectable_structure = false;
			for (const auto& entry : m_tiles[y][x].getObjectStructs()) {
				if (!entry.object_p || entry.object_p->isDeleted() || !entry.object_p->getAddonP()) continue;
				auto* existing = entry.object_p->getAddonP();
				tile.has_road |= existing->isInCategories(CategoryID::Road);
				tile.has_connectable |= existing->isInCategories(CategoryID::Connectable);
				tile.has_structure |= !existing->isInCategories(CategoryID::Tile);
				tile.has_nonconnectable_structure |= !existing->isInCategories(CategoryID::Tile)
					&& !existing->isInCategories(CategoryID::Connectable);
			}
			request.tiles.push_back(tile);
		}
	}
	return rust::citymap::plan_build(std::move(request));
}

bool CityMap::m_build_building(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak) {
	// ObjectIDの決定
	int objectID = m_get_next_objectID();
	
	tuple<bool, TypeID::Type, DirectionID::Type> build_tuple = m_can_build_building_here(cursor.coordinate, selectedAddon);
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
		m_set_rate(m_objects[objectID], origin_coordinate, false);

		// 旧版と同じく、建設に成功したときだけ資金を減らす
		m_rust_core->charge_construction_cost();
		return true;
	}

	return false;
}

void CityMap::m_set_rate(Object* arg_object, CoordinateStruct arg_origin_coordinate, bool will_be_deleted) {
    if (arg_object->isDeleted()) {
        return;
    }

	rust::Vec<rust::citymap::TileRateState> tiles;
	for (int y = 0; y < m_map_size.y; ++y) {
		for (int x = 0; x < m_map_size.x; ++x) {
			const Tile& tile = m_tiles[y][x];
			rust::citymap::TileRateState state;
			state.x = x;
			state.y = y;
			state.land_price = tile.rate.count(RateID::LandPrice) ? tile.rate.at(RateID::LandPrice) : 0;
			state.crime_rate = tile.rate.count(RateID::CrimeRate) ? tile.rate.at(RateID::CrimeRate) : 0;
			state.education_rate = tile.rate.count(RateID::EducationRate) ? tile.rate.at(RateID::EducationRate) : 0;
			state.noise_rate = tile.rate.count(RateID::NoiseRate) ? tile.rate.at(RateID::NoiseRate) : 0;
			state.happiness_rate = tile.rate.count(RateID::HappinessRate) ? tile.rate.at(RateID::HappinessRate) : 0;
			tiles.push_back(std::move(state));
		}
	}

	rust::Vec<rust::citymap::RateEffect> effects;
	const auto requiredTiles = arg_object->getAddonDirectionStruct().requiredTiles;
	const int centerX = arg_origin_coordinate.x + requiredTiles.x / 2;
	const int centerY = arg_origin_coordinate.y + requiredTiles.y / 2;
	for (const auto& [rateID, effect] : arg_object->getAddonP()->getEffects()) {
		rust::citymap::RateEffect state;
		state.rate_kind = static_cast<int32_t>(rateID);
		state.influence = effect.influence;
		state.range = effect.grid;
		state.origin_x = centerX;
		state.origin_y = centerY;
		state.will_be_deleted = will_be_deleted;
		effects.push_back(std::move(state));
	}

	const auto updated = m_rust_core->update_rates(
		m_map_size.x,
		m_map_size.y,
		std::move(tiles),
		std::move(effects)
	);
	for (const auto& state : updated) {
		if (state.x < 0 || state.y < 0 || state.x >= m_map_size.x || state.y >= m_map_size.y) {
			continue;
		}
		Tile& tile = m_tiles[state.y][state.x];
		tile.rate[RateID::LandPrice] = state.land_price;
		tile.rate[RateID::CrimeRate] = state.crime_rate;
		tile.rate[RateID::EducationRate] = state.education_rate;
		tile.rate[RateID::NoiseRate] = state.noise_rate;
		tile.rate[RateID::HappinessRate] = state.happiness_rate;
	}
}

void CityMap::breaking(CoordinateStruct coordinate, bool isTemporaryDelete, bool updateAroundTiles, bool deleteThis) {
	// オブジェクトの除去
	for (ObjectStruct object_struct : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
        m_break_once(object_struct, coordinate, isTemporaryDelete, updateAroundTiles, deleteThis);
	}
}
void CityMap::m_break_only_category(CategoryID::Type category, CoordinateStruct coordinate, bool isTemporaryDelete, bool updateAroundTiles, bool deleteThis) {
    // オブジェクトの除去
    for (ObjectStruct object_struct : m_tiles[coordinate.y][coordinate.x].getObjectStructs()) {
        // 周囲タイルの除去からの呼び出し時、アドオンが存在しない状態での削除は避ける
        if (object_struct.object_p->isDeleted()) {
            //Size delete_object_required_tiles = object_struct.object_p->getAddonDirectionStruct().requiredTiles;
            //for (int y = object_struct.relative_coordinate.origin.y; y < object_struct.relative_coordinate.origin.y + delete_object_required_tiles.y; y++) {
                //for (int x = object_struct.relative_coordinate.origin.x; x < object_struct.relative_coordinate.origin.x + delete_object_required_tiles.x; x++) {
                    // オブジェクトの削除
                    m_tiles[coordinate.y][coordinate.x].deleteObject(object_struct.object_p);
                    
                    // 草地タイルの設置
                    m_put_grass(coordinate);
                //}
            //}
            continue;
        }
        
        // 除去処理
        if (object_struct.object_p->getAddonP()->isInCategories(category)) {
            m_break_once(object_struct, coordinate, isTemporaryDelete, updateAroundTiles, deleteThis);
        }
    }
}
void CityMap::m_break_once(ObjectStruct &object_struct, CoordinateStruct coordinate, bool isTemporaryDelete, bool updateAroundTiles, bool deleteThis) {
    Object* object = object_struct.object_p;
    if (object == nullptr || object->isDeleted() || object->getAddonP() == nullptr) {
        if (object != nullptr) {
            m_tiles[coordinate.y][coordinate.x].deleteObject(object);
        }
        return;
    }

    // 効果を削除
    m_set_rate(object, object_struct.relative_coordinate.origin, true);

    Array<CoordinateStruct> need_to_del_list;
    Array<CategoryID::Type> category_ids = object->getAddonP()->getCategories();
    const bool is_common_object = object->isCommonObject();
	if (deleteThis && category_ids.contains(CategoryID::Connectable)) {
		m_rust_core->remove_connectable_node(object->getObjectID());
	}

    Size delete_object_required_tiles = object->getAddonDirectionStruct().requiredTiles;
    for (int y = object_struct.relative_coordinate.origin.y; y < object_struct.relative_coordinate.origin.y + delete_object_required_tiles.y; y++) {
        for (int x = object_struct.relative_coordinate.origin.x; x < object_struct.relative_coordinate.origin.x + delete_object_required_tiles.x; x++) {
            // クリア処理

			// 周囲の道路の向きを修正
            if (updateAroundTiles) {
                need_to_del_list.append(m_update_connected_tiles(CoordinateStruct{x, y}));
			}

			// オブジェクトをタイルから削除
            Console << U"call deleteObject: " << x << U", " << y;
            m_tiles[y][x].deleteObject(object);

            // 更地になったら芝生を置く
            // 要修正 : 共通の動作は一つの関数にまとめること
            if (!isTemporaryDelete && m_tiles[y][x].getObjectStructs().size() == 0) {
                UnitaryTools::debugLog(U"before put");
                m_put_grass(CoordinateStruct{ x, y });
                UnitaryTools::debugLog(U"after put");
            }
        }
    }

    // 非共通オブジェクトは、形状変更後のサイズに依存せず全タイルから参照を外す。
    // 実体は参照除去後、CityMap::draw() の末尾で一度だけ解放する。
    if (!is_common_object) {
        for (int y = 0; y < m_map_size.y; ++y) {
            for (int x = 0; x < m_map_size.x; ++x) {
                while (m_tiles[y][x].deleteObject(object)) {
                }
            }
        }
    }

    // クリア処理
	if (deleteThis && !is_common_object && !object->isDeleted()) {
		object->setDeleted();
	}
    
    // 必要に応じて周囲タイルも削除（周囲タイルにも影響する場合）
    // タイルが孤立している場合など
    if (updateAroundTiles) {
        for (auto coordinate : need_to_del_list) {
            for (auto category_id : category_ids) {
                m_break_only_category(category_id, coordinate, isTemporaryDelete, updateAroundTiles, deleteThis);
            }
        }
    }
}

// 建物の建設の可否
tuple<bool, TypeID::Type, DirectionID::Type> CityMap::m_can_build_building_here(CoordinateStruct coordinate, CBAddon* addon) {
	if (addon->isInCategories(CategoryID::PutType)) {
		return { true, TypeID::Normal, DirectionID::None };
	}
	const auto decision = m_plan_build(coordinate, addon, false, TypeID::Normal, DirectionID::Disabled);
	if (!decision.allowed) {
		m_last_build_error = Unicode::FromUTF8(std::string(decision.error_message.data(), decision.error_message.size()));
		return { false, TypeID::Disabled, DirectionID::Disabled };
	}
	return { true, TypeID::Normal, static_cast<DirectionID::Type>(decision.direction) };
}

// 接続している周囲のタイルの向きを更新する
// 要修正 : タイルを直接弄らないこと
Array<CoordinateStruct> CityMap::m_update_connected_tiles(CoordinateStruct position) {
	Tile* currentTile = &m_tiles[position.y][position.x];
	
	// タイル状のConnectableオブジェクトを削除
	Array<CoordinateStruct> ret;
	for (auto current_object : currentTile->getObjectsP(CategoryID::Connectable)) {
		ret = current_object->del();		// ここで周囲を更新（戻り値：他に削除が必要な周囲タイルのリスト）
	}

	return ret;
}

void CityMap::m_put_grass(CoordinateStruct arg_coordinate) {
    auto common_object = m_common_objects.find(m_addons[U"tile_greenfield"]->getName(NameMode::English));
    if (common_object != m_common_objects.end()) {
        RelativeCoordinateStruct relative_coordinate;
        relative_coordinate.origin = arg_coordinate;
        relative_coordinate.relative = CoordinateStruct{ 0, 0 };
        
        cout << "put grass: " << common_object->second->getObjectID() << endl;
        
        m_tiles[arg_coordinate.y][arg_coordinate.x].addObject(common_object->second, relative_coordinate);
        
        return;
    }
    
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
