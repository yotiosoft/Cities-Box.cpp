//
//  Header.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//
#pragma once

#include <stdio.h>
#include <fstream>
#include <algorithm>

#include <Siv3D.hpp> // OpenSiv3D v0.4.2

#define CHIP_SIZE		64
#define RELEASE_NUMBER	141
#define AROUND_TILES	4

using namespace std;

class CityMap;
class Addon;
class Images;

namespace RCOIFP {
	enum Type {
		None			= -1,
		Residential		= 0,
		Commercial		= 1,
		Office			= 2,
		Industrial		= 3,
		Farm			= 4,
		Public			= 5,
	};
}

namespace School {
	enum Type {
		None				= -1,
		ElementarySchool	= 0,
		JuniorHighSchool	= 1,
		HighSchool			= 2,
		University			= 3,
	};
}

namespace IconFont {
	enum ID {
		None				= 0x0000,
		
		Floppy				= 0xf0c7,
		Plus				= 0xf067,
		
		Cursor				= 0xf245,
		Road				= 0xf018,
		WALK				= 0xf554,
		CAR					= 0xf1b9,
		Railroad			= 0xf238,
		Residential			= 0xf015,
		Commercial			= 0xf54f,
		Office				= 0xf1ad,
		Industrial			= 0xf275,
		Farm				= 0xf722,
		Public				= 0xf19c,
		Park				= 0xf1bb,
		WaterWay			= 0xf21a,
		AirPort				= 0xf5b0,
		Field 				= 0xf0c8,
		
		Information			= 0xf05a,
		Rate				= 0xf009,
		Delete				= 0xf12d,
		Budget				= 0xf53c,
		Setting				= 0xf013,
		Save				= 0xf0c7,
		
		Crime				= 0xf6de,
		Durability			= 0xf72e,
		Education			= 0xf549,
		Firing				= 0xf06d,
		FreeWiFi			= 0xf1eb,
		Garbage				= 0xf2ed,
		LandPrice			= 0xf53a,
		MobileCommunication = 0xf012,
		Noise				= 0xf028,
		Post				= 0xf7d7,
		Television			= 0xf26c,
		Radio				= 0xf519,
		Tourist				= 0xf6fc,
		
		Population			= 0xf0c0,
	};
}
/*
namespace RCOIFcolor {
	enum Color {
		Residential			= Color(34, 177, 76),
		Commercial			= Color(63, 72, 204),
		Office				= Color(9, 132, 227),
		Industrial			= Color(255, 242, 0),
		Farm				= Color(185, 122, 87),
	}
}
*/
typedef struct ImageStruct {
	Texture texture;
	string file_path;
	string file_name;
	string tag;
} ImageStruct;

typedef struct PositionStruct {
	int x;
	int y;
} PositionStruct;

typedef struct CoordinateStruct {
	int x;
	int y;
} CoordinateStruct;

typedef struct CameraStruct {
	PositionStruct position;
	CoordinateStruct center;
} CameraStruct;

typedef struct SizeStruct {
	int width;
	int height;
} SizeStruct;

typedef struct RGBstruct {
	unsigned short int r;
	unsigned short int g;
	unsigned short int b;
} RGBstruct;

typedef struct FileStruct {
	string file_path;
	
	string folder_path;
	string folder_name;
	string file_name;
} FileStruct;

typedef struct CursorStruct {
	Texture* texture;
	CoordinateStruct coordinate;
	PositionStruct position;
	PositionStruct position_per_tiles;
} CursorStruct;

// 周囲4方向
const pair<String, CoordinateStruct> AroundTiles[4] = {
	{U"top", {0, -1}}, {U"right", {1, 0}}, {U"bottom", {0, 1}}, {U"left", {-1, 0}}
};
