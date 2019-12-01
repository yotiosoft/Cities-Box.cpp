//
//  Button.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/12/01.
//

#include "Button.hpp"

Button::Button(ImageStruct& image) {
	size = SizeStruct{image.texture.size().x, image.texture.size().y};
	button_texture = image.texture;
	isIcon = false;
}

Button::Button(ImageStruct& image, int new_size) {
	size = SizeStruct{new_size, new_size};
	button_texture = image.texture.resized(size.width, size.height).texture;
	isIcon = false;
}

Button::Button(IconFont::ID icon_id) {
	size = SizeStruct{64, 64};
	button_texture = Texture(Icon(icon_id, size.width));
	isIcon = true;
}

Button::Button(IconFont::ID icon_id, int new_size) {
	size = SizeStruct{new_size, new_size};
	button_texture = Texture(Icon(icon_id, size.width));
	isIcon = true;
}

void Button::put(PositionStruct new_position) {
	position = new_position;
	
	if (Cursor::Pos().x >= position.x && Cursor::Pos().y >= position.y && Cursor::Pos().x <= position.x+size.width && Cursor::Pos().y <= position.y+size.height) {
		Rect(position.x, position.y, size.width, size.height).draw(Color(0, 162, 232, 128));
	}
	
	if (isIcon) {
		position.x += 3;
		position.y += 3;
	}
	button_texture.draw(position.x, position.y);
}

bool Button::push() {
	if (Cursor::Pos().x >= position.x && Cursor::Pos().y >= position.y && Cursor::Pos().x <= position.x+size.width && Cursor::Pos().y <= position.y+size.height) {
		if (MouseL.down()) {
			return true;
		}
	}
	
	return false;
}
