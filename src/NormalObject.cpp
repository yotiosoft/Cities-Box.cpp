//
//  NormalObject.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/06/18.
//

#include "NormalObject.hpp"

void NormalObject::connect(CoordinateStruct arg_connect_coordinate, Object *arg_object_p, bool from_here) {
	// 道路用なので何もしない
	return;
}

void NormalObject::connectWithSpecifiedType(CoordinateStruct arg_connect_coordinate, Object *arg_object_p, TypeID::Type type, bool from_here) {
    // 道路用なので何もしない
    return;
}

Array<CoordinateStruct> NormalObject::del() {
	// 何もしない
	return Array<CoordinateStruct>();
}

void NormalObject::update() {
	// 何もしない
	return;
}

// デストラクタ
NormalObject::~NormalObject() {
	
}
