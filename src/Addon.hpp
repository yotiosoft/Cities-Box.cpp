//
//  Addon.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//
#ifndef Addon_hpp
#define Addon_hpp

#pragma once

#include <Specific.hpp>
#include "ImagesStruct.hpp"
#include "AddonType.hpp"
#include "AddonDirectionStruct.hpp"

typedef struct EffectStruct {
	int influence;
	int grid;
} EffectStruct;

typedef struct EffectsStruct {
	EffectStruct landPrice 				= {0, 0};	// 地価
	EffectStruct crimeRate 				= {0, 0};	// 犯罪率
	EffectStruct educationRate 			= {0, 0};	// 教育度
	EffectStruct noise 					= {0, 0};	// 騒音
	EffectStruct garbageDisposal 		= {0, 0};	// ごみ処理効率
	EffectStruct firingRate 			= {0, 0};	// 発火率
	EffectStruct post					= {0, 0};	// 郵便充足度
	EffectStruct mobileCommunication	= {0, 0};	// モバイル通信
	EffectStruct freeWiFi				= {0, 0};	// 無料Wi-Fiスポット
	EffectStruct television				= {0, 0};	// テレビ放送
	EffectStruct radio					= {0, 0};	// ラジオ放送
	EffectStruct touristAttraction		= {0, 0};	// 観光魅力度
	EffectStruct durability				= {0, 0};	// 耐久安全性
} EffectsStruct;

typedef struct CropStruct {
	String name;
	int amount;
} CropStruct;

class Addon {
public:
	Addon();
	
	// アドオン読み込み
	bool load(FileStruct newFilePath, String loadingAddonsSetName);
	
	// 名前の取得
	String getName(NameMode::Type mode);
	
	// 製作者名の取得
	String getAuthorName();
	
	// 説明文の取得
	String getSummary();
	
	// Typeの名前の取得
	TypeID::Type getTypeID(int type_num);
	
	// Directionの名前の取得
	DirectionID::Type getDirectionID(int typeNum, int directionNum);
	DirectionID::Type getDirectionID(String typeName, int directionNum);
	
	// DirectionStructを参照
	AddonDirectionStruct getDirectionStruct(TypeID::Type arg_type_id, DirectionID::Type arg_direction_id);
	
	// カテゴリを取得
	Array<CategoryID::Type> getCategories();
	
	// 指定したカテゴリに該当するか
	bool isInCategories(CategoryID::Type searchCategory);
	bool isInCategories(Array<CategoryID::Type> searchCategories);
	bool isMatch(Addon* target_addon, CategoryID::Type hint);
	bool canConnect(Addon* target_addon);
	
	// 効果を取得
	map<RateID::Type, EffectStruct> getEffects();
	
	// アイコンのテクスチャの取得
	void drawIcon(PositionStruct position, PositionStruct leftTop, Size size);
	
	// 使用するタイルを取得
	CoordinateStruct getUseTiles(TypeID::Type typeID, DirectionID::Type directionID);
	
	// アドオンを描画する
	void draw(TypeID::Type typeID, DirectionID::Type directionID, PositionStruct position, RelativeCoordinateStruct tilesCount, Color addColor, TimeStruct time);
	
protected:
	/* プライベート関数 */
	// アドオン読み込み
	bool m_load_adat(FileStruct newFilePath, String loadingAddonsSetName);
	bool m_load_adj(FileStruct newFilePath, String loadingAddonsSetName);
	
	// adatファイル読み込み関連
	bool m_get_element(String str, String searchElementName, String& ret);
	bool m_get_element(String str, String searchElementName, int& ret);
	bool m_get_types(String str, String searchElementName, Array<String>& ret);
	
	// r141以前のアドオンファイルからAddonLayerの読み込み
	void m_load_layer_before141(int layer_num, JSONValue type, AddonType& arg_addon_type, Array<AddonLayer>& arg_layers);
	
	// JSON形式に変換する
	void m_converter();
	
	/* プライベート変数 */
	// アドオンファイルのパス
	FileStruct m_addon_file_path;
	
	// アドオン名
	String m_addon_name;				// 英語名
	String m_addon_jp_name;				// 日本語名
	
	// 製作者名
	String m_addon_author;
	
	// 説明文
	String m_addon_summary;
	
	// 所属するアドオンセットの名前（空白はNormalとみなす）
	Array<String> m_belong_addons_set_name;
	
	// アドオンのタイプ
	Array<CategoryID::Type> m_addon_categories;
	
	// アイコン画像のパス
	String m_addon_icon;
	
	// アイコンのテクスチャ
	Texture m_icon_texture;
	
	// 使用するtype
	Array<String> m_use_types;
	
	// 収容人数 or 最大従業員数
	int m_maximum_capacity;
	
	// 建物の効果
	map<RateID::Type, EffectStruct> m_effects;
	
	// 地価
	int m_land_price_influence;				// 上昇額
	int m_land_price_influence_grid;		// 地価の上下が影響するマス
	
	// 各typeの情報
	map<TypeID::Type, AddonType> m_types;
};

#endif /* Addon_hpp */
