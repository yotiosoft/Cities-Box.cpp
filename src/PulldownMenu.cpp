//
//  PulldownMenu.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/02/13.
//

#include "PulldownMenu.hpp"

PulldownMenu::PulldownMenu(Array<String>& argList, Font& argFont, Size argSize, PositionStruct argPosition) {
	m_list = argList;
	m_font = argFont;
	m_size = argSize;
	m_position = argPosition;
	m_selected_element_num = 0;
	m_pulling_down = false;
}

void PulldownMenu::draw() {
	Rect(m_position.x, m_position.y, m_size.x, m_size.y).draw(Palette::White).drawFrame(1, 1, Palette::Black);
	m_font(m_list[m_selected_element_num]).draw(m_position.x+10, m_position.y+m_size.y/2-m_font(m_list[m_selected_element_num]).region(0, 0).h/2, Palette::Black);
	Triangle(m_position.x+m_size.x-30, m_position.y+m_size.y/2-5, m_position.x+m_size.x-20, m_position.y+m_size.y/2-5, m_position.x+m_size.x-25, m_position.y+m_size.y/2+5).draw(Palette::Black);
	
	if (Cursor::Pos().x >= m_position.x && Cursor::Pos().y >= m_position.y && Cursor::Pos().x <= m_position.x+m_size.x && Cursor::Pos().y <= m_position.y+m_size.y) {
		if (MouseL.down()) {
			if (m_pulling_down) {
				m_pulling_down = false;
			}
			else {
				m_pulling_down = true;
			}
		}
	}
	
	if (m_pulling_down) {
		Rect(m_position.x, m_position.y+m_size.y, m_size.x, (m_font(m_list[m_selected_element_num]).region(0, 0).h + 4) * m_list.size()).draw(Palette::Snow);
		
		for (int i=0; i<m_list.size(); i++) {
			int elementSizeH = m_font(m_list[i]).region(0, 0).h+4;
			
			if (Cursor::Pos().x >= m_position.x && Cursor::Pos().y >= m_position.y+m_size.y+elementSizeH*i && Cursor::Pos().x <= m_position.x+m_size.x && Cursor::Pos().y <= m_position.y+m_size.y+elementSizeH*(i+1)-1) {
				if (MouseL.down()) {
					m_selected_element_num = i;
					m_pulling_down = false;
				}
				else {
					if (i == m_selected_element_num) {
						Rect(m_position.x, m_position.y+m_size.y+elementSizeH * i, m_size.x, elementSizeH).draw(Palette::Blue);
						m_font(m_list[i]).draw(m_position.x+10, m_position.y+m_size.y+elementSizeH * i + 2, Palette::White);
					}
					else {
						Rect(m_position.x, m_position.y+m_size.y+elementSizeH * i, m_size.x, elementSizeH).draw(Palette::Skyblue);
						m_font(m_list[i]).draw(m_position.x+10, m_position.y+m_size.y+elementSizeH * i + 2, Palette::Black);
					}
				}
			}
			else {
				if (i == m_selected_element_num) {
					Rect(m_position.x, m_position.y+m_size.y+elementSizeH * i, m_size.x, elementSizeH).draw(Palette::Blue);
					m_font(m_list[i]).draw(m_position.x+10, m_position.y+m_size.y+elementSizeH * i + 2, Palette::White);
				}
				else {
					m_font(m_list[i]).draw(m_position.x+10, m_position.y+m_size.y+elementSizeH * i + 2, Palette::Black);
				}
			}
		}
	}
}

int PulldownMenu::selectedNumber() {
	return m_selected_element_num;
}
