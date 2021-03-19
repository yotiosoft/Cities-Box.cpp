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
#include <random>

#include <Siv3D.hpp> // OpenSiv3D v0.4.3

#include "Enums.hpp"
#include "Structs.hpp"

#define CHIP_SIZE		64
#define RELEASE_NUMBER	141
#define AROUND_TILES	4
#define DIRECTIONS		15

using namespace std;

class CityMap;
class Addon;
class ImagesStruct;

// 周囲4方向
const pair<String, CoordinateStruct> AroundTiles[4] = {
	{U"right", {1, 0}}, {U"bottom", {0, 1}}, {U"top", {0, -1}}, {U"left", {-1, 0}}
};

