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
	PulldownMenu(Array<String>& argList, Font& argFont, Size argSize, PositionStruct argPosition);
	
	void draw();
	
	int selectedNumber();
	
private:
	Array<String> m_list;
	Font m_font;
	Size m_size;
	PositionStruct m_position;
	int m_selected_element_num;
	bool m_pulling_down;
};

#endif /* PulldownMenu_hpp */
