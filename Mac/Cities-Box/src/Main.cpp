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
	cout << graph.isConnected(CoordinateStruct{1, 2}, CoordinateStruct{1, 3}) << endl;
	cout << graph.isConnected(CoordinateStruct{1, 3}, CoordinateStruct{1, 2}) << endl;
	cout << "(1,2)(1,4)" << endl;
	cout << graph.isConnected(CoordinateStruct{1, 2}, CoordinateStruct{1, 4}) << endl;
	cout << graph.isConnected(CoordinateStruct{1, 4}, CoordinateStruct{1, 2}) << endl;
	cout << "(1,3)(1,4)" << endl;
	cout << graph.isConnected(CoordinateStruct{1, 3}, CoordinateStruct{1, 4}) << endl;
	cout << graph.isConnected(CoordinateStruct{1, 4}, CoordinateStruct{1, 3}) << endl;
	
	CitiesBox();
}
