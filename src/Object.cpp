//
//  Object.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/26.
//

#include "Object.hpp"

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

void Object::draw(RelativeCoordinateStruct arg_draw_coordinate, CameraStruct arg_camera, TimeStruct arg_time) {
	
}
