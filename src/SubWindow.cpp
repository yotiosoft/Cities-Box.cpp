//
//  SubWindow.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/04.
//

#include "SubWindow.hpp"

SubWindow::SubWindow(String newTitle, Font* newFont, Size newSize, Color backgroundColor) {
	m_size = newSize;
	m_render = RenderTexture(m_size.x, m_size.y, backgroundColor);
	
	backgroundColor.a = 240;
	m_render_window = RenderTexture(m_size.x, m_size.y+newFont->fontSize()+6, backgroundColor);
	
	m_position.x = Scene::Width()/2-m_size.x/2;
	m_position.y = Scene::Height()/2-m_size.y/2;
	
	m_title = newTitle;
	m_font = newFont;
	
	m_moving = false;
	
	// テンプレートの描画
	ScopedRenderTarget2D target(m_render_window);
	Rect(0, 0, m_size.x, m_font->fontSize()+6).draw(Color(9, 132, 227));
	
	(*m_font)(m_title).draw(2, 2);
}

RenderTexture* SubWindow::getRenderTexture() {
	return &m_render;
}

void SubWindow::move(PositionStruct newPosition) {
	m_position = newPosition;
}

void SubWindow::update() {
	ScopedRenderTarget2D target(m_render_window);
	m_render.draw(0, m_font->fontSize()+6+1);
}

void SubWindow::draw() {
	if (MouseL.pressed() && Cursor::Pos().x >= m_position.x && Cursor::Pos().y >= m_position.y
		&& Cursor::Pos().x <= m_position.x+m_size.x && Cursor::Pos().y <= m_position.y+m_font->fontSize()+6) {
		m_moving = true;
	}
	
	if (m_moving) {
		if (!MouseL.pressed()) {
			m_moving = false;
		}
		else {
			m_position.x += Cursor::Delta().x;
			m_position.y += Cursor::Delta().y;
		}
	}
	
	m_render_window.draw(m_position.x, m_position.y);
}
