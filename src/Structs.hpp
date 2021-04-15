//
//  Structs.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/19.
//

#ifndef Structures_hpp
#define Structures_hpp

#include <stdio.h>

using namespace std;

class Object;

// 画像格納用
typedef struct ImageStruct {
	Texture texture;
	string file_path;
	string file_name;
	string tag;
} ImageStruct;

// 画面上の座標格納
typedef struct PositionStruct {
	int x;
	int y;
} PositionStruct;

// マップ上の座標格納
typedef struct CoordinateStruct {
	int x;
	int y;
} CoordinateStruct;

// マップ上の相対的な座標格納
typedef struct RelativeCoordinateStruct {
	CoordinateStruct relative;
	CoordinateStruct origin;
} RelativeCoordinateStruct;

// カメラの情報
typedef struct CameraStruct {
	PositionStruct position;
	CoordinateStruct center;
} CameraStruct;

// RGB値
typedef struct RGBstruct {
	unsigned short int r;
	unsigned short int g;
	unsigned short int b;
} RGBstruct;

// ファイル情報
typedef struct FileStruct {
	string file_path;
	
	string folder_path;
	string folder_name;
	string file_name;
} FileStruct;

// カーソル情報
typedef struct CursorStruct {
	Texture* texture;
	CoordinateStruct coordinate;
	PositionStruct position;
	PositionStruct position_per_tiles;
	bool pressed = false;
} CursorStruct;

// 時間
typedef struct TimeStruct {
	int year;
	int month;
	int date;
	int hour;
	int minutes;
} TimeStruct;

// RCOIFメータの値を格納
typedef struct RCOIFstruct {
	int residential;
	int commercial;
	int office;
	int industrial;
	int farm;
} RCOIFstruct;

// 労働者情報
typedef struct WorkersStruct {
	int commercial;
	int office;
	int industrial;
	int farm;
	int publicFacility;
} WorkersStruct;
// 支出情報
typedef struct BudgetStruct {
	int police;
	int fireDepertment;
	int postOffice;
	int education;
} BudgetStruct;

// 勤務地情報
typedef struct WorkPlaceStruct {
	RCOIFP::Type workPlace;
	int workPlacesSerialNumber;
} WorkPlaceStruct;

// 学校情報
typedef struct SchoolStruct {
	School::Type school;
	int schoolSerialNumber;
} SchoolStruct;

// オブジェクト情報
typedef struct ObjectStruct {
	Object* object_p;
	RelativeCoordinateStruct relative_coordinate;
} ObjectStruct;

// 接続情報
typedef struct ConnectStruct {
	Array<pair<DirectionID::Type, Object*>> roadTypeConnect;
} ConnectStruct;

#endif /* Structs_hpp */
