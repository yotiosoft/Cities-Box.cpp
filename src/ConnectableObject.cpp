//
//  ConnectableObject.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/06/18.
//

#include "ConnectableObject.hpp"

void ConnectableObject::connect(CityNetwork& road_network, CoordinateStruct arg_connect_coordinate, Object *arg_object_p) {
	// arg_connect_coordinateのオブジェクトと繋げられるか確認
	// 繋げられるならつなげる
	
	if ((arg_object_p->getAddonP()->isInCategories(CategoryID::Road) && m_addon_p->isInCategories(CategoryID::Road)) ||
		(arg_object_p->getAddonP()->isInCategories(CategoryID::Railroad) && m_addon_p->isInCategories(CategoryID::Railroad)) ||
		(arg_object_p->getAddonP()->isInCategories(CategoryID::Waterway) && m_addon_p->isInCategories(CategoryID::Waterway)) ||
		(arg_object_p->getAddonP()->isInCategories(CategoryID::Airport) && m_addon_p->isInCategories(CategoryID::Airport))) {
		
		// マップ上で接続
		DirectionID::Type relative_direction_id = UnitaryTools::getDirectionIDfromDifference(m_start_coordinate + arg_connect_coordinate, arg_object_p->getOriginCoordinate());
		set_direction_id(relative_direction_id, false);
		
		set_type_id();
		
		m_connects[arg_connect_coordinate.y][arg_connect_coordinate.x].roadTypeConnect << pair<DirectionID::Type, Object*>{relative_direction_id, arg_object_p};
		cout << "set roadtypeconnect" << endl;
	}
}

void ConnectableObject::del(CityNetwork& road_network) {
	// arg_connect_coordinateのオブジェクトと繋がっているか確認
	// 繋がっていたら切断する
	setDeleted();
	
	cout << "m_connects size: " << m_connects.size() << endl;
	for (auto connects : m_connects) {
		for (auto connect : connects) {
			cout << "size: " << connect.roadTypeConnect.size() << endl;
			for (auto road_type_connect : connect.roadTypeConnect) {
				// 接続先を更新：自分への接続を解除
				cout << "update at " << road_type_connect.second->getOriginCoordinate().x << "," << road_type_connect.second->getOriginCoordinate().y << endl;
				road_type_connect.second->update();
				/*
				if (road_type_connect.second->isOn(arg_connect_coordinate)) {
					set_direction_id(UnitaryTools::getDirectionIDfromDifference(m_start_coordinate + arg_connect_coordinate, arg_object_p->getOriginCoordinate()), true);
					set_type_id();
					
					
				}*/
			}
		}
	}
}

void ConnectableObject::update() {
	for (int y=0; y<m_connects.size(); y++) {
		for (int x=0; x<m_connects[y].size(); x++) {
			for (auto it = m_connects[y][x].roadTypeConnect.begin(), e = m_connects[y][x].roadTypeConnect.end(); it != e; it++) {
				// 接続先の道路が存在していなければ、切断する
				if (it->second->isDeleted()) {
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
			
		default:
			return TypeID::Disabled;
	}
}

// デストラクタ
ConnectableObject::~ConnectableObject() {
	
}
