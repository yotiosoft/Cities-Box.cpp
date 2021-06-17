//
//  ConnectableObject.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/06/18.
//

#ifndef ConnectableObject_hpp
#define ConnectableObject_hpp

#include "Object.hpp"

class ConnectableObject : public Object {
public:
	// コンストラクタ(Objectに継承)
	ConnectableObject();
	ConnectableObject(int arg_object_id, Addon* arg_addon_p, String arg_original_name, CoordinateStruct arg_start_coordinate, AroundObjects around_objects) : Object(arg_object_id, arg_addon_p, arg_original_name, arg_start_coordinate, around_objects) {};
	ConnectableObject(int arg_object_id, Addon* arg_addon_p, String arg_original_name, TypeID::Type arg_type_id, DirectionID::Type arg_direction_id, CoordinateStruct arg_start_coordinate) : Object(arg_object_id, arg_addon_p, arg_original_name, arg_type_id, arg_direction_id, arg_start_coordinate) {};
	
	// 接続時のアップデート
	void update();
	
private:
	
};

#endif /* ConnectableObject_hpp */
