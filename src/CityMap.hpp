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
#include "CBAddon.hpp"
#include "UnitaryTools.hpp"
#include "Tile.hpp"
#include "NormalObject.hpp"
#include "ConnectableObject.hpp"
#include "CityNetwork.hpp"
#include <rs-citymap.h>

class CityMap {
public:
	// コンストラクタ
	CityMap()
		: m_rust_core(rust::citymap::new_city_map()) // Rust側のオブジェクトを初期化
	{}

	// マップの読み込み
	void load(String loadMapFilePath);
	
	// カテゴリに当てはまるアドオンの一覧を返す
	Array<CBAddon*> getFitAddons(Array<CategoryID::Type> selectedCategories);
	
	// アドオンリストを返す
	map<String, CBAddon*> getAllAddons();
	
	// 読込中画面
	void loadingScreen();
	
	// マップの描画
	void draw(CameraStruct camera, CursorStruct& cursor, bool window_size_changed);
	
	// Addon構造体の取得
	Array<CBAddon> getAddon(CoordinateStruct coordinate);
	
	// 需要度の取得
	RCOIFstruct getDemand();
	
	// マップサイズの取得
	Size getMapSize();
	
	// いずれかのアドオンがカテゴリに含まれているか
	bool hasCategory(CategoryID::Type searchCategory, CoordinateStruct coordinate);
	
	// アドオンを更新
	void update(CoordinateStruct position, ObjectStruct* object_struct, Array<CoordinateStruct>& needUpdate);
    
    // アドオンを設置
    bool build(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak);
    
    // アドオンを除去
    void breaking(CoordinateStruct coordinate, bool isTemporaryDelete, bool updateAroundTiles, bool deleteThis);
    
    // 建設状態のアップデート
    void breakUnconnectedRoads();
    
    // 座標から描画位置、描画位置から座標を取得
    CoordinateStruct positionToCoordinate(PositionStruct position, CameraStruct camera);
    PositionStruct coordinateToPosition(CoordinateStruct coordinate, CameraStruct camera);
	
	// レート表示モード
	void setShowRate(RateID::Type effect_id);
	
	// マップ保存
	bool save();
	
	// メモリ解放
	void freeMapAndAddons();
	
	// 人口の取得 : Rust 側の実装
	int getPopulation() { 
		return m_rust_core->get_population(); 
	}

	// 資金の取得 : Rust 側の実装
	int getMoney() { 
		return m_rust_core->get_money(); 
	}
	
	// 時間を進ませて取得 : Rust 側の実装
	TimeStruct cityTime(int minutesDelta) {
		return m_rust_core->city_time(minutesDelta);
	}

	// 気温の取得 : Rust 側の実装
	int getTemperature() {
		return m_rust_core->get_temperature();
	}
	
private:
	/* プライベート関数 */
	bool m_get_element(String str, String searchElementName, String& ret);
	bool m_get_element(String str, String searchElementName, int& ret);
	bool m_get_element(String str, String searchElementName, bool& ret);
	bool m_get_types(String str, String searchElementName, Array<String>& ret);
    bool m_is_there_crossable_object(CBAddon *addon, CoordinateStruct coordinate);
    
    // マップの読み込み
    void m_load_CBJ(String loadMapFilePath);
    
    // アドオンの読み込み
    void m_load_addons(String addonSetName);
    
    // 対象物の接続可能なタイプを取得
    CategoryID::Type m_get_connectable_CategoryID(CBAddon* addon);
    CategoryID::Type m_get_connectable_CategoryID_explicitly(CBAddon* addon);
    
    // 描画範囲を取得
    pair<CoordinateStruct, CoordinateStruct> m_get_draw_area(CameraStruct camera, bool window_size_changed);
    
    // アドオンを設置
    bool m_build_connectable_type(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak);
    bool m_update_connection_type(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak);
    bool m_build_building(CursorStruct cursor, CursorStruct before_cursor, CBAddon* selectedAddon, bool needToBreak);
    
    // 道路を接続
    void m_connect_objects(CoordinateStruct from, CoordinateStruct to, int object_id);
    
    // 効果の指定
    void m_set_rate(Object* arg_object, CoordinateStruct arg_origin_coordinate, bool will_be_deleted);
    
    // アドオンを除去
    void m_break_only_category(CategoryID::Type category, CoordinateStruct coordinate, bool isTemporaryDelete, bool updateAroundTiles, bool deleteThis);
    void m_break_once(ObjectStruct &object_struct, CoordinateStruct coordinate, bool isTemporaryDelete, bool updateAroundTiles, bool deleteThis);
    
    // 道路の敷設の可否、TypeID, DirectionIDの設定
    bool m_can_build_road_here(CoordinateStruct coordinate);
    TypeID::Type m_set_road_type(CoordinateStruct coordinate, CBAddon* addon);
    DirectionID::Type m_set_road_direction(CoordinateStruct coordinate, CBAddon* addon);
    
    // 建物の建設の可否、TypeID, DirectionIDの設定
    tuple<bool, TypeID::Type, DirectionID::Type> m_can_build_building_here(CoordinateStruct coordinate, CBAddon* addon);
    
    // アドオンの接続状態を更新
    Array<CoordinateStruct> m_update_connected_tiles(CoordinateStruct position);
    
    // 踏切を設置（道路と線路が交差していれば）
    bool m_put_train_crossing(CBAddon* addon, CoordinateStruct coordinate, TypeID::Type &type);
    
    // 橋を設置（道路/線路と水路が交差していれば）
    bool m_put_bridge(CBAddon* addon, CoordinateStruct coordinate, TypeID::Type &type);
    
	// 次のObjectID
	int m_get_next_objectID();
	
	// 芝生を置く
	void m_put_grass(CoordinateStruct arg_coordinate);
	
private:
	// Rust 側のコアロジック
	rust::Box<rust::citymap::RustCityMap> m_rust_core;

	// 道路ネットワーク
	CityNetwork road_network;
	
	/* プライベート変数 */
	int m_saved_version;
	String m_addon_set_name;
	
	String m_city_name;
	String m_mayor_name;
	bool m_change_weather;
	bool m_dark_on_night;
	
	Size m_map_size;
	
	RCOIFstruct m_demand;
	
	BudgetStruct m_budget;
	RCOIFstruct m_tax;
	
	Array<Array<Tile>> m_tiles;
	
	bool m_loading_complete;
	
	map<String, CBAddon*> m_addons;
	
	map<int, Object*> m_objects;
    map<String, Object*> m_common_objects;
	int m_max_object_id = 0;
	
    Array<Object*> m_constructing_connectable_objects;
	
	CameraStruct m_camera_before;
	pair<CoordinateStruct, CoordinateStruct> m_range;
	
	String m_map_file_path;
	
	RateID::Type m_show_rate = RateID::None;
};

#endif /* CityMap_hpp */
