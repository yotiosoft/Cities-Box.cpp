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
	
	int size_width;					// 画像の横方向のサイズ
	int size_height;				// 画像の縦方向のサイズ
	int chip_x;						// アドオンが占める横方向のマスの数
	int chip_y;						// アドオンが占める縦方向のマスの数
	
	int top_left_x;					// 左上のx座標
	int top_left_y;					// 左上のy座標
	int bottom_right_x;				// 右下のx座標
	int bottom_right_y;				// 右下のy座標
} AddonDirectionStruct;

typedef struct AddonTypeStruct {
	string type;
	
	String image;					// アドオン画像のパス
	String night_mask;				// ナイトマスク画像のパス
	Array<String> direction_names;	// typeに含まれる方向
	RGBstruct transparent_color;	// 透過色のRGB値
	
	map<String, AddonDirectionStruct> directions;	// typeに含まれる各方向の情報
	
	Texture texture;				// アドオン画像のテクスチャ
} AddonTypeStruct;

class Addon {
public:
	Addon();
	
	// 内容の変更
	bool load(FileStruct new_file_path, String loading_addons_set_name);
	bool loadADAT(FileStruct new_file_path, String loading_addons_set_name);
	bool loadADJ(FileStruct new_file_path, String loading_addons_set_name);
	
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
	String getDirectionName(int type_num, int direction_num);
	
	// 最終的に表示する座標を取得
	PositionStruct getPosition(String type_name, String direction_name, PositionStruct position, CoordinateStruct use_tiles, CoordinateStruct tiles_count);
	
	// アイコンのImageStructの取得
	ImageStruct getIconImageStruct();
	
	// アドオンを描画する
	void draw(String type_name, String direction_name, PositionStruct position, CoordinateStruct use_tiles, CoordinateStruct tiles_count);
	
	// JSON形式に変換する
	void converter();
	
private:
	// アドオンファイルのパス
	FileStruct addon_file_path;
	
	// アドオン名
	String addon_name;				// 英語名
	String addon_jp_name;			// 日本語名
	
	// 製作者名
	String addon_author;
	
	// 説明文
	String addon_summary;
	
	// 所属するアドオンセットの名前（空白はNormalとみなす）
	Array<String> belong_addons_set_name;
	
	// アドオンのタイプ
	Array<String> addon_categories;
	
	// アイコン画像のパス
	String addon_icon;
	
	// 使用するtype
	Array<String> use_types;
	
	// 収容人数
	int maxium_capacity;
	
	// 地価
	int land_price_influence;		// 上昇額
	int land_price_influence_grid;	// 地価の上下が影響するマス
	
	// 各typeの情報
	map<String, AddonTypeStruct> types;
	
	
	// プライベート関数
	bool getElement(String str, String search_element_name, String& ret);
	bool getElement(String str, String search_element_name, int& ret);
	bool getTypes(String str, String search_element_name, Array<String>& ret);
	void set_alpha_color(Image& image_temp, Color transparent_rgb);
};

#endif /* Addon_hpp */
