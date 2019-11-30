//
//  Header.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//
#pragma once

#include <stdio.h>
#include <vector>
#include <fstream>

#include <Siv3D.hpp> // OpenSiv3D v0.4.1

#include "SpecificHeader.hpp"

using namespace std;

class CityMap;
class Addon;
class Images;

namespace RCOIFP {
	enum Type {
		None = -1,
		Residential = 0,
		Commercial = 1,
		Office = 2,
		Industrial = 3,
		Farm = 4,
		Public = 5,
	};
}

namespace School {
	enum Type {
		None = -1,
		ElementarySchool = 0,
		JuniorHighSchool = 1,
		HighSchool = 2,
		University = 3,
	};
}

struct ImageStruct {
	Texture texture;
	string file_path;
	string file_name;
	string tag;
};

struct PositionStruct {
	int x;
	int y;
};

struct RGBstruct {
	int r;
	int g;
	int b;
};

struct FileStruct {
	string file_path;

	string folder_path;
	string folder_name;
	string file_name;
};
