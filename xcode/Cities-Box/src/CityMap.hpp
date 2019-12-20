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

typedef struct WorkPlaceStruct {
	RCOIFP::Type work_place;
	int work_places_serial_number;
} WorkPlaceStruct;

typedef struct SchoolStruct {
	School::Type school;
	int school_serial_number;
} SchoolStruct;

typedef struct SquareStruct {
	String original_name;
	//Array<String> category;
	
	Array<String> types;
	Array<String> directions;
	
	int serial_number;
	
	CoordinateStruct tiles_count;
	
	int residents;
	WorkersStruct workers;
	int students;
	
	int happiness_rate;
	map<String, int> rate;
	
	//CropStruct crop;
	
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
	void loadAddons(String addon_set_name);
	
	// カテゴリに当てはまるアドオンの一覧を返す
	Array<Addon*> getFitAddons(Array<String> selected_categories);
	
	// アドオンリストを返す
	map<String, Addon*> getAllAddons();
	
	// 読込中画面
	void loadingScreen();
	
	// マップの描画
	void drawSquare(CoordinateStruct coordinate, CameraStruct camera);
	void draw(CameraStruct camera, CursorStruct& cursor);
	
	// Addon構造体の取得
	Array<Addon> getAddon(CoordinateStruct coordinate);
	
	// 需要度の取得
	RCOIFstruct getDemand();
	
	// 人口の取得
	int getPopulation();
	
	// 資金の取得
	int getMoney();
	
	// マップサイズの取得
	SizeStruct getMapSize();
	
	// 座標から描画位置を取得
	CoordinateStruct positionToCoordinate(PositionStruct position, CameraStruct camera);
	
	PositionStruct coordinateToPosition(CoordinateStruct coordinate, CameraStruct camera);
	
	// 描画範囲を取得
	pair<CoordinateStruct, CoordinateStruct> getDrawArea(CameraStruct camera);
	
	// いずれかのアドオンがカテゴリに含まれているか
	bool isInCategories(String search_category, CoordinateStruct coordinate);
	
	// アドオンを設置
	bool build(CoordinateStruct position, Addon* selected_addon, bool need_to_break);
	
	// アドオンを更新
	void update(CoordinateStruct position, Addon* selected_addon, Array<CoordinateStruct>& need_update);
	
	// アドオンを除去
	void breaking(CoordinateStruct position);
	
	// アドオンの始点となるマスに移動する
	CoordinateStruct moveToAddonStartSquare(CoordinateStruct search_coordinate, int addon_number);
	
	// 指定した場所に合うアドオンのTypeとDirectionを取得
	bool getBuildTypeAndDirection(CoordinateStruct coordinate, Addon* selected_addon, String& ret_type, String& ret_direction, Array<CoordinateStruct>& need_update);
	
	// アドオンを削除
	void clear(CoordinateStruct position);
	
	// 座標がマップ範囲内に入っているか
	bool isPositionAvailable(CoordinateStruct coordinate);
	
	// 時間を進ませて取得
	TimeStruct cityTime(int minutes_delta);
	
	// 各率を取得
	map<String, int> getRate(CoordinateStruct coordinate);
	int getRate(CoordinateStruct coordinate, String rate_name);
	
	// 各率を表示するときの色を取得
	Color getRateColor(int rate, bool upper, int standard);
	
	// レート表示モード
	void setShowRate(String rate_name);
	
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
	
	map<String, Addon*> addons;
	
	CameraStruct camera_before;
	pair<CoordinateStruct, CoordinateStruct> range;
	
	String map_file_path;
	
	String show_rate = U"";
	
	// プライベート関数
	bool getElement(String str, String search_element_name, String& ret);
	bool getElement(String str, String search_element_name, int& ret);
	bool getElement(String str, String search_element_name, bool& ret);
	bool getTypes(String str, String search_element_name, Array<String>& ret);
};

#endif /* CityMap_hpp */
