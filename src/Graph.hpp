//
//  Graph.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/23.
//
#pragma once

#ifndef Graph_hpp
#define Graph_hpp

#include <Specific.hpp>

struct GraphNode {
	CoordinateStruct coordinate;
	Array<GraphNode*> connections;
	
	// 接続先の座標を変更（繋ぎ変え）
	bool changeConnect(CoordinateStruct before, GraphNode* after_node) {
		for (int i=0; i<connections.size(); i++) {
			if (connections[i]->coordinate == before) {
				connections[i] = after_node;
				return true;
			}
		}
		return false;
	}
	
	// 特定の接続先の有無
	bool isConnected(CoordinateStruct to) {
		for (int i=0; i<connections.size(); i++) {
			if (connections[i]->coordinate == to) {
				return true;
			}
		}
		
		return false;
	}
	
	// 特定の接続を解除
	void disconnect(CoordinateStruct connect_coordinate) {
		connections.remove_if([&connect_coordinate](const GraphNode* v) { return v->coordinate == connect_coordinate; });
	}
};

class Graph {
public:
	// コンストラクタ
	Graph();
	Graph(int arg_width, int arg_height);
	
	// ノードの追加
	bool addNode(CoordinateStruct coordinate, CoordinateStruct connect_coordinate = {-1, -1}, bool isBidirectional = false);
	
	// ノードの削除
	bool deleteNode(CoordinateStruct coordinate);
	
	// ノードの座標変更
	bool moveNode(CoordinateStruct before, CoordinateStruct after);
	
	// ノードの接続
	bool connect(CoordinateStruct from, CoordinateStruct to);
	bool connectBidirectionally(CoordinateStruct coordinate1, CoordinateStruct coordinate2);
	
	// 接続状況の確認
	bool isConnected(CoordinateStruct from, CoordinateStruct to);
	
	// 経路探索
	
	// グラフの解放
	void clear();
	
private:
	//int m_count_unconnected(int x1, int x2, int y1, int y2);
	
private:
	// ノード
	Grid<GraphNode*> graph;
};

#endif /* Graph_hpp */
