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
	PulldownMenu(Array<String>& arg_list, Font& arg_font, SizeStruct arg_size, PositionStruct arg_position);
	
	void draw();
	
	int selected_number();
	
private:
	Array<String> list;
	Font font;
	SizeStruct size;
	PositionStruct position;
	int selected_element_num;
	bool pulling_down;
};

#endif /* PulldownMenu_hpp */
