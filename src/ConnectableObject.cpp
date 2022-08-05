//
//  ConnectableObject.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/06/18.
//

#include "ConnectableObject.hpp"

void ConnectableObject::connect(CityNetwork& road_network, CoordinateStruct arg_connect_coordinate, Object *arg_object_p) {
	// マップ上で接続
	DirectionID::Type relative_direction_id = UnitaryTools::getDirectionIDfromDifference(m_start_coordinate + arg_connect_coordinate, arg_object_p->getOriginCoordinate());
	set_direction_id(relative_direction_id, false);
    if (m_type_id != TypeID::TrainCrossing && m_type_id != TypeID::Bridge)
        set_type_id();
	
	m_connects[arg_connect_coordinate.y][arg_connect_coordinate.x].roadTypeConnect << pair<DirectionID::Type, Object*>{relative_direction_id, arg_object_p};
	cout << "set roadtypeconnect " << m_direction_id << " / " << m_type_id << endl;
}

void ConnectableObject::connectWithSpecifiedType(CityNetwork& road_network, CoordinateStruct arg_connect_coordinate, Object *arg_object_p, TypeID::Type type) {
    // マップ上で接続
    DirectionID::Type relative_direction_id = UnitaryTools::getDirectionIDfromDifference(m_start_coordinate + arg_connect_coordinate, arg_object_p->getOriginCoordinate());
    set_direction_id(relative_direction_id, false);
    m_type_id = type;
    
    m_connects[arg_connect_coordinate.y][arg_connect_coordinate.x].roadTypeConnect << pair<DirectionID::Type, Object*>{relative_direction_id, arg_object_p};
    cout << "set roadtypeconnect " << m_direction_id << " / " << m_type_id << endl;
}

void ConnectableObject::del(CityNetwork& road_network) {
	// arg_connect_coordinateのオブジェクトと繋がっているか確認
	// 繋がっていたら切断する
	setDeleted();
	
	for (auto connects : m_connects) {
		for (auto connect : connects) {
			for (auto road_type_connect : connect.roadTypeConnect) {
				// 接続先を更新：自分への接続を解除
				// すでにそのオブジェクトが存在しなければスルー
				if (road_type_connect.first == DirectionID::Disabled) {
					continue;
				}

				cout << "update at " << road_type_connect.second->getOriginCoordinate().x << "," << road_type_connect.second->getOriginCoordinate().y << endl;
				road_type_connect.second->update();
				/*
				if (road_type_connect.second->isOn(arg_connect_coordinate)) {
					set_direction_id(UnitaryTools::getDirectionIDfromDifference(m_start_coordinate + arg_connect_coordinate, arg_object_p->getOriginCoordinate()), true);
					set_type_id();
					
					
				}*/

				// 切断後、接続先の向きが無効になっていたら接続先を削除
				if (road_type_connect.first == DirectionID::Disabled) {
					road_type_connect.second->del(road_network);
				}
			}
		}
	}
}

void ConnectableObject::update() {
	for (int y=0; y<(int)m_connects.size(); y++) {
		for (int x=0; x<(int)m_connects[y].size(); x++) {
			for (auto it = m_connects[y][x].roadTypeConnect.begin(), e = m_connects[y][x].roadTypeConnect.end(); it != e; it++) {
				// 接続先の道路が存在していなければ、切断する
				if (it->second->isDeleted()) {
					cout << "update: delete for " << x << "," << y << endl;
					//CoordinateStruct abs_coordinate = m_start_coordinate + CoordinateStruct{x, y};
					
					set_direction_id(it->first, true);
					set_type_id();
				}
			}
			
			m_connects[y][x].roadTypeConnect.remove_if([](pair<DirectionID::Type, Object*> v) { return v.second->isDeleted(); });
		}
	}
}

void ConnectableObject::set_direction_id(DirectionID::Type arg_direction, bool is_deleted) {
	if (is_deleted) {	// 削除時
		m_direction_id = UnitaryTools::subDirectionID(m_direction_id, arg_direction);
	}
	else {				// 接続時
		m_direction_id = UnitaryTools::addDirectionID(m_direction_id, arg_direction);
	}
	
	return;
}

void ConnectableObject::set_type_id() {
	m_type_id = get_type_id(m_direction_id);
}

TypeID::Type ConnectableObject::get_type_id(DirectionID::Type arg_direction) {
	switch (arg_direction) {
		case DirectionID::None:
			return TypeID::Normal;
			
		case DirectionID::North:
		case DirectionID::South:
		case DirectionID::East:
		case DirectionID::West:
			return TypeID::DeadEnd;
			
		case DirectionID::EastWest:
		case DirectionID::NorthSouth:
			return TypeID::Default;
			
		case DirectionID::SouthWest:
		case DirectionID::NorthWest:
		case DirectionID::SouthEast:
		case DirectionID::NorthEast:
			return TypeID::Turn;
			
		case DirectionID::SouthEastWest:
		case DirectionID::NorthEastWest:
		case DirectionID::NorthSouthWest:
		case DirectionID::NorthSouthEast:
			return TypeID::IntersectionT;
			
		case DirectionID::All:
			return TypeID::IntersectionCross;
			
		case DirectionID::Offshore:
			return TypeID::WaterOffshore;
			
		case DirectionID::WithoutSouthwestNorthwest:
		case DirectionID::WithoutNortheastNorthwest:
		case DirectionID::WithoutSoutheastSouthwest:
		case DirectionID::WithoutNortheastSoutheast:
			return TypeID::WaterEstuary;
			
		case DirectionID::WithoutEast:
		case DirectionID::WithoutSouth:
		case DirectionID::WithoutNorth:
		case DirectionID::WithoutWest:
			return TypeID::WaterIntersectionT;
			
		case DirectionID::WithoutNorthWestNorthwest:
		case DirectionID::WithoutNorthNortheastEast:
		case DirectionID::WithoutSouthSouthwestWest:
		case DirectionID::WithoutEastSoutheastSouth:
			return TypeID::WaterTurn;
			
		case DirectionID::WithoutNorthwest:
		case DirectionID::WithoutNortheast:
		case DirectionID::WithoutSouthwest:
		case DirectionID::WithoutSoutheast:
			return TypeID::WaterIntersectionCrossWithoutOneCorner;
			
		default:
			return TypeID::Disabled;
	}
}

TypeID::Type ConnectableObject::get_type_id_waterway(DirectionID::Type arg_direction) {
	switch (arg_direction) {
		case DirectionID::None:
			return TypeID::UnderConstruction;
			
		case DirectionID::North:
		case DirectionID::South:
		case DirectionID::East:
		case DirectionID::West:
			return TypeID::DeadEnd;
			
		case DirectionID::EastWest:
		case DirectionID::NorthSouth:
			return TypeID::Default;
			
		case DirectionID::SouthWest:
		case DirectionID::NorthWest:
		case DirectionID::SouthEast:
		case DirectionID::NorthEast:
			return TypeID::Turn;
			
		case DirectionID::SouthEastWest:
		case DirectionID::NorthEastWest:
		case DirectionID::NorthSouthWest:
		case DirectionID::NorthSouthEast:
			return TypeID::IntersectionT;
			
		case DirectionID::All:
			return TypeID::IntersectionCross;
			
		default:
			return TypeID::Disabled;
	}
}

// デストラクタ
ConnectableObject::~ConnectableObject() {
	
}
