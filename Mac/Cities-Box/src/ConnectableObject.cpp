//
//  ConnectableObject.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/06/18.
//

#include "ConnectableObject.hpp"

void ConnectableObject::connect(CoordinateStruct arg_connect_coordinate, Object *arg_object_p) {
	// arg_connect_coordinateのオブジェクトと繋げられるか確認
	// 繋げられるならつなげる
	
	if ((arg_object_p->getAddonP()->isInCategories(U"road") && m_addon_p->isInCategories(U"road")) ||
		(arg_object_p->getAddonP()->isInCategories(U"railroad") && m_addon_p->isInCategories(U"railroad")) ||
		(arg_object_p->getAddonP()->isInCategories(U"waterway") && m_addon_p->isInCategories(U"waterway")) ||
		(arg_object_p->getAddonP()->isInCategories(U"airport") && m_addon_p->isInCategories(U"airport"))) {
		
		set_direction_id(UnitaryTools::getDirectionIDfromDifference(arg_connect_coordinate, m_start_coordinate), false);
		set_type_id();
	}
}

void ConnectableObject::disconnect(CoordinateStruct arg_connect_coordinate, Object *arg_object_p) {
	// arg_connect_coordinateのオブジェクトと繋がっているか確認
	// 繋がっていたら切断する
	
	for (auto connects : m_connects) {
		for (auto connect : connects) {
			for (auto road_type_connect : connect.roadTypeConnect) {
				if (road_type_connect.second->isOn(arg_connect_coordinate)) {
					set_direction_id(UnitaryTools::getDirectionIDfromDifference(arg_connect_coordinate, m_start_coordinate), true);
					set_type_id();
				}
			}
		}
	}
}

void ConnectableObject::set_direction_id(DirectionID::Type arg_direction, bool is_deleted) {
	int direction_id_int;
	if (is_deleted) {	// 削除時
		direction_id_int = (int)m_direction_id - (int)arg_direction;
	}
	else {				// 接続時
		direction_id_int = (int)m_direction_id + (int)arg_direction;
	}
	m_direction_id = (DirectionID::Type)direction_id_int;
	
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
