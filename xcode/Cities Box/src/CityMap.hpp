//
//  CityMap.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#ifndef CityMap_hpp
#define CityMap_hpp

#include "Header.hpp"
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
	string name;
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
	vector<string> addon_name;
	string original_name;
	vector<string> category;
	
	vector<int> type_number;
	vector<int> direction_number;
	
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
	
	vector<int> age;
	vector<string> gender;
	
	vector<WorkPlaceStruct> work_places;
	vector<SchoolStruct> schools;
	
	RCOIFP::Type reservation;
	
	vector<Addon*> addons;
	
} SquareStruct;



class CityMap {
public:
	// マップの読み込み
	void load(FileStruct map_file, map<string, Addon*> addons);
	
	// 読込中画面
	void loadingScreen();
	
	// マップの描画
	void draw_square(CoordinateStruct coordinate, CameraStruct camera);
	void draw();
	
	// マップサイズの取得
	SizeStruct getMapSize();
	
	// 座標から描画位置を取得
	CoordinateStruct positionToCoordinate(PositionStruct position, CameraStruct camera);
	
	PositionStruct coordinateToPosition(CoordinateStruct coordinate, CameraStruct camera);
	
	// 描画範囲を取得
	vector<CoordinateStruct> getDrawArea(CameraStruct camera);
	
	// メモリ解放
	void free();
	
private:
	int saved_version;
	string addon_set;
	
	string city_name;
	string mayor_name;
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
	
	vector<vector<SquareStruct>> squares;
	
	// プライベート関数
	bool getElement(string str, string search_element_name, string& ret);
	bool getElement(string str, string search_element_name, int& ret);
	bool getElement(string str, string search_element_name, bool& ret);
	bool getTypes(string str, string search_element_name, vector<string>& ret);
};

#endif /* CityMap_hpp */
