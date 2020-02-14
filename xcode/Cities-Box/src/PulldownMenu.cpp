//
//  PulldownMenu.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/02/13.
//

#include "PulldownMenu.hpp"

PulldownMenu::PulldownMenu(Array<String>& arg_list, Font& arg_font, SizeStruct arg_size, PositionStruct arg_position) {
	list = arg_list;
	font = arg_font;
	size = arg_size;
	position = arg_position;
	selected_element_num = 0;
	pulling_down = false;
}

void PulldownMenu::draw() {
	Rect(position.x, position.y, size.width, size.height).draw(Palette::White).drawFrame(1, 1, Palette::Black);
	font(list[selected_element_num]).draw(position.x+10, position.y+size.height/2-font(list[selected_element_num]).region(0, 0).h/2, Palette::Black);
	Triangle(position.x+size.width-30, position.y+size.height/2-5, position.x+size.width-20, position.y+size.height/2-5, position.x+size.width-25, position.y+size.height/2+5).draw(Palette::Black);
	
	if (Cursor::Pos().x >= position.x && Cursor::Pos().y >= position.y && Cursor::Pos().x <= position.x+size.width && Cursor::Pos().y <= position.y+size.height) {
		if (MouseL.down()) {
			if (pulling_down) {
				pulling_down = false;
			}
			else {
				pulling_down = true;
			}
		}
	}
	
	if (pulling_down) {
		Rect(position.x, position.y+size.height, size.width, (font(list[selected_element_num]).region(0, 0).h + 4) * list.size()).draw(Palette::Snow);
		
		for (int i=0; i<list.size(); i++) {
			int element_size_y = font(list[i]).region(0, 0).h+4;
			
			if (Cursor::Pos().x >= position.x && Cursor::Pos().y >= position.y+size.height+element_size_y*i && Cursor::Pos().x <= position.x+size.width && Cursor::Pos().y <= position.y+size.height+element_size_y*(i+1)-1) {
				if (MouseL.down()) {
					selected_element_num = i;
					pulling_down = false;
				}
				else {
					if (i == selected_element_num) {
						Rect(position.x, position.y+size.height+element_size_y * i, size.width, element_size_y).draw(Palette::Blue);
						font(list[i]).draw(position.x+10, position.y+size.height+element_size_y * i + 2, Palette::White);
					}
					else {
						Rect(position.x, position.y+size.height+element_size_y * i, size.width, element_size_y).draw(Palette::Skyblue);
						font(list[i]).draw(position.x+10, position.y+size.height+element_size_y * i + 2, Palette::Black);
					}
				}
			}
			else {
				if (i == selected_element_num) {
					Rect(position.x, position.y+size.height+element_size_y * i, size.width, element_size_y).draw(Palette::Blue);
					font(list[i]).draw(position.x+10, position.y+size.height+element_size_y * i + 2, Palette::White);
				}
				else {
					font(list[i]).draw(position.x+10, position.y+size.height+element_size_y * i + 2, Palette::Black);
				}
			}
		}
	}
}

int PulldownMenu::selected_number() {
	return selected_element_num;
}
