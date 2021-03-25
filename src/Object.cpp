//
//  Object.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/26.
//

#include "Object.hpp"

Object::Object(Addon* arg_addon_p, TypeID::Type arg_type_id, DirectionID::Type arg_direction_id, PositionStruct arg_start_position, Size arg_size) {
	m_addon_p = arg_addon_p;
	m_type_id = arg_type_id;
	m_direction_id = arg_direction_id;
	m_start_position = arg_start_position;
	m_size = arg_size;
}
