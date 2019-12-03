//
//  Header.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//
#pragma once

#include <stdio.h>
#include <list>
#include <dirent.h>
#include <fstream>
#include <thread>

#include <Siv3D.hpp> // OpenSiv3D v0.4.1

#include "Specific.hpp"

#define CHIP_SIZE		64

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
		Floppy				= 0xf0c7,
		Plus				= 0xf067,
	};
}

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
