//
//  StartUp.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/12/01.
//

#include "StartUp.hpp"

void loadImages(Images& images) {
	Color black;
	black.r = 0;
	black.g = 0;
	black.b = 0;
	
	images.load("./data/images/area", "area", black);
	images.load("./data/images/arrows", "arrows", black);
	images.load("./data/images/pointer", "pointer", black);
	images.load("./data/images/quarter", "quarter", black);
	images.load("./data/images/title_menu", "title_menu", black);
}
