//
//  ConnectableObject.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/06/18.
//

#include "ConnectableObject.hpp"
#include <rs-citymap.h>

namespace {
	rust::citymap::ConnectableConnectionDecision planConnectionInRust(
		CoordinateStruct from,
		CoordinateStruct to,
		DirectionID::Type currentDirection,
		TypeID::Type currentType,
		bool allowDiagonal,
		bool fromHere,
		bool connectionSlotOccupied,
		bool forceType,
		TypeID::Type forcedType
	) {
		rust::citymap::ConnectableConnectionRequest request;
		request.from_x = from.x;
		request.from_y = from.y;
		request.to_x = to.x;
		request.to_y = to.y;
		request.current_direction = static_cast<int>(currentDirection);
		request.current_type = static_cast<int>(currentType);
		request.allow_diagonal = allowDiagonal;
		request.from_here = fromHere;
		request.connection_slot_occupied = connectionSlotOccupied;
		request.force_type = forceType;
		request.forced_type = static_cast<int>(forcedType);
		return rust::citymap::plan_connectable_connection(request);
	}
}

void ConnectableObject::connect(CoordinateStruct arg_connect_coordinate, Object *arg_object_p, bool from_here) {
	const auto from = m_start_coordinate + arg_connect_coordinate;
	const auto to = arg_object_p->getOriginCoordinate();
	const auto decision = planConnectionInRust(
		from,
		to,
		m_direction_id,
		m_type_id,
		m_addon_p->isInCategories(CategoryID::Waterway),
		from_here,
		!m_connects[arg_connect_coordinate.y][arg_connect_coordinate.x].roadTypeConnect.empty(),
		false,
		TypeID::Disabled
	);

	if (decision.status == rust::citymap::ConnectableConnectionStatus::AlreadyConnected) {
		return;
	}

	// Directionが無効なら自身のオブジェクトを削除し終了
	if (decision.status == rust::citymap::ConnectableConnectionStatus::InvalidDirection) {
		UnitaryTools::debugLog(U"connect", arg_connect_coordinate, U"Direction disabled");
		m_addon_p = nullptr;
		setDeleted();
		return;
	}

	m_direction_id = static_cast<DirectionID::Type>(decision.updated_direction);
	m_type_id = static_cast<TypeID::Type>(decision.updated_type);
	
	m_connects[arg_connect_coordinate.y][arg_connect_coordinate.x].roadTypeConnect << pair<DirectionID::Type, Object*>{static_cast<DirectionID::Type>(decision.relative_direction), arg_object_p};
	UnitaryTools::debugLog(U"connect", U"set roadtypeconect " + Format(m_direction_id) + U" / " + Format(m_type_id));
}

void ConnectableObject::connectWithSpecifiedType(CoordinateStruct arg_connect_coordinate, Object *arg_object_p, TypeID::Type type, bool from_here) {
	const auto from = m_start_coordinate + arg_connect_coordinate;
	const auto to = arg_object_p->getOriginCoordinate();
	const auto decision = planConnectionInRust(
		from,
		to,
		m_direction_id,
		m_type_id,
		m_addon_p->isInCategories(CategoryID::Waterway),
		from_here,
		!m_connects[arg_connect_coordinate.y][arg_connect_coordinate.x].roadTypeConnect.empty(),
		true,
		type
	);

	if (decision.status == rust::citymap::ConnectableConnectionStatus::AlreadyConnected) {
		return;
	}

	// Directionが無効なら自身のオブジェクトを削除し終了
	if (decision.status == rust::citymap::ConnectableConnectionStatus::InvalidDirection) {
		UnitaryTools::debugLog(U"connectWithSpecifiedType", arg_connect_coordinate, U"Direction disabled");
		m_addon_p = nullptr;
		setDeleted();
		return;
	}
	
	m_direction_id = static_cast<DirectionID::Type>(decision.updated_direction);
	m_type_id = static_cast<TypeID::Type>(decision.updated_type);
    
	m_connects[arg_connect_coordinate.y][arg_connect_coordinate.x].roadTypeConnect << pair<DirectionID::Type, Object*>{static_cast<DirectionID::Type>(decision.relative_direction), arg_object_p};
	UnitaryTools::debugLog(U"connectWithSpecifiedType", U"set roadtypeconect " + Format(m_direction_id) + U" / " + Format(m_type_id));
}

Array<CoordinateStruct> ConnectableObject::del() {
	Array<CoordinateStruct> ret;	// 切断後、削除必須な周囲タイルのリスト

	// arg_connect_coordinateのオブジェクトと繋がっているか確認
	// 繋がっていたら切断する
    
	setDeleted();   // 自分自身を削除状態に設定
    
	for (auto connects : m_connects) {
		for (auto connect : connects) {
			for (auto road_type_connect : connect.roadTypeConnect) {
				// 接続先を更新：自分への接続を解除
				// すでにそのオブジェクトが存在しなければスルー
				if (road_type_connect.first == DirectionID::Disabled) {
					continue;
				}

				cout << "update at " << road_type_connect.second->getOriginCoordinate().x << "," << road_type_connect.second->getOriginCoordinate().y << endl;
				road_type_connect.second->update();     // 自分の削除後に相手のタイルを更新

				// 切断後、接続先の向きが無効になっていたら接続先を削除
				DirectionID::Type after_del_direction_id = road_type_connect.second->getDirectionID();
				if (after_del_direction_id == DirectionID::Disabled || after_del_direction_id == DirectionID::None) {
					ret.push_back(road_type_connect.second->getOriginCoordinate());
				}
			}
		}
	}

	return ret;
}

void ConnectableObject::update() {
	for (int y=0; y<(int)m_connects.size(); y++) {
		for (int x=0; x<(int)m_connects[y].size(); x++) {
			for (auto it = m_connects[y][x].roadTypeConnect.begin(), e = m_connects[y][x].roadTypeConnect.end(); it != e; it++) {
				// 接続先の道路が存在していなければ(deleted)、切断する
				if (it->second->isDeleted()) {
					cout << "update: delete for " << x << "," << y << endl;
					//CoordinateStruct abs_coordinate = m_start_coordinate + CoordinateStruct{x, y};
					
					const auto decision = rust::citymap::plan_connectable_removal(
						static_cast<int>(m_direction_id),
						static_cast<int>(it->first)
					);
					m_direction_id = static_cast<DirectionID::Type>(decision.updated_direction);
					m_type_id = static_cast<TypeID::Type>(decision.updated_type);
                    
                    // 既に自身が無効なオブジェクトならdeleted状態に
                    if (m_addon_p == nullptr) {
                        setDeleted();
                        break;
                    }
                    
                    // アップデート時、正しくない向きになっていたらaddon_pを削除
                    if (!m_addon_p->isCorrectCondition(m_type_id, m_direction_id)) {
                        m_addon_p = nullptr;
                        setDeleted();
                        break;
                    }
				}
			}
			
			m_connects[y][x].roadTypeConnect.remove_if([](pair<DirectionID::Type, Object*> v) { return v.second->isDeleted(); });
		}
	}
}

// デストラクタ
ConnectableObject::~ConnectableObject() {
	
}
