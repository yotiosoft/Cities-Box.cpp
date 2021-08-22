//
//  Graph.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/23.
//

#include "Graph.hpp"

// コンストラクタ
Graph::Graph(int arg_width, int arg_height) {
	graph.resize(arg_width, arg_height, nullptr);
}

// ノードの追加
bool Graph::addNode(CoordinateStruct coordinate) {
	// coordinate上を通過枝を探索
	// 2次元なので縦横だけ確認すれば良い
	// y軸固定、x軸変動
	int nearest_west_x = -1;
	for (int x=0; x<coordinate.x; x++) {
		if (graph[coordinate.y][x] != nullptr) {
			nearest_west_x = x;
		}
	}
	int nearest_east_x = -1;
	for (int x=(int)graph.width(); x>coordinate.x; x--) {
		if (graph[coordinate.y][x] != nullptr) {
			nearest_east_x = x;
		}
	}
	// x軸固定、y軸変動
	int nearest_north_y = -1;
	for (int y=0; y<coordinate.y; y++) {
		if (graph[y][coordinate.x] != nullptr) {
			nearest_north_y = y;
		}
	}
	int nearest_south_y = -1;
	for (int y=(int)graph.width(); y>coordinate.y; y--) {
		if (graph[y][coordinate.x] != nullptr) {
			nearest_south_y = y;
		}
	}
	
	// ノードを生成しないパターン：枝の直線上に座標が存在する場合
	// -> 枝を生成せずに戻る
	if (nearest_west_x != nearest_east_x) {
		return false;
	}
	if (nearest_north_y != nearest_south_y) {
		return false;
	}
	
	// ノードを生成するパターン：枝の端となる、あるいは交差する、または曲がり角となる
	GraphNode* new_node = new GraphNode();
	new_node->coordinate = coordinate;
	if (nearest_west_x != -1) {
		new_node->connections[DirectionID::West] = graph[coordinate.y][nearest_west_x];
	}
	if (nearest_east_x != -1) {
		new_node->connections[DirectionID::East] = graph[coordinate.y][nearest_east_x];
	}
	if (nearest_north_y != -1) {
		new_node->connections[DirectionID::North] = graph[nearest_north_y][coordinate.x];
	}
	if (nearest_south_y != -1) {
		new_node->connections[DirectionID::South] = graph[nearest_south_y][coordinate.x];
	}
	graph[coordinate.y][coordinate.x] = new_node;
	
	return true;
}

void Graph::clear() {
	for (int y=0; y<graph.height(); y++) {
		for (int x=0; x<graph.width(); x++) {
			if (graph[y][x] != nullptr) {
				delete(graph[y][x]);
			}
		}
	}
}

int Graph::m_count_unconnected(int x1, int x2, int y1, int y2) {
	int count = 0;
	if (x1 == -1)
		count ++;
	if (x2 == -1)
		count ++;
	if (y1 == -1)
		count ++;
	if (y2 == -1)
		count ++;
	
	return count;
}
