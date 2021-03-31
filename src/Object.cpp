//
//  Object.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/26.
//

#include "Object.hpp"

Object::Object() {
}

Object::Object(Addon* arg_addon_p, TypeID::Type arg_type_id, DirectionID::Type arg_direction_id, CoordinateStruct arg_start_coordinate) {
	m_addon_p = arg_addon_p;
	m_type_id = arg_type_id;
	m_direction_id = arg_direction_id;
	m_start_coordinate = arg_start_coordinate;
	
	/*debugLog(U"{},{},{},{},{}"_fmt(m_addon_p->getName(NameMode::English),
										 m_type_id,
										 m_direction_id,
										 m_start_coordinate.x,
										 m_start_coordinate.y));*/
}

// 原点を取得
CoordinateStruct Object::getOriginCoordinate() {
	return m_start_coordinate;
}

// ObjectIDを取得
int Object::getObjectID() {
	return m_object_id;
}

void Object::draw(RelativeCoordinateStruct arg_draw_coordinate, PositionStruct arg_draw_position, TimeStruct arg_time, Color arg_add_color) {
	m_addon_p->draw(m_type_id, m_direction_id, arg_draw_position, arg_draw_coordinate, arg_add_color, arg_time);
}
