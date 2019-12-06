//
//  CityMap.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#ifndef CityMap_hpp
#define CityMap_hpp

#include "Specific.hpp"
#include "Addon.hpp"
#include "OtherFunctions.hpp"

typedef struct TimeStruct {
	int year;
	int month;
	int date;
	int hour;
	int minutes;
} TimeStruct;

typedef struct RCOIFstruct {
	int residential;
	int commercial;
	int office;
	int industrial;
	int farm;
} RCOIFstruct;

typedef struct WorkersStruct {
	int commercial;
	int office;
	int industrial;
	int farm;
	int public_facility;
} WorkersStruct;

typedef struct BudgetStruct {
	int police;
	int fire_depertment;
	int post_office;
	int education;
} BudgetStruct;

typedef struct CropStruct {
	String name;
	int amount;
} CropStruct;

typedef struct WorkPlaceStruct {
	RCOIFP::Type work_place;
	int work_places_serial_number;
} WorkPlaceStruct;

typedef struct SchoolStruct {
	School::Type school;
	int school_serial_number;
} SchoolStruct;

typedef struct SquareStruct {
	Array<String> addon_name;
	String original_name;
	Array<String> category;
	
	Array<int> type_number;
	Array<int> direction_number;
	
	int serial_number;
	
	CoordinateStruct tiles_count;
	CoordinateStruct use_tiles;
	
	int residents;
	WorkersStruct workers;
	int students;
	
	int land_price;
	int crime_rate;
	int education_rate;
	int happiness_rate;
	int noise;
	
	CropStruct crop;
	
	Array<int> age;
	Array<String> gender;
	
	Array<WorkPlaceStruct> work_places;
	Array<SchoolStruct> schools;
	
	RCOIFP::Type reservation;
	
	Array<Addon*> addons;
	
} SquareStruct;



class CityMap {
public:
	// マップの読み込み
	void load(String new_map_file_path);
	void loadCBD(String new_map_file_path);
	void loadCBJ(String new_map_file_path);
	
	// アドオンの読み込み
	void loadAddons(string addon_set_name);
	
	// 読込中画面
	void loadingScreen();
	
	// マップの描画
	void drawSquare(CoordinateStruct coordinate, CameraStruct camera);
	void draw(CameraStruct camera, CursorStruct& cursor);
	
	// マップサイズの取得
	SizeStruct getMapSize();
	
	// 座標から描画位置を取得
	CoordinateStruct positionToCoordinate(PositionStruct position, CameraStruct camera);
	
	PositionStruct coordinateToPosition(CoordinateStruct coordinate, CameraStruct camera);
	
	// 描画範囲を取得
	pair<CoordinateStruct, CoordinateStruct> getDrawArea(CameraStruct camera);
	
	// マップ保存
	bool save();
	
	// メモリ解放
	void freeMapAndAddons();
	
private:
	int saved_version;
	String addon_set;
	
	String city_name;
	String mayor_name;
	int total_population;
	bool change_weather;
	int temperature;
	bool dark_on_night;
	
	SizeStruct mapsize;
	
	TimeStruct time;
	
	RCOIFstruct demand;
	
	int money;
	
	BudgetStruct budget;
	RCOIFstruct tax;
	
	Array<Array<SquareStruct>> squares;
	
	bool loading_complete;
	
	map<string, Addon*> addons;
	
	CameraStruct camera_before;
	pair<CoordinateStruct, CoordinateStruct> range;
	
	String map_file_path;
	
	// プライベート関数
	bool getElement(String str, String search_element_name, String& ret);
	bool getElement(String str, String search_element_name, int& ret);
	bool getElement(String str, String search_element_name, bool& ret);
	bool getTypes(String str, String search_element_name, Array<String>& ret);
};

#endif /* CityMap_hpp */
