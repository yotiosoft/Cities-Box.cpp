//
//  Object.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/26.
//

#include "Object.hpp"

Object::Object() {
}

Object::Object(int arg_object_id, CBAddon* arg_addon_p, String arg_original_name, CoordinateStruct arg_start_coordinate) {
	m_object_id = arg_object_id;
	m_addon_p = arg_addon_p;
	m_original_name = arg_original_name;
	m_start_coordinate = arg_start_coordinate;
	
	// オブジェクトサイズ分のm_connectsを用意
	m_connects.resize(getAddonDirectionStruct().requiredTiles.y);
	for (int y = 0; y < m_connects.size(); y++) {
		m_connects[y].resize(getAddonDirectionStruct().requiredTiles.x);
	}
}

Object::Object(int arg_object_id, CBAddon* arg_addon_p, String arg_original_name, TypeID::Type arg_type_id, DirectionID::Type arg_direction_id, CoordinateStruct arg_start_coordinate) {
	m_object_id = arg_object_id;
	m_addon_p = arg_addon_p;
	m_original_name = arg_original_name;
	m_type_id = arg_type_id;
	m_direction_id = arg_direction_id;
	m_start_coordinate = arg_start_coordinate;
	
	// オブジェクトサイズ分のm_connectsを用意
	m_connects.resize(getAddonDirectionStruct().requiredTiles.y);
	for (int y = 0; y < m_connects.size(); y++) {
		m_connects[y].resize(getAddonDirectionStruct().requiredTiles.x);
	}
	
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
CBAddon* Object::getAddonP() {
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

// 引数の座標上にこのオブジェクトが存在するか？
bool Object::isOn(CoordinateStruct arg_coordinate) {
	int width  = arg_coordinate.x - m_start_coordinate.x;
	int height = arg_coordinate.y - m_start_coordinate.y;
	
	if (width < m_size.x && height < m_size.y) {
		return true;
	}
	return false;
}

// 周囲のオブジェクトと接続
/*
void Object::connect(CoordinateStruct arg_coordinate, DirectionID::Type arg_direction, Object* arg_object_p) {
	cout << arg_direction << " : " << arg_object_p << endl;
	m_connects[arg_coordinate.y][arg_coordinate.x].roadTypeConnect << pair<DirectionID::Type, Object*>{arg_direction, arg_object_p};
}*/

// 描画
void Object::draw(RelativeCoordinateStruct arg_draw_coordinate, PositionStruct arg_draw_position, TimeStruct arg_time, Color arg_add_color) {
	m_addon_p->draw(m_type_id, m_direction_id, arg_draw_position, arg_draw_coordinate, arg_add_color, arg_time);
}

// 削除
void Object::setDeleted() {
	m_deleted = true;
}

bool Object::isDeleted() {
	return m_deleted;
}

Object::~Object() {
	
}
