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
#define LAYER_TYPES		15

using namespace std;

class CityMap;
class Addon;
class ImagesStruct;

// 周囲4方向
const pair<DirectionID::Type, CoordinateStruct> AroundTiles[AROUND_TILES] = {
	{DirectionID::Right, {1, 0}}, {DirectionID::Bottom, {0, 1}}, {DirectionID::Top, {0, -1}}, {DirectionID::Left, {-1, 0}}
};

// LayerType::Typeのすべて
const LayerType::Type AllLayerTypes[LAYER_TYPES] = {
	LayerType::Normal, LayerType::OnSpring, LayerType::OnSummer, LayerType::OnAutumn, LayerType::OnWinter,
	LayerType::Evening, LayerType::OnSpringEvening, LayerType::OnSummerEvening, LayerType::OnAutumnEvening, LayerType::OnWinterEvening,
	LayerType::Night, LayerType::OnSpringNight, LayerType::OnSummerNight, LayerType::OnAutumnNight, LayerType::OnWinterNight
};
