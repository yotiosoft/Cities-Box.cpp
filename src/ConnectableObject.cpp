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
		set_direction_id(UnitaryTools::getDirectionIDfromDifference(m_start_coordinate, arg_object_p->getOriginCoordinate() + arg_connect_coordinate), false);
		
		set_type_id();
	}
}

void ConnectableObject::disconnect(CityNetwork& road_network, CoordinateStruct arg_connect_coordinate, Object *arg_object_p) {
	// arg_connect_coordinateのオブジェクトと繋がっているか確認
	// 繋がっていたら切断する
	
	for (auto connects : m_connects) {
		for (auto connect : connects) {
			for (auto road_type_connect : connect.roadTypeConnect) {
				if (road_type_connect.second->isOn(arg_connect_coordinate)) {
					set_direction_id(UnitaryTools::getDirectionIDfromDifference(m_start_coordinate, arg_object_p->getOriginCoordinate() + arg_connect_coordinate), true);
					set_type_id();
				}
			}
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
