//
//  Graph.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/23.
//

#include "Graph.hpp"

// コンストラクタ
Graph::Graph() {
	
}
Graph::Graph(int arg_width, int arg_height) {
	graph.resize(arg_width, arg_height, nullptr);
}

// ノードの追加
bool Graph::addNode(CoordinateStruct coordinate, CoordinateStruct connect_coordinate, bool isBidirectional) {
	GraphObject* new_node = new GraphObject();
	new_node->coordinate = coordinate;
	new_node->type = GraphObjectType::Node;
	
	GraphObject* connect_node = graph[connect_coordinate.y][connect_coordinate.x];
	
	// 接続元のノードについて
	// 接続元が存在しないなら: このノードは孤立点
	if (connect_coordinate.x == -1 && connect_coordinate.y == -1) {
		graph[coordinate.y][coordinate.x] = new_node;
		return true;
	}
	
	// 接続元が指定されているが、存在しない場合: エラー
	if (connect_node == nullptr) {
		delete(new_node);
		return false;
	}
	
	// 端点であれば: 接続元ノードを指定された座標に移動
	// ただし、接続方向が双方向同士あるいは単方向同士でなければelseへ
	if (connect_node->connections.size() == 1
		&& isConnected(connect_node->connections[0]->coordinate, connect_coordinate) == isBidirectional) {
		delete(new_node);
		moveNode(connect_coordinate, coordinate);
	}
	// それ以外であれば: 接続元ノードを残し、接続元と接続
	else {
		graph[coordinate.y][coordinate.x] = new_node;
		
		if (isBidirectional) {
			connectNodesBidirectionally(connect_coordinate, coordinate);
		}
		else {
			connectNodes(connect_coordinate, coordinate);
		}
	}
	fillEdge(coordinate, connect_coordinate, isBidirectional);
	
	return true;
	/*
	// coordinate上を通過する枝を探索
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
	
	return true;*/
}

bool Graph::deleteNode(CoordinateStruct coordinate) {
	if (graph[coordinate.y][coordinate.x] == nullptr) {
		return false;
	}
	
	delete(graph[coordinate.y][coordinate.x]);
	graph[coordinate.y][coordinate.x] = nullptr;
	
	return true;
}
	
// ノードの座標変更
bool Graph::moveNode(CoordinateStruct before, CoordinateStruct after) {
	if (graph[before.y][before.x] == nullptr) {
		return false;
	}
	
	GraphObject* node = graph[before.y][before.x];
	node->coordinate = after;
	graph[before.y][before.x] = nullptr;
	graph[after.y][after.x] = node;
	
	return true;
}

// 直線の枝を設定
bool Graph::fillEdge(CoordinateStruct from, CoordinateStruct to, bool isBidirectional) {
	if (from == to) {
		return false;
	}
	
	// 直線のみ対応
	// y軸を変動
	if (from.x == to.x) {
		int x = from.x;
		
		if (from.y > to.y) {
			for (int y=from.y-1; y>to.y; y--) {
				GraphObject* new_graph_object = new GraphObject();
				new_graph_object->coordinate = CoordinateStruct{x, y};
				new_graph_object->type = GraphObjectType::Edge;
				graph[y][x] = new_graph_object;
			}
			for (int y=from.y-1; y>to.y; y--) {
				
			}
		}
		else {
			for (int y=start_coordinate.y+1; y<=end_coordinate.y; y++) {
				if (y!=end_coordinate.y)
					graph[y][x] = new GraphObject{CoordinateStruct{y, x}, {graph[y-1][x]}, GraphObjectType::Edge};
				
				if (isBidirectional) {
					connect(CoordinateStruct{y-1, x}, CoordinateStruct{y, x});
				}
			}
		}
		return true;
	}
	// x軸を変動
	else {
		int y = start_coordinate.y;
		
		if (start_coordinate.x > end_coordinate.x) {
			for (int x=start_coordinate.x-1; x>end_coordinate.x; x--) {
				if (x!=end_coordinate.x)
					graph[y][x] = new GraphObject{CoordinateStruct{y, x}, {graph[y][x+1]}, GraphObjectType::Edge};
				
				if (isBidirectional) {
					connect(CoordinateStruct{y, x+1}, CoordinateStruct{y, x});
				}
			}
		}
		else {
			for (int x=start_coordinate.x+1; x<end_coordinate.x; x++) {
				if (y!=end_coordinate.y)
					graph[y][x] = new GraphObject{CoordinateStruct{y, x}, {graph[y][x-1]}, GraphObjectType::Edge};
				
				if (isBidirectional) {
					connect(CoordinateStruct{y, x-1}, CoordinateStruct{y, x});
				}
			}
		}
		return true;
	}
	
	return false;
}

bool Graph::connectNodes(CoordinateStruct from, CoordinateStruct to) {
	if (graph[from.y][from.x] == nullptr || graph[to.y][to.x] == nullptr) {
		return false;
	}
	
	// すでに接続されているなら何もしない
	if (graph[from.y][from.x]->isConnected(to)) {
		return true;
	}
	
	// 接続
	graph[from.y][from.x]->connections << graph[to.y][to.x];
	
	return true;
}

bool Graph::connectNodesBidirectionally(CoordinateStruct coordinate1, CoordinateStruct coordinate2) {
	bool ret1 = connectNodes(coordinate1, coordinate2);
	bool ret2 = connectNodes(coordinate2, coordinate1);
	
	return ret1 && ret2;
}

bool Graph::connectEdge(CoordinateStruct from, CoordinateStruct to) {
	if (graph[from.y][from.x] == nullptr || graph[to.y][to.x] == nullptr) {
		return false;
	}
	
	// すでに接続されているなら何もしない
	if (graph[from.y][from.x]->isConnected(to)) {
		return true;
	}
	
	// 接続
	graph[from.y][from.x]->connections << graph[to.y][to.x];
	
	return true;
}

bool Graph::connectEdgeBidirectionally(CoordinateStruct coordinate1, CoordinateStruct coordinate2) {
	bool ret1 = connectNodes(coordinate1, coordinate2);
	bool ret2 = connectNodes(coordinate2, coordinate1);
	
	return ret1 && ret2;
}

bool Graph::isNodesConnected(CoordinateStruct from, CoordinateStruct to) {
	if (graph[from.y][from.x] == nullptr) {
		return false;
	}
	if (graph[from.y][from.x]->node_connections.size() == 0) {
		return false;
	}
	
	for (auto from_connect : graph[from.y][from.x]->node_connections) {
		if (from_connect->coordinate == to) {
			return true;
		}
	}
	
	return false;
}

bool Graph::isEdgeConnected(CoordinateStruct from, CoordinateStruct to) {
	if (graph[from.y][from.x] == nullptr) {
		return false;
	}
	if (graph[from.y][from.x]->edge_connections.size() == 0) {
		return false;
	}
	
	for (auto from_connect : graph[from.y][from.x]->edge_connections) {
		if (from_connect->coordinate == to) {
			return true;
		}
	}
	
	return false;
}

void Graph::close() {
	for (int y=0; y<graph.height(); y++) {
		for (int x=0; x<graph.width(); x++) {
			if (graph[y][x] != nullptr) {
				deleteNode(CoordinateStruct{x, y});
			}
		}
	}
}
