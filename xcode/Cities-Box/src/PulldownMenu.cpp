//
//  PulldownMenu.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/02/13.
//

#include "PulldownMenu.hpp"

PulldownMenu::PulldownMenu(Array<String>& argList, Font& argFont, SizeStruct argSize, PositionStruct argPosition) {
	list = argList;
	font = argFont;
	size = argSize;
	position = argPosition;
	selectedElementNum = 0;
	pullingDown = false;
}

void PulldownMenu::draw() {
	Rect(position.x, position.y, size.width, size.height).draw(Palette::White).drawFrame(1, 1, Palette::Black);
	font(list[selectedElementNum]).draw(position.x+10, position.y+size.height/2-font(list[selectedElementNum]).region(0, 0).h/2, Palette::Black);
	Triangle(position.x+size.width-30, position.y+size.height/2-5, position.x+size.width-20, position.y+size.height/2-5, position.x+size.width-25, position.y+size.height/2+5).draw(Palette::Black);
	
	if (Cursor::Pos().x >= position.x && Cursor::Pos().y >= position.y && Cursor::Pos().x <= position.x+size.width && Cursor::Pos().y <= position.y+size.height) {
		if (MouseL.down()) {
			if (pullingDown) {
				pullingDown = false;
			}
			else {
				pullingDown = true;
			}
		}
	}
	
	if (pullingDown) {
		Rect(position.x, position.y+size.height, size.width, (font(list[selectedElementNum]).region(0, 0).h + 4) * list.size()).draw(Palette::Snow);
		
		for (int i=0; i<list.size(); i++) {
			int elementSizeH = font(list[i]).region(0, 0).h+4;
			
			if (Cursor::Pos().x >= position.x && Cursor::Pos().y >= position.y+size.height+elementSizeH*i && Cursor::Pos().x <= position.x+size.width && Cursor::Pos().y <= position.y+size.height+elementSizeH*(i+1)-1) {
				if (MouseL.down()) {
					selectedElementNum = i;
					pullingDown = false;
				}
				else {
					if (i == selectedElementNum) {
						Rect(position.x, position.y+size.height+elementSizeH * i, size.width, elementSizeH).draw(Palette::Blue);
						font(list[i]).draw(position.x+10, position.y+size.height+elementSizeH * i + 2, Palette::White);
					}
					else {
						Rect(position.x, position.y+size.height+elementSizeH * i, size.width, elementSizeH).draw(Palette::Skyblue);
						font(list[i]).draw(position.x+10, position.y+size.height+elementSizeH * i + 2, Palette::Black);
					}
				}
			}
			else {
				if (i == selectedElementNum) {
					Rect(position.x, position.y+size.height+elementSizeH * i, size.width, elementSizeH).draw(Palette::Blue);
					font(list[i]).draw(position.x+10, position.y+size.height+elementSizeH * i + 2, Palette::White);
				}
				else {
					font(list[i]).draw(position.x+10, position.y+size.height+elementSizeH * i + 2, Palette::Black);
				}
			}
		}
	}
}

int PulldownMenu::selectedNumber() {
	return selectedElementNum;
}
