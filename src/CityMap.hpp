//
//  CityMap.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#pragma once

#ifndef CityMap_hpp
#define CityMap_hpp

#include <Specific.hpp>
#include "Addon.hpp"
#include "OtherFunctions.hpp"
#include "Tile.hpp"
#include "Object.hpp"


class CityMap {
public:
	// マップの読み込み
	void load(String loadMapFilePath);
	void loadCBD(String loadMapFilePath);
	void loadCBJ(String loadMapFilePath);
	
	// アドオンの読み込み
	void loadAddons(String addonSetName);
	
	// カテゴリに当てはまるアドオンの一覧を返す
	Array<Addon*> getFitAddons(Array<String> selectedCategories);
	
	// アドオンリストを返す
	map<String, Addon*> getAllAddons();
	
	// 読込中画面
	void loadingScreen();
	
	// マップの描画
	void draw(CameraStruct camera, CursorStruct& cursor);
	
	// Addon構造体の取得
	Array<Addon> getAddon(CoordinateStruct coordinate);
	
	// 需要度の取得
	RCOIFstruct getDemand();
	
	// 人口の取得
	int getPopulation();
	
	// 資金の取得
	int getMoney();
	
	// 気温の取得
	int getTemperature();
	
	// マップサイズの取得
	Size getMapSize();
	
	// 座標から描画位置、描画位置から座標を取得
	CoordinateStruct positionToCoordinate(PositionStruct position, CameraStruct camera);
	PositionStruct coordinateToPosition(CoordinateStruct coordinate, CameraStruct camera);
	
	// 描画範囲を取得
	pair<CoordinateStruct, CoordinateStruct> getDrawArea(CameraStruct camera);
	
	// いずれかのアドオンがカテゴリに含まれているか
	bool hasCategory(String searchCategory, CoordinateStruct coordinate);
	
	// アドオンを設置
	bool build(CoordinateStruct position, Addon* selectedAddon, bool needToBreak);
	
	// 効果の指定
	void setRate(Object* arg_object, CoordinateStruct arg_origin_coordinate, bool will_be_deleted);
	
	// アドオンを更新
	void update(CoordinateStruct position, Addon* selectedAddon, Array<CoordinateStruct>& needUpdate);
	
	// アドオンを除去
	void breaking(CoordinateStruct position, bool isTemporaryDelete);
	
	// アドオンの始点となるマスに移動する
	CoordinateStruct moveToAddonStartTile(CoordinateStruct searchCoordinate, int addonNumber);
	
	// 指定した場所に合うアドオンのTypeとDirectionを取得
	bool getBuildTypeAndDirection(CoordinateStruct coordinate, Addon* selectedAddon, TypeID::Type& retType, DirectionID::Type& retDirection, Array<CoordinateStruct>& needUpdate);
	
	// アドオンを削除
	void clear(CoordinateStruct position);
	
	// 座標がマップ範囲内に入っているか
	bool isPositionAvailable(CoordinateStruct coordinate);
	
	// 時間を進ませて取得
	TimeStruct cityTime(int minutesDelta);
	
	// レート表示モード
	void setShowRate(RateID::Type effect_id);
	
	// マップ保存
	bool save();
	
	// メモリ解放
	void freeMapAndAddons();
	
private:
	/* プライベート関数 */
	bool m_get_element(String str, String searchElementName, String& ret);
	bool m_get_element(String str, String searchElementName, int& ret);
	bool m_get_element(String str, String searchElementName, bool& ret);
	bool m_get_types(String str, String searchElementName, Array<String>& ret);
	
	// 次のObjectID
	int m_get_next_objectID();
	
	// 芝生を置く
	void m_put_grass(CoordinateStruct arg_coordinate);
	
	/* プライベート変数 */
	int m_saved_version;
	String m_addon_set_name;
	
	String m_city_name;
	String m_mayor_name;
	int m_total_population;
	bool m_change_weather;
	int m_temperature;
	bool m_dark_on_night;
	
	Size m_map_size;
	
	TimeStruct m_time_now;
	
	RCOIFstruct m_demand;
	
	int m_money;
	
	BudgetStruct m_budget;
	RCOIFstruct m_tax;
	
	Array<Array<Tile>> m_tiles;
	
	bool m_loading_complete;
	
	map<String, Addon*> m_addons;
	
	map<int, Object> m_objects;
	int m_max_object_id;
	
	CameraStruct m_camera_before;
	pair<CoordinateStruct, CoordinateStruct> m_range;
	
	String m_map_file_path;
	
	RateID::Type m_show_rate = RateID::None;
};

#endif /* CityMap_hpp */
