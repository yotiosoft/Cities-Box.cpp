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
	int publicFacility;
} WorkersStruct;

typedef struct BudgetStruct {
	int police;
	int fireDepertment;
	int postOffice;
	int education;
} BudgetStruct;

typedef struct WorkPlaceStruct {
	RCOIFP::Type workPlace;
	int workPlacesSerialNumber;
} WorkPlaceStruct;

typedef struct SchoolStruct {
	School::Type school;
	int schoolSerialNumber;
} SchoolStruct;

typedef struct SquareStruct {
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
	
} SquareStruct;



class CityMap {
public:
	// マップの読み込み
	void load(String loadMapFilePath);
	void loadCBD(String loadMapFilePath);
	void loadCBJ(String loadMapFilePath);
	
	// アドオンの読み込み
	void loadAddons(String addonSetName);
	
	// カテゴリに当てはまるアドオンの一覧を返す
	Array<Addon*> getFitAddons(Array<String> selectedCategories);
	
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
	bool isInCategories(String searchCategory, CoordinateStruct coordinate);
	
	// アドオンを設置
	bool build(CoordinateStruct position, Addon* selectedAddon, bool needToBreak);
	
	// アドオンを更新
	void update(CoordinateStruct position, Addon* selectedAddon, Array<CoordinateStruct>& needUpdate);
	
	// アドオンを除去
	void breaking(CoordinateStruct position);
	
	// アドオンの始点となるマスに移動する
	CoordinateStruct moveToAddonStartSquare(CoordinateStruct searchCoordinate, int addonNumber);
	
	// 指定した場所に合うアドオンのTypeとDirectionを取得
	bool getBuildTypeAndDirection(CoordinateStruct coordinate, Addon* selectedAddon, String& retType, String& retDirection, Array<CoordinateStruct>& needUpdate);
	
	// アドオンを削除
	void clear(CoordinateStruct position);
	
	// 座標がマップ範囲内に入っているか
	bool isPositionAvailable(CoordinateStruct coordinate);
	
	// 時間を進ませて取得
	TimeStruct cityTime(int minutesDelta);
	
	// 各率を取得
	map<String, int> getRate(CoordinateStruct coordinate);
	int getRate(CoordinateStruct coordinate, String rateName);
	
	// 各率を表示するときの色を取得
	Color getRateColor(int rate, bool upper, int standard);
	
	// レート表示モード
	void setShowRate(String rateName);
	
	// マップ保存
	bool save();
	
	// メモリ解放
	void freeMapAndAddons();
	
private:
	int savedVersion;
	String addonSet;
	
	String cityName;
	String mayorName;
	int totalPopulation;
	bool changeWeather;
	int temperature;
	bool darkOnNight;
	
	SizeStruct mapsize;
	
	TimeStruct time;
	
	RCOIFstruct demand;
	
	int money;
	
	BudgetStruct budget;
	RCOIFstruct tax;
	
	Array<Array<SquareStruct>> squares;
	
	bool loadingComplete;
	
	map<String, Addon*> addons;
	
	CameraStruct cameraBefore;
	pair<CoordinateStruct, CoordinateStruct> range;
	
	String mapFilePath;
	
	String showRate = U"";
	
	// プライベート関数
	bool getElement(String str, String searchElementName, String& ret);
	bool getElement(String str, String searchElementName, int& ret);
	bool getElement(String str, String searchElementName, bool& ret);
	bool getTypes(String str, String searchElementName, Array<String>& ret);
};

#endif /* CityMap_hpp */
