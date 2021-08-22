//
//  Graph.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/23.
//

#ifndef Graph_hpp
#define Graph_hpp

#include <Specific.hpp>

struct GraphNode {
	CoordinateStruct coordinate;
	map<DirectionID::Type, GraphNode*> connections;
};

class Graph {
public:
	// コンストラクタ
	Graph(int arg_width, int arg_height);
	
	// ノードの追加
	bool addNode(CoordinateStruct coordinate);
	
	// ノードの削除
	bool deleteNode(CoordinateStruct coordinate);
	
	// 経路探索
	
	// グラフの解放
	void clear();
	
private:
	int m_count_unconnected(int x1, int x2, int y1, int y2);
	
private:
	// ノード
	Grid<GraphNode*> graph;
};

#endif /* Graph_hpp */
