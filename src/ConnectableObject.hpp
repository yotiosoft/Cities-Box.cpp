//
//  ConnectableObject.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/06/18.
//

#ifndef ConnectableObject_hpp
#define ConnectableObject_hpp

#include "Object.hpp"
#include "CityNetwork.hpp"

class ConnectableObject : public Object {
public:
	// コンストラクタ(Objectに継承)
	ConnectableObject();
	ConnectableObject(int arg_object_id, CBAddon* arg_addon_p, String arg_original_name, CoordinateStruct arg_start_coordinate) : Object(arg_object_id, arg_addon_p, arg_original_name, arg_start_coordinate) {};
	ConnectableObject(int arg_object_id, CBAddon* arg_addon_p, String arg_original_name, TypeID::Type arg_type_id, DirectionID::Type arg_direction_id, CoordinateStruct arg_start_coordinate) : Object(arg_object_id, arg_addon_p, arg_original_name, arg_type_id, arg_direction_id, arg_start_coordinate) {};
	
	// 周囲のオブジェクトと接続
	void connect(CityNetwork& road_network, CoordinateStruct arg_connect_coordinate, Object *arg_object_p, bool from_here);
    void connectWithSpecifiedType(CityNetwork& road_network, CoordinateStruct arg_connect_coordinate, Object *arg_object_p, TypeID::Type type, bool from_here);
	
	// 自分自身を削除
	// 戻り値；削除必須な周囲タイルのリスト（呼び出し元; CityMap::m_update_connected_tiles()側で処理）
	Array<CoordinateStruct> del(CityNetwork& road_network);
	
	// TypeIDおよびDirectionIDの更新
	void update();
	
	// デストラクタ
	~ConnectableObject();
	
private:
	/* 関数部 */
	// オブジェクトのDirectionIDを設定
	void set_direction_id(DirectionID::Type arg_direction, bool is_deleted);
	
	// オブジェクトのTypeIDを設定
	void set_type_id();
	
	// DirectionID -> 道路のTypeIDに変換
	TypeID::Type get_type_id(DirectionID::Type arg_direction);
	TypeID::Type get_type_id_waterway(DirectionID::Type arg_direction);
};

#endif /* ConnectableObject_hpp */
