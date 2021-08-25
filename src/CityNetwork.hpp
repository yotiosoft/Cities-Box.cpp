//
//  CityNetwork.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/25.
//

#ifndef CityNetwork_hpp
#define CityNetwork_hpp

#include <Specific.hpp>

class CityNetwork {
public:
	// ポリゴンに追加
	bool append(int x, int y);
	
	// ポリゴンから削除
	bool del(int x, int y);
	
	// 探索
	LineString solve(CoordinateStruct start, CoordinateStruct end);
	
private:
	// ネットワーク保持用ポリゴン
	Polygon m_polygon;
	
	// NavMesh
	NavMesh m_navmesh;
};

#endif /* CityNetwork_hpp */
