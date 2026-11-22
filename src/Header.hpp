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

#define CHIP_SIZE		        64
#define TITLE			        U"Cities Box.cpp"
#define VERSION			        U"2.0 alpha2"
#define RELEASE_NUMBER	        142
#define AROUND_TILES	        4
#define DIRECTIONS		        15
#define LAYER_TYPES		        15

#define _DEBUG					1		// Debugビルドか？（DebugLog()で使用）

using namespace std;

namespace UIConfig {
#if SIV3D_PLATFORM(WEB)
	inline constexpr int FontLarge = 24;
	inline constexpr int FontMedium = 18;
	inline constexpr int FontSmall = 12;
	inline constexpr int DetailsBarWidth = 720;
	inline constexpr int DetailsIconSize = 30;
	inline constexpr int DetailsItemGap = 20;
	inline constexpr int DetailsTemperatureTextGap = 4;
	inline constexpr int DetailsPopulationTextGap = 16;
	inline constexpr int DetailsTimeTextGap = 16;
	inline constexpr int ToolbarIconStep = 50;
	inline constexpr int AddonIconSize = 40;
	inline constexpr int AddonIconStep = 48;
	inline constexpr int MenuBarHeight = 60;

	constexpr int scaleIcon(const int size) {
		return ((size * 3 + 1) / 2);
	}
#else
	inline constexpr int FontLarge = 16;
	inline constexpr int FontMedium = 12;
	inline constexpr int FontSmall = 8;
	inline constexpr int DetailsBarWidth = 450;
	inline constexpr int DetailsIconSize = 20;
	inline constexpr int DetailsItemGap = 10;
	inline constexpr int DetailsTemperatureTextGap = 0;
	inline constexpr int DetailsPopulationTextGap = 10;
	inline constexpr int DetailsTimeTextGap = 5;
	inline constexpr int ToolbarIconStep = 35;
	inline constexpr int AddonIconSize = 32;
	inline constexpr int AddonIconStep = 32;
	inline constexpr int MenuBarHeight = 60;

	constexpr int scaleIcon(const int size) {
		return size;
	}
#endif
}

class CityMap;
class CBAddon;

// 周囲4方向
const pair<DirectionID::Type, CoordinateStruct> AroundTiles[AROUND_TILES] = {
	{DirectionID::East, {1, 0}}, {DirectionID::South, {0, 1}}, {DirectionID::North, {0, -1}}, {DirectionID::West, {-1, 0}}
};

// LayerType::Typeのすべて
const LayerType::Type AllLayerTypes[LAYER_TYPES] = {
	LayerType::Normal, LayerType::OnSpring, LayerType::OnSummer, LayerType::OnAutumn, LayerType::OnWinter,
	LayerType::Evening, LayerType::OnSpringEvening, LayerType::OnSummerEvening, LayerType::OnAutumnEvening, LayerType::OnWinterEvening,
	LayerType::Night, LayerType::OnSpringNight, LayerType::OnSummerNight, LayerType::OnAutumnNight, LayerType::OnWinterNight
};
