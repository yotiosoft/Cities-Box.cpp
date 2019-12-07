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
	
	string image;					// アドオン画像のパス
	string night_mask;				// ナイトマスク画像のパス
	Array<string> direction;		// typeに含まれる方向
	RGBstruct transparent_color;	// 透過色のRGB値
	
	map<string, AddonDirectionStruct> directions;	// typeに含まれる各方向の情報
	
	Texture texture;				// アドオン画像のテクスチャ
} AddonTypeStruct;

class Addon {
public:
	Addon();
	
	// 内容の変更
	bool load(FileStruct file_path, string loading_addons_set_name);
	
	// 名前の取得
	string getName();		// 原名
	string getNameJP();		// 日本語名
	
	// 製作者名の取得
	string getAuthorName();
	
	// 説明文の取得
	string getSummary();
	
	// Typeの名前の取得
	string getTypeName(int type_num);
	
	// Directionの名前の取得
	string getDirectionName(int type_num, int direction_num);
	
	// アイコンのImageStructの取得
	ImageStruct getIconImageStruct();
	
	// アドオンを描画する
	void draw(string type_name, string direction_name, PositionStruct position, CoordinateStruct use_tiles, CoordinateStruct tiles_count);
	
private:
	// アドオン名
	string addon_name;				// 英語名
	string addon_jp_name;			// 日本語名
	
	// 製作者名
	string addon_author;
	
	// 説明文
	string addon_summary;
	
	// 所属するアドオンセットの名前（空白はNormalとみなす）
	string belong_addons_set_name;
	
	// アドオンのタイプ
	string addon_type;
	
	// アイコン画像のパス
	string addon_icon;
	
	// 使用するtype
	Array<string> use_types;
	
	// 使用するdirection
	Array<Array<string>> directions_name;
	
	// 収容人数
	int maxium_capacity;
	
	// 地価
	int land_price_influence;		// 上昇額
	int land_price_influence_grid;	// 地価の上下が影響するマス
	
	// 各typeの情報
	map<string, AddonTypeStruct> types;
	
	
	// プライベート関数
	bool getElement(string str, string search_element_name, string& ret);
	bool getElement(string str, string search_element_name, int& ret);
	bool getTypes(string str, string search_element_name, Array<string>& ret);
	void set_alpha_color(Image& image_temp, Color transparent_rgb);
};

#endif /* Addon_hpp */
