#include <Specific.hpp>
#include <CitiesBox.hpp>
#include <Graph.hpp>

void Main() {
	// グラフのテスト
	Graph graph(100, 100);
	graph.addNode(CoordinateStruct{1, 2});
	graph.addNode(CoordinateStruct{1, 3}, CoordinateStruct{1, 2}, true);
	graph.addNode(CoordinateStruct{1, 4}, CoordinateStruct{1, 3}, false);
	cout << "(1,2)(1,3)" << endl;
	cout << graph.isNodesConnected(CoordinateStruct{1, 2}, CoordinateStruct{1, 3}) << endl;
	cout << graph.isNodesConnected(CoordinateStruct{1, 3}, CoordinateStruct{1, 2}) << endl;
	cout << "(1,2)(1,4)" << endl;
	cout << graph.isNodesConnected(CoordinateStruct{1, 2}, CoordinateStruct{1, 4}) << endl;
	cout << graph.isNodesConnected(CoordinateStruct{1, 4}, CoordinateStruct{1, 2}) << endl;
	cout << "(1,3)(1,4)" << endl;
	cout << graph.isNodesConnected(CoordinateStruct{1, 3}, CoordinateStruct{1, 4}) << endl;
	cout << graph.isNodesConnected(CoordinateStruct{1, 4}, CoordinateStruct{1, 3}) << endl;
	
	graph.addNode(CoordinateStruct{1, 6}, CoordinateStruct{1, 4}, true);
	cout << "(1,5)(1,6)" << endl;
	cout << graph.isNodesConnected(CoordinateStruct{1, 6}, CoordinateStruct{1, 5}) << endl;
	cout << graph.isNodesConnected(CoordinateStruct{1, 5}, CoordinateStruct{1, 6}) << endl;
	
	CitiesBox();
}
