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
		
		User				= 0xf007,
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
		Disabled			= -2,	// 無効
		Ground				= -1,	// 地面
		
		Normal				= 0,	// 常時表示される部分
		Evening				= 10,	// 夕方表示される部分
		Night				= 20,	// 夜間表示される部分
		
		OnSpring			= 1,	// 春季だけ表示される部分
		OnSummer			= 2,	// 夏季だけ表示される部分
		OnAutumn			= 3,	// 秋季だけ表示される部分
		OnWinter			= 4,	// 冬季だけ表示される部分
		
		OnSpringEvening		= 11,	// 春季夕方表示される部分
		OnSummerEvening		= 12,	// 夏季夕方表示される部分
		OnAutumnEvening		= 13,	// 秋季夕方表示される部分
		OnWinterEvening		= 14,	// 冬季夕方表示される部分
		
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
		Disabled								= 0,
		Normal									= 1,
		UnderConstruction						= 2,
		Default									= 3,
		Turn									= 4,
		IntersectionT							= 5,
		IntersectionCross						= 6,
		DeadEnd									= 7,
		TrainCrossing							= 8,
		Bridge									= 9,
		TileAutumn								= 10,
		TileSnow								= 11,
		WaterEstuary							= 12,
		WaterIntersectionCrossWithoutOneCorner	= 13,
		Isolated								= 14,
		WaterIntersectionT						= 16,
		WaterOffshore							= 17,
		WaterTurn								= 18,
		
	};
}

// アドオンの方向の識別
namespace DirectionID {
	enum Type {
		// Disabled
		Disabled		= -1,
		
		// Default, Isolated
		None			= 0,
		
		// DeadEnd
		North			= 1,
		South			= 2,
		East			= 5,
		West			= 9,
		
		// Normal
		EastWest		= East + West,
		NorthSouth		= North + South,
		
		// Turn
		SouthWest		= South + West,
		NorthWest		= North + West,
		SouthEast		= South + East,
		NorthEast		= North + East,
		
		// T字路
		SouthEastWest	= South + East + West,
		NorthEastWest	= North + East + West,
		NorthSouthWest	= North + South + West,
		NorthSouthEast	= North + South + East,
		
		// 交差点
		All				= North + East + South + West,
		
		// 斜め
		Northeast		= 21,
		Northwest		= 22,
		Southeast		= 26,
		Southwest		= 29,
		
		// 遠洋
		Offshore		= North + Northeast + East + Southeast + South + Southwest + West + Northwest,
		
		// WaterEstuary
		WithoutSouthwestNorthwest = Offshore - Southwest - Northwest,
		WithoutNortheastNorthwest = Offshore - Northeast - Northwest,
		WithoutSoutheastSouthwest = Offshore - Southeast - Southwest,
		WithoutNortheastSoutheast = Offshore - Northeast - Southeast,
		
		// WaterIntersectionT
		WithoutEast  = Offshore - Northeast - East - Southeast,
		WithoutSouth = Offshore - Southeast - South - Southwest,
		WithoutNorth = Offshore - North - Northeast - Northwest,
		WithoutWest  = Offshore - Southwest - West - Northwest,
		
		// WaterTurn
		WithoutNorthWestNorthwest = Offshore - North - West - Northwest,
		WithoutNorthNortheastEast = Offshore - North - Northeast - East,
		WithoutSouthSouthwestWest = Offshore - South - Southwest - West,
		WithoutEastSoutheastSouth = Offshore - East - Southeast - South,
		
		// WaterIntersectionCrossWithoutOneCorner(WOC)
		WithoutNorthwest = Offshore - Northwest,
		WithoutNortheast = Offshore - Northeast,
		WithoutSouthwest = Offshore - Southwest,
		WithoutSoutheast = Offshore - Southeast
	};
}

// レート表示情報
namespace RateID {
	enum Type {
		None,
		LandPrice,
		CrimeRate,
		EducationRate,
		HappinessRate
	};
}

// カテゴリ
namespace CategoryID {
	enum Type {
		Disabled,           // 無効
		
		Connectable,		// 接続可能
		Road,				// 道路
		Car,				// 車道
		Promenade,			// 歩道
		TwoLane,			// 2車線（双方向）
		Railroad,			// 線路
		Train,				// 鉄道路線
		Station,			// 駅
		Waterway,			// 水路、海
		Ship,				// 航路
		Airline,			// 航空路
		Airport,			// 空港
		Runway,				// 滑走路
		Taxiway,			// 誘導路
		RailroadCrossing,	// 踏切
		
		ObjectType,			// 建物
		HighDensity,		// 高密度
		LowDensity,			// 低密度
		Residential,		// 住宅
		Commecial,			// 商業
		Office,				// オフィス
		Industrial,			// 工場
		Farm,				// 農業
		Public,				// 公共施設
		FireDepartment,		// 消防署
		Police,				// 警察署
		PostOffice,			// 郵便局
		Education,			// 教育施設
		ElementarySchool,	// 小学校
		JuniorHighSchool,	// 中学校
		HighSchool,			// 高校
		University,			// 大学
		CityHall,			// 市役所
		PublicHall,			// 公民館
		Park,				// 公園
		
		PutType,			// タイル
		Tile,				// タイル
	};
}

#endif /* Enums_hpp */
