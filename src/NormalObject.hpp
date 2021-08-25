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
	NormalObject(int arg_object_id, Addon* arg_addon_p, String arg_original_name, CoordinateStruct arg_start_coordinate) : Object(arg_object_id, arg_addon_p, arg_original_name, arg_start_coordinate) {};
	NormalObject(int arg_object_id, Addon* arg_addon_p, String arg_original_name, TypeID::Type arg_type_id, DirectionID::Type arg_direction_id, CoordinateStruct arg_start_coordinate) : Object(arg_object_id, arg_addon_p, arg_original_name, arg_type_id, arg_direction_id, arg_start_coordinate) {};
	
	void connect(CityNetwork& road_network, CoordinateStruct arg_connect_coordinate, Object *arg_object_p);
	
	// デストラクタ
	~NormalObject();
	
private:
	/* 関数部 */
	// オブジェクトのDirectionIDを設定
	void set_direction_id(DirectionID::Type arg_direction, bool is_deleted);
	
	// オブジェクトのTypeIDを設定
	void set_type_id();
	
	// DirectionID -> 道路のTypeIDに変換
	TypeID::Type get_type_id(DirectionID::Type arg_direction);
};

#endif /* NormalObject_hpp */
