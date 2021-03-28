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
	void addObject(Object* arg_object_p);
	
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
	
	// クリア
	void clearAddons();				// addons, tiles, directionsだけをクリア（更新用）
	void clearAll();				// タイル上のすべてのデータ（アドオン情報＋住民情報など）をクリア
	
	int serialNumber;
	
	CoordinateStruct tilesCount;
	
	int residents;
	WorkersStruct workers;
	int students;
	
	int happinessRate;
	map<String, int> rate;
	
	//CropStruct crop;
	
	Array<int> age;
	Array<String> gender;
	
	Array<WorkPlaceStruct> workPlaces;
	Array<SchoolStruct> schools;
	
	RCOIFP::Type reservation;
	
	Array<Addon*> addons;
	
private:
	String m_original_name;
	//Array<String> category;
	
	Array<TypeID::Type> m_types;
	Array<DirectionID::Type> m_directions;
};

#endif /* Tile_hpp */
