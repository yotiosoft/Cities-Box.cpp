//
//  Object.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/26.
//

#ifndef Object_hpp
#define Object_hpp

#include <Specific.hpp>
#include "Addon.hpp"

class Object {
public:
	// コンストラクタ
	Object();
	Object(int arg_object_id, Addon* arg_addon_p, String arg_original_name, TypeID::Type arg_type_id, DirectionID::Type arg_direction_id, CoordinateStruct arg_start_coordinate);
	
	// 原点を取得
	CoordinateStruct getOriginCoordinate();
	
	// アドオン名を取得
	String getAddonName(NameMode::Type arg_name_mode);
	
	// 固有名称を取得
	String getOriginalName();
	
	// アドオンを取得
	Addon* getAddonP();
	AddonDirectionStruct getAddonDirectionStruct();
	
	// TypeID
	TypeID::Type getTypeID();
	
	// DirectionID
	DirectionID::Type getDirectionID();
	
	// ObjectID
	void setObjectID(int arg_object_id);
	int getObjectID();
	
	// 周囲のオブジェクトと接続（ConnectableTypeの場合）
	void connect(CoordinateStruct arg_coordinate, DirectionID::Type arg_direction, Object* arg_object_p);
	
	// 描画
	void draw(RelativeCoordinateStruct arg_draw_coordinate, PositionStruct arg_draw_position, TimeStruct arg_time, Color arg_add_color);
	
private:
	// 保持するアドオン
	Addon* m_addon_p;
	
	// TypeID
	TypeID::Type m_type_id;
	
	// DirectionID
	DirectionID::Type m_direction_id;
	
	// 固有名称
	String m_original_name;
	
	// オブジェクト番号
	int m_object_id;
	
	// 始点
	CoordinateStruct m_start_coordinate;
	
	// サイズ
	Size m_size;
	
	// 接続情報
	Array<Array<ConnectStruct>> m_connects;
};

#endif /* Object_hpp */
