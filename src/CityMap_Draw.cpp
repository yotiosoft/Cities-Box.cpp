//
//  CityMap_Draw.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/26.
//

#include "CityMap.hpp"

void CityMap::draw(CameraStruct camera, CursorStruct& cursor, bool window_size_changed) {
	// 削除されたオブジェクトをオブジェクトリストから除去
	for (auto removing_object : remove_objects_list) {
		if (m_objects.count(removing_object) > 0) {
			m_objects.erase(removing_object);
		}
	}

	// マップを描画
	for (short int y = m_get_draw_area(camera, window_size_changed).first.y; y < m_get_draw_area(camera, window_size_changed).second.y; y++) {
		for (short int x = m_get_draw_area(camera, window_size_changed).first.x; x < m_get_draw_area(camera, window_size_changed).second.x; x++) {
			PositionStruct drawPos = coordinateToPosition(CoordinateStruct{ x, y }, camera);

			// 一マス分描画
			if (drawPos.x >= -CHIP_SIZE && drawPos.y >= -CHIP_SIZE / 2 && drawPos.x <= Scene::Width() && drawPos.y <= Scene::Height() + CHIP_SIZE * 2) {
				m_tiles[y][x].draw(m_show_rate, drawPos, m_time_now);
			}

			// カーソルの描画
			if (x == cursor.coordinate.x && y == cursor.coordinate.y) {
				cursor.texture->draw(cursor.position_per_tiles.x, cursor.position_per_tiles.y, Alpha(128));
			}
			
			// テスト用
			/*
			cout << cursor.coordinate.x << "," << cursor.coordinate.y << endl << "{";
			for (auto object_st : m_tiles[cursor.coordinate.y][cursor.coordinate.x].getObjectStructs()) {
				cout << object_st.object_p->getOriginCoordinate().x << "," << object_st.object_p->getOriginCoordinate().y;
				//cout << " / " << object_st.relative_coordinate.origin.x << "," << object_st.relative_coordinate.origin.y << " " << endl;
				cout << "TypeID: " << object_st.object_p->getTypeID() << " / DirectionID: " << object_st.object_p->getDirectionID() << endl;
			}
			cout << "}" << endl;*/
		}
	}
}

PositionStruct CityMap::coordinateToPosition(CoordinateStruct coordinate, CameraStruct camera) {
	// カメラの座標がデフォルト値（64*mapsize.width/2-Scene::Width()/2, 0）のときの描画位置を算出
	CameraStruct defaultCamera;
	defaultCamera.position = PositionStruct{ 0, 0 };
	defaultCamera.center = { 0, 0 };

	// 基準点となるx:0, y:0のマスの表示位置を算出する
	PositionStruct tile0x0Position = PositionStruct{ defaultCamera.position.x - camera.position.x,
		defaultCamera.position.y - camera.position.y };

	PositionStruct tilePosition = PositionStruct{ tile0x0Position.x + coordinate.x * 32 - coordinate.y * 32, tile0x0Position.y + coordinate.y * 16 + coordinate.x * 16 };

	return tilePosition;
}

CoordinateStruct CityMap::positionToCoordinate(PositionStruct position, CameraStruct camera) {
	int tempMX = position.x + camera.position.x - CHIP_SIZE / 2;
	int tempMY = position.y + camera.position.y;

	CoordinateStruct ret;
	ret.x = (tempMY + tempMX / 2) / (CHIP_SIZE / 2);
	ret.y = (-tempMX + tempMY * 2) / CHIP_SIZE;

	if (ret.x < 0) {
		ret.x = 0;
	}
	if (ret.y < 0) {
		ret.y = 0;
	}

	if (ret.x >= m_map_size.x) {
		ret.x = m_map_size.x - 1;
	}
	if (ret.y >= m_map_size.y) {
		ret.y = m_map_size.y - 1;
	}

	return ret;
}

pair<CoordinateStruct, CoordinateStruct> CityMap::m_get_draw_area(CameraStruct camera, bool window_size_changed) {
	// カメラの座標が変わっていなければrangeを返す
	if (!window_size_changed && camera.position.x == m_camera_before.position.x && camera.position.y == m_camera_before.position.y) {
		return m_range;
	}
	m_camera_before = camera;

	// 描画できる範囲
	int rangeOneDirection = (sqrt(powf(Scene::Width(), 2) + powf(Scene::Height(), 2)) / sqrt(powf(CHIP_SIZE / 2, 2) + powf(CHIP_SIZE / 4, 2)));

	// 画面中央の座標
	CoordinateStruct centerCoordinate = positionToCoordinate(PositionStruct{ Scene::Width() / 2, Scene::Height() / 2 }, camera);

	pair<CoordinateStruct, CoordinateStruct> ret;
	// 左上の座標
	ret.first = CoordinateStruct{ centerCoordinate.x - rangeOneDirection, centerCoordinate.y - rangeOneDirection };

	if (ret.first.x < 0) {
		ret.first.x = 0;
	}
	if (ret.first.y < 0) {
		ret.first.y = 0;
	}
	if (ret.first.x >= m_map_size.x) {
		ret.first.x = m_map_size.x - 1;
	}
	if (ret.first.y >= m_map_size.y) {
		ret.first.y = m_map_size.y - 1;
	}

	// 右下の座標
	ret.second = CoordinateStruct{ centerCoordinate.x + rangeOneDirection, centerCoordinate.y + rangeOneDirection };

	if (ret.second.x < 0) {
		ret.second.x = 0;
	}
	if (ret.second.y < 0) {
		ret.second.y = 0;
	}
	if (ret.second.x >= m_map_size.x) {
		ret.second.x = m_map_size.x - 1;
	}
	if (ret.second.y >= m_map_size.y) {
		ret.second.y = m_map_size.y - 1;
	}

	m_range = ret;

	return ret;
}

void CityMap::setShowRate(RateID::Type rate_id) {
	m_show_rate = rate_id;
}
