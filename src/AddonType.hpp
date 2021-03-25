//
//  AddonType.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#ifndef AddonType_hpp
#define AddonType_hpp

#pragma once

#include <Specific.hpp>
#include "AddonLayer.hpp"
#include "AddonDirectionStruct.hpp"

class AddonType {
public:
	// コンストラクタ
	AddonType();
	AddonType(TypeID::Type arg_type_ID);
	
	// 描画
	void draw(TimeStruct time, DirectionID::Type direction_id,
			  PositionStruct position, CoordinateStruct coordinate, Color add_color);
	
	// 最終的に表示する座標を取得
	PositionStruct getPosition(DirectionID::Type directionID, PositionStruct position, Size useTiles, CoordinateStruct tilesCount);
	
	// 暫定
	Texture tempGetTexture(TimeStruct time);
	
	// レイヤのセット
	void setLayers(Array<AddonLayer> arg_layers);
	
	// レイヤを返す
	Array<AddonLayer>& getLayers();
	
	// レイヤ数のカウント
	int countLayers();
	
	// AddonDirectionStructをm_directionsに追加
	void addAddonDirectionStruct(AddonDirectionStruct arg_direction_struct);
	
	// AddonDirectionsStructを取得
	AddonDirectionStruct getDirectionStruct(DirectionID::Type direction_id);
	map<DirectionID::Type, AddonDirectionStruct> getDirectionStructs();
	
	// AddonDirectionIDsを取得
	DirectionID::Type getDirectionID(int num);
	Array<DirectionID::Type> getDirectionIDs();
	
private:
	/* プライベート関数 */
	// 全レイヤに存在するすべてのDirectionIDを取得
	Array<DirectionID::Type> m_get_all_direction_IDs();
	
	// すべての状況に応じたテクスチャを構成
	void m_make_all_textures();
	
	// 保持するm_directionsのDirectionIDを検索
	bool m_is_there(DirectionID::Type direction_id);
	
	// レイヤの重ね合わせ
	void m_over_write(Image& to, Image& from, AddonLayer layer, LayerType::Type making_type);
	
	// 指定したLayerTypeが夕方に該当するか？
	bool m_is_evening(LayerType::Type layer_type);
	
	// 指定したLayerTypeが夜間に該当するか？
	bool m_is_night(LayerType::Type layer_type);
	
	// DirectionIDs
	Array<DirectionID::Type> m_addon_direction_ids;
	
	// デフォルトレイヤの取得
	LayerType::Type m_get_default_layer(LayerType::Type layer_type);
	
	/* プライベート変数 */
	// TypeID
	TypeID::Type m_type_id;
	
	// 方向
	map<DirectionID::Type, AddonDirectionStruct> m_directions;
	
	// レイヤ
	Array<AddonLayer> m_layers;
	
	// 夜間に電気を灯すか？
	bool m_light_on_night;
	
	// 各LayerType独自のレイヤがm_layersに存在するか？
	map<LayerType::Type, bool> m_layer_exists_list;
	
	// アドオン画像のテクスチャ
	map<LayerType::Type, Image> m_images;
	Texture m_texture;
	
	// 更新前の日時のLayerType
	LayerType::Type m_before_layer_type;
};

#endif /* AddonType_hpp */
