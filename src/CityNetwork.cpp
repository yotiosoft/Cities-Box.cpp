//
//  CityNetwork.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/08/25.
//

#include "CityNetwork.hpp"

bool CityNetwork::append(int x, int y) {
	const Circle shape(Arg::center(x, y), 1);
	m_polygon.append(shape.asPolygon());
	
	return true;
}
