//
//  NormalObject.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/06/18.
//

#ifndef NormalObject_hpp
#define NormalObject_hpp

#include "Object.hpp"

class NormalObject : public Object {
public:
	// コンストラクタ(Objectに継承)
	NormalObject();
	NormalObject(int arg_object_id, Addon* arg_addon_p, String arg_original_name, CoordinateStruct arg_start_coordinate, AroundObjects around_objects) : Object(arg_object_id, arg_addon_p, arg_original_name, arg_start_coordinate, around_objects) {};
	NormalObject(int arg_object_id, Addon* arg_addon_p, String arg_original_name, TypeID::Type arg_type_id, DirectionID::Type arg_direction_id, CoordinateStruct arg_start_coordinate) : Object(arg_object_id, arg_addon_p, arg_original_name, arg_type_id, arg_direction_id, arg_start_coordinate) {};
	
	// アップデート（何もしない）
	void update();
	
private:
	
};

#endif /* NormalObject_hpp */
