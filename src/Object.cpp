//
//  Object.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/26.
//

#include "Object.hpp"

Object::Object() {
}

Object::Object(int arg_object_id, Addon* arg_addon_p, String arg_original_name, TypeID::Type arg_type_id, DirectionID::Type arg_direction_id, CoordinateStruct arg_start_coordinate) {
	m_object_id = arg_object_id;
	m_addon_p = arg_addon_p;
	m_original_name = arg_original_name;
	m_type_id = arg_type_id;
	m_direction_id = arg_direction_id;
	m_start_coordinate = arg_start_coordinate;
	
	/*debugLog(U"{},{},{},{},{}"_fmt(m_addon_p->getName(NameMode::English),
										 m_type_id,
										 m_direction_id,
										 m_start_coordinate.x,
										 m_start_coordinate.y));*/
}

// アドオン名を取得
String Object::getAddonName(NameMode::Type arg_name_mode) {
	return m_addon_p->getName(arg_name_mode);
}

// 固有名称を取得
String Object::getOriginalName() {
	return m_original_name;
}

// アドオンを取得
Addon* Object::getAddonP() {
	return m_addon_p;
}

AddonDirectionStruct Object::getAddonDirectionStruct() {
	return m_addon_p->getDirectionStruct(m_type_id, m_direction_id);
}

// TypeID
TypeID::Type Object::getTypeID() {
	return m_type_id;
}

// DirectionID
DirectionID::Type Object::getDirectionID() {
	return m_direction_id;
}

// 原点を取得
CoordinateStruct Object::getOriginCoordinate() {
	return m_start_coordinate;
}

// ObjectID
void Object::setObjectID(int arg_object_id) {
	m_object_id = arg_object_id;
}

int Object::getObjectID() {
	return m_object_id;
}

void Object::draw(RelativeCoordinateStruct arg_draw_coordinate, PositionStruct arg_draw_position, TimeStruct arg_time, Color arg_add_color) {
	m_addon_p->draw(m_type_id, m_direction_id, arg_draw_position, arg_draw_coordinate, arg_add_color, arg_time);
}
