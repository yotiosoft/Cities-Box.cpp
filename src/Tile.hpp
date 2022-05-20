//
//  Tile.hpp
//  Cities-Box
//
//  Created by YotioSodt on 2021/02/28.
//

#ifndef Tile_hpp
#define Tile_hpp

#include <Specific.hpp>
#include "Object.hpp"

class Tile {
public:
	// オブジェクトの登録
	void addObject(Object* arg_object_p, RelativeCoordinateStruct arg_relative_coordinate);
	
	// オブジェクトの建設（もともとあったものは削除する）
	void buildObject(Object* arg_object_p, RelativeCoordinateStruct arg_relative_coordinate);
	
	// originalName
	bool setOriginalName(String new_original_name);
	String getOriginalName();
	
	// types
	bool setTypes(Array<TypeID::Type> new_types);
	bool addType(TypeID::Type add_type);
	bool addTypes(Array<TypeID::Type> add_types);
	Array<TypeID::Type> getTypes();
	TypeID::Type getType(int num);
	
	// directions
	bool setDirections(Array<DirectionID::Type> new_directions);
	bool addDirection(DirectionID::Type add_direction);
	bool addDirections(Array<DirectionID::Type> add_directions);
	Array<DirectionID::Type> getDirections();
	DirectionID::Type getDirection(int num);
	
	// 効果の設定
	void setTileRate(RateID::Type arg_rate_type, int arg_rate);
	
	// 効果を取得
	map<RateID::Type, int> getRate();
	int getRate(RateID::Type rate_id);
	
	// 引数のアドオンがタイル上に存在するか？
	bool hasAddon(CBAddon* addon_p);
    
    // 引数のカテゴリに該当するアドオンがタイル上に存在するか？
    Object* hasCategory(CategoryID::Type category);
	
	// Object情報を取得
	bool isObjectExists(int arg_object_id);
	Object* getObjectP(String arg_addon_name, NameMode::Type arg_name_mode);
	Array<Object*> getObjectsP(CategoryID::Type category_id);
	Array<ObjectStruct> getObjectStructs();
	
	// 接続状態を更新
	bool updateConnections(Array<Array<Tile>>& arg_tiles);
	
	// 描画
	void draw(RateID::Type arg_show_rate_id, PositionStruct arg_draw_position, TimeStruct arg_time);
	
	// 特定のオブジェクトを削除
	bool deleteObject(int arg_object_id);
	
	// クリア
	void clearAddons();				// addons, tiles, directionsだけをクリア（更新用）
	void clearAll();				// タイル上のすべてのデータ（アドオン情報＋住民情報など）をクリア
	
	int serialNumber;
	
	CoordinateStruct tilesCount;
	
	int residents;
	WorkersStruct workers;
	int students;
	
	int happinessRate;
	map<RateID::Type, int> rate;
	
	//CropStruct crop;
	
	Array<int> age;
	Array<String> gender;
	
	Array<WorkPlaceStruct> workPlaces;
	Array<SchoolStruct> schools;
	
	RCOIFP::Type reservation;
	
	Array<CBAddon*> addons;
	
private:
	// プライベート関数
	// 各率を表示するときの色を取得
	Color m_get_rate_color(int rate, bool upper, int standard);
	
	// そのオブジェクトが自分の方向を向いているか（updateConnectionsで使用）
	bool m_is_this_points_for_me(ObjectStruct& my_object_st, ObjectStruct& target_object_st);
	
	// オブジェクト同士のカテゴリの一致
	bool m_is_objects_category_match(Object* obj1, Object* obj2);
	
	// プライベート変数
	// 保持するオブジェクト
	Array<ObjectStruct> m_objects;
	
	String m_original_name;
	//Array<String> category;
	
	Array<TypeID::Type> m_types;
	Array<DirectionID::Type> m_directions;
};

#endif /* Tile_hpp */
