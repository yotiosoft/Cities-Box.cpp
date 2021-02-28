//
//  Tile.hpp
//  Cities-Box
//
//  Created by YotioSodt on 2021/02/28.
//

#ifndef Tile_hpp
#define Tile_hpp

#include "Specific.hpp"

class Tile {
public:
	String originalName;
	//Array<String> category;
	
	Array<String> types;
	Array<String> directions;
	
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
};

#endif /* Tile_hpp */
