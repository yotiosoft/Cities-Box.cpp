//
//  Tile.hpp
//  Cities-Box
//
//  Created by YotioSodt on 2021/02/28.
//

#ifndef Tile_hpp
#define Tile_hpp

#include "Specific.hpp"

class Object{
public:
int a;
};

class Tile {
public:
	// originalName
	bool setOriginalName(String new_original_name);
	String getOriginalName();
	
	// types
	bool setTypes(Array<String> new_types);
	bool addType(String add_type);
	bool addTypes(Array<String> add_types);
	Array<String> getTypes();
	String getType(int num);
	
	// directions
	bool setDirections(Array<String> new_directions);
	bool addDirection(String add_direction);
	bool addDirections(Array<String> add_directions);
	Array<String> getDirections();
	String getDirection(int num);
	
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
	String originalName;
	//Array<String> category;
	
	Array<String> types;
	Array<String> directions;
};

#endif /* Tile_hpp */
