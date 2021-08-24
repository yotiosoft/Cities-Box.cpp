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

namespace GraphObjectType {
	enum Type {
		Node,
		Edge
	};
}

struct GraphObject {
	CoordinateStruct coordinate;
	Array<GraphObject*> node_connections;
	Array<GraphObject*> edge_connections;
	GraphObjectType::Type type;
	
	// 接続先の座標を変更（繋ぎ変え）
	bool changeConnect(CoordinateStruct before, GraphObject* after_node) {
		for (int i=0; i<node_connections.size(); i++) {
			if (node_connections[i]->coordinate == before) {
				node_connections[i] = after_node;
				return true;
			}
		}
		return false;
	}
	
	// 特定の接続先の有無
	bool isConnected(CoordinateStruct to) {
		for (int i=0; i<node_connections.size(); i++) {
			if (node_connections[i]->coordinate == to) {
				return true;
			}
		}
		
		return false;
	}
	
	// 特定の接続を解除
	void disconnect(CoordinateStruct connect_coordinate) {
		node_connections.remove_if([&connect_coordinate](GraphObject* v) { return v->coordinate == connect_coordinate; });
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
	
	// 直線の枝を設定
	bool fillEdge(CoordinateStruct from, CoordinateStruct to, bool isBidirectional = false);
	
	// ノードの接続
	bool connectNodes(CoordinateStruct from, CoordinateStruct to);
	bool connectNodesBidirectionally(CoordinateStruct coordinate1, CoordinateStruct coordinate2);
	
	// 枝の接続
	bool connectEdge(CoordinateStruct from, CoordinateStruct to);
	bool connectEdgeBidirectionally(CoordinateStruct coordinate1, CoordinateStruct coordinate2);
	
	// 接続状況の確認
	bool isNodesConnected(CoordinateStruct from, CoordinateStruct to);
	bool isEdgeConnected(CoordinateStruct from, CoordinateStruct to);
	
	// 経路探索
	
	// グラフの解放
	void close();
	
private:
	//int m_count_unconnected(int x1, int x2, int y1, int y2);
	
private:
	// ノード
	Grid<GraphObject*> graph;
};

#endif /* Graph_hpp */
