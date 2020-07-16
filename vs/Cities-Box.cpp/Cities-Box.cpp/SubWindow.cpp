//
//  SubWindow.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/04.
//

#include "SubWindow.hpp"

SubWindow::SubWindow(String newTitle, Font* newFont, SizeStruct newSize, Color backgroundColor) {
	size = newSize;
	render = RenderTexture(size.width, size.height, backgroundColor);
	
	backgroundColor.a = 240;
	renderWindow = RenderTexture(size.width, size.height+newFont->fontSize()+6, backgroundColor);
	
	position.x = Scene::Width()/2-size.width/2;
	position.y = Scene::Height()/2-size.height/2;
	
	title = newTitle;
	font = newFont;
	
	moving = false;
	
	// テンプレートの描画
	ScopedRenderTarget2D target(renderWindow);
	Rect(0, 0, size.width, font->fontSize()+6).draw(Color(9, 132, 227));
	
	(*font)(title).draw(2, 2);
}

RenderTexture* SubWindow::getRenderTexture() {
	return &render;
}

void SubWindow::move(PositionStruct newPosition) {
	position = newPosition;
}

void SubWindow::update() {
	ScopedRenderTarget2D target(renderWindow);
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
	
	renderWindow.draw(position.x, position.y);
}
