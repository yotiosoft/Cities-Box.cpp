//
//  Enums.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/19.
//

#ifndef Enums_hpp
#define Enums_hpp

// RCOIFP（種類）の識別
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

// 学校の識別
namespace School {
	enum Type {
		None				= -1,
		ElementarySchool	= 0,
		JuniorHighSchool	= 1,
		HighSchool			= 2,
		University			= 3,
	};
}

// アイコンの定義
namespace IconFont {
	enum ID {
		None				= 0x0000,
		
		Floppy				= 0xf0c7,
		Plus				= 0xf067,
		Compass				= 0xf568,
		
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
		Happiness			= 0xf4da,
		
		Population			= 0xf0c0,
		
		LeftWithCircle		= 0xf0a8,
		RightWithCircle		= 0xf0a9,
		
		Left				= 0xf060,
		Right				= 0xf061,
		
		Clock				= 0xf017,
		
		Sunny				= 0xf185,
		Cloudy				= 0xf0c2,
		Rainy				= 0xf740,
		Snowy				= 0xf2dc,
		
		TemperatureLow		= 0xf2ca,
		TemperatureMiddle	= 0xf2c9,
		TemperatureHigh		= 0xf2c8,
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

// 天気の定義
namespace Weather {
	enum Type {
		Sunny	= 0,
		Cloudy	= 1,
		Rainy	= 2,
		Snowy	= 3,
	};
}

// レイヤのタイプ
namespace LayerType {
	enum Type {
		Ground				= 0,	// 地面
		Normal				= 1,	// 常時表示される部分
		
		Evening				= 9,	// 夕方表示される部分
		
		Night				= 10,	// 夜間表示される部分
		
		OnSpring			= 11,	// 春季だけ表示される部分
		OnSummer			= 12,	// 夏季だけ表示される部分
		OnAutumn			= 13,	// 秋季だけ表示される部分
		OnWinter			= 14,	// 冬季だけ表示される部分
		
		OnSpringNight		= 21,	// 春季夜間表示される部分
		OnSummerNight		= 22,	// 夏季夜間表示される部分
		OnAutumnNight		= 23,	// 秋季夜間表示される部分
		OnWinterNight		= 24,	// 冬季夜間表示される部分
	};
}

// No, Neutral, Yesからなる戻り値
namespace ThreeTypesAnswer {
	enum Type {
		No,
		Neutral,
		Yes
	};
}

// 名前のモード
namespace NameMode {
	enum Type {
		English		= 0,
		Japanese	= 1,
	};
}

// アドオンのタイプの識別
namespace TypeID {
	enum Type {
		Normal				= 0,
		UnderConstruction	= 1,
		Default				= 2,
		Turn				= 3,
		IntersectionT		= 4,
		IntersectionCross	= 5,
		DeadEnd				= 6,
		TrainCrossing		= 7,
		Bridge				= 8,
		TileNormal			= 9,
		TileAutumn			= 10,
		TileSnow			= 11,
	};
}

// アドオンの方向の識別
namespace DirectionID {
	enum Type {
		Normal			= 0,
		Left			= 1,
		Top				= 2,
		Right			= 3,
		Bottom			= 4,
		Depth			= 5,
		Width			= 6,
		LeftRightBottom	= 7,
		LeftRightTop	= 8,
		LeftTopBottom	= 9,
		RightTopBottom	= 10,
		LeftBottom		= 11,
		LeftTop			= 12,
		RightBottom		= 13,
		RightTop		= 14,
	};
}

#endif /* Enums_hpp */
