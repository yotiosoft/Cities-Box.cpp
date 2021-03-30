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
	Object(Addon* arg_addon_p, TypeID::Type arg_type_id, DirectionID::Type arg_direction_id, CoordinateStruct arg_start_coordinate);
	
	// 描画
	void draw(RelativeCoordinateStruct arg_draw_coordinate, CameraStruct arg_camera, TimeStruct arg_time);
	
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
};

#endif /* Object_hpp */
