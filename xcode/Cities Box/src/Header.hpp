//
//  Header.hpp
//
#pragma once

#include <stdio.h>
#include <vector>
#include <dirent.h>
#include <fstream>

#include <Siv3D.hpp> // OpenSiv3D v0.4.1

using namespace std;

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

typedef struct RGBstruct {
	int r;
	int g;
	int b;
} RGBstruct;

typedef struct FileStruct {
	string file_path;
	
	string folder_path;
	string folder_name;
	string file_name;
} FileStruct;
