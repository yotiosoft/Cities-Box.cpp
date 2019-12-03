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

Button::Button(ImageStruct& image, int new_size, String new_label, Font& new_font) {
	size = SizeStruct{new_size, new_size};
	button_texture = image.texture.resized(size.width, size.height).texture;
	isIcon = false;
	font = new_font;
	label = new_label;
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

Button::Button(IconFont::ID icon_id, int new_size, String new_label, Font& new_font) {
	size = SizeStruct{new_size, new_size};
	button_texture = Texture(Icon(icon_id, size.width));
	isIcon = true;
	font = new_font;
	label = new_label;
}

void Button::put(PositionStruct new_position) {
	position = new_position;
	
	if (Cursor::Pos().x >= position.x && Cursor::Pos().y >= position.y && Cursor::Pos().x <= position.x+size.width && Cursor::Pos().y <= position.y+size.height) {
		Rect(position.x, position.y, size.width, size.height).draw(Color(0, 162, 232, 128));
		
		if (label.length() > 0) {
			font(label).draw(position.x+size.width/2-font(label).region(Scene::Width()/2, Scene::Height()/2).w/2, position.y+size.height);
		}
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
