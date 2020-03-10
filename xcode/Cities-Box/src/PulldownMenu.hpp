//
//  PulldownMenu.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/02/13.
//

#ifndef PulldownMenu_hpp
#define PulldownMenu_hpp

#include "Header.hpp"

class PulldownMenu {
public:
	PulldownMenu(Array<String>& argList, Font& argFont, SizeStruct argSize, PositionStruct argPosition);
	
	void draw();
	
	int selectedNumber();
	
private:
	Array<String> list;
	Font font;
	SizeStruct size;
	PositionStruct position;
	int selectedElementNum;
	bool pullingDown;
};

#endif /* PulldownMenu_hpp */
