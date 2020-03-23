//
//  Addon.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//
#ifndef Addon_hpp
#define Addon_hpp

#include "Specific.hpp"
#include "Images.hpp"

typedef struct AddonDirectionStruct {
	string direction;
	
	int sizeWidth;					// 画像の横方向のサイズ
	int sizeHeight;					// 画像の縦方向のサイズ
	int chipX;						// アドオンが占める横方向のマスの数
	int chipY;						// アドオンが占める縦方向のマスの数
	
	int topLeftX;					// 左上のx座標
	int topLeftY;					// 左上のy座標
	int bottomRightX;				// 右下のx座標
	int bottomRightY;				// 右下のy座標
} AddonDirectionStruct;

typedef struct AddonTypeStruct {
	string type;
	
	String image;					// アドオン画像のパス
	String nightMask;				// ナイトマスク画像のパス
	Array<String> directionNames;	// typeに含まれる方向
	RGBstruct transparentColor;		// 透過色のRGB値
	
	map<String, AddonDirectionStruct> directions;	// typeに含まれる各方向の情報
	
	Texture texture;				// アドオン画像のテクスチャ
} AddonTypeStruct;

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
	
	// 内容の変更
	bool load(FileStruct newFilePath, String loadingAddonsSetName);
	bool loadADAT(FileStruct newFilePath, String loadingAddonsSetName);
	bool loadADJ(FileStruct newFilePath, String loadingAddonsSetName);
	
	// 名前の取得
	String getName();		// 原名
	String getNameJP();		// 日本語名
	
	// 製作者名の取得
	String getAuthorName();
	
	// 説明文の取得
	String getSummary();
	
	// Typeの名前の取得
	String getTypeName(int type_num);
	
	// Directionの名前の取得
	String getDirectionName(int typeNum, int directionNum);
	String getDirectionName(String typeName, int directionNum);
	
	// カテゴリを取得
	Array<String> getCategories();
	
	// 指定したカテゴリに該当するか
	bool isInCategories(String searchCategory);
	bool isInCategories(Array<String> searchCategories);
	
	// 効果を取得
	map<String, EffectStruct> getEffects();
	
	// アイコンのテクスチャの取得
	void drawIcon(PositionStruct position, PositionStruct leftTop, SizeStruct size);
	
	// 使用するタイルを取得
	CoordinateStruct getUseTiles(String typeName, String directionName);
	
	// 最終的に表示する座標を取得
	PositionStruct getPosition(String typeName, String directionName, PositionStruct position, CoordinateStruct useTiles, CoordinateStruct tilesCount);
	
	// アドオンを描画する
	void draw(String typeName, String directionName, PositionStruct position, CoordinateStruct useTiles, CoordinateStruct tilesCount, Color* addColor);
	
	// JSON形式に変換する
	void converter();
	
protected:
	// アドオンファイルのパス
	FileStruct addonFilePath;
	
	// アドオン名
	String addonName;				// 英語名
	String addonJPName;				// 日本語名
	
	// 製作者名
	String addonAuthor;
	
	// 説明文
	String addonSummary;
	
	// 所属するアドオンセットの名前（空白はNormalとみなす）
	Array<String> belongAddonsSetName;
	
	// アドオンのタイプ
	Array<String> addonCategories;
	
	// アイコン画像のパス
	String addonIcon;
	
	// アイコンのテクスチャ
	Texture iconTexture;
	
	// 使用するtype
	Array<String> useTypes;
	
	// 収容人数 or 最大従業員数
	int maximumCapacity;
	
	// 建物の効果
	map<String, EffectStruct> effects;
	
	// 地価
	int landPriceInfluence;			// 上昇額
	int landPriceInfluenceGrid;		// 地価の上下が影響するマス
	
	// 各typeの情報
	map<String, AddonTypeStruct> types;
	
	
	// プライベート関数
	bool getElement(String str, String searchElementName, String& ret);
	bool getElement(String str, String searchElementName, int& ret);
	bool getTypes(String str, String searchElementName, Array<String>& ret);
	void setAlphaColor(Image& imageTemp, Color transparentRGB);
};

#endif /* Addon_hpp */
