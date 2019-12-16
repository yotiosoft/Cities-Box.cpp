//
//  SubWindow.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/04.
//

#include "SubWindow.hpp"

SubWindow::SubWindow(String new_title, Font* new_font, SizeStruct new_size, Color background_color) {
	size = new_size;
	render = RenderTexture(size.width, size.height, background_color);
	
	background_color.a = 240;
	render_window = RenderTexture(size.width, size.height+new_font->fontSize()+6, background_color);
	
	position.x = Scene::Width()/2-size.width/2;
	position.y = Scene::Height()/2-size.height/2;
	
	title = new_title;
	font = new_font;
	
	moving = false;
	
	// テンプレートの描画
	ScopedRenderTarget2D target(render_window);
	Rect(0, 0, size.width, font->fontSize()+6).draw(Color(9, 132, 227));
	
	(*font)(title).draw(2, 2);
}

RenderTexture* SubWindow::getRenderTexture() {
	return &render;
}

void SubWindow::move(PositionStruct new_position) {
	position = new_position;
}

void SubWindow::update() {
	ScopedRenderTarget2D target(render_window);
	render.draw(0, font->fontSize()+6+1);
}

void SubWindow::draw() {
	if (MouseL.pressed() && Cursor::Pos().x >= position.x && Cursor::Pos().y >= position.y
		&& Cursor::Pos().x <= position.x+size.width && Cursor::Pos().y <= position.y+font->fontSize()+6) {
		moving = true;
	}
	
	if (moving) {
		if (!MouseL.pressed()) {
			moving = false;
		}
		else {
			position.x += Cursor::Delta().x;
			position.y += Cursor::Delta().y;
		}
	}
	
	render_window.draw(position.x, position.y);
}
