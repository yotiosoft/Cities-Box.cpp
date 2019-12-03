//
//  OtherFunctions.cpp
//

#include "OtherFunctions.hpp"

Array<string> split(string str, string separator) {
	auto separator_length = separator.length();
	
	Array<string> list = Array<string>();
	
	if (separator_length == 0) {
		list.push_back(str);
	} else {
		auto offset = std::string::size_type(0);
		while (1) {
			auto pos = str.find(separator, offset);
			if (pos == std::string::npos) {
				list.push_back(str.substr(offset));
				break;
			}
			list.push_back(str.substr(offset, pos - offset));
			offset = pos + separator_length;
		}
	}
	
	return list;
}

void cMes(Font& font, String str, PositionStruct top_left, PositionStruct bottom_right, Color& color) {
	int x = (bottom_right.x+top_left.x)/2 - font(str).region(Scene::Width()/2, Scene::Height()/2).w/2;
	int y = (bottom_right.y+top_left.y)/2 - font(str).region(Scene::Width()/2, Scene::Height()/2).h/2;
	
	font(str).draw(x, y, color);
}
void cMes(Font& font, String str, PositionStruct top_left, SizeStruct size, Color& color) {
	int x = (top_left.x*2+size.width)/2 - font(str).region(Scene::Width()/2, Scene::Height()/2).w/2;
	int y = (top_left.y*2+size.height)/2 - font(str).region(Scene::Width()/2, Scene::Height()/2).h/2;
	
	font(str).draw(x, y, color);
}
