//
//  Button.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/12/01.
//

#include "Button.hpp"

Button::Button() {
	active = false;
}

Button::Button(ImageStruct& image) {
	active = false;
	set(image);
}

Button::Button(ImageStruct& image, int new_size) {
	active = false;
	set(image, new_size);
}

Button::Button(ImageStruct& image, int new_size, String new_label, Font& new_font) {
	active = false;
	set(image, new_size, new_label, new_font);
}

Button::Button(IconFont::ID icon_id) {
	active = false;
	set(icon_id);
}

Button::Button(IconFont::ID icon_id, int new_size, int new_icon_size, PositionStruct new_shift) {
	active = false;
	set(icon_id, new_size, new_icon_size, new_shift);
}

Button::Button(IconFont::ID icon_id, int new_size, int new_icon_size, PositionStruct new_shift, String new_label, Font& new_font) {
	active = false;
	set(icon_id, new_size, new_icon_size, new_shift, new_label, new_font);
}

void Button::set(ImageStruct& image) {
	size = SizeStruct{image.texture.size().x, image.texture.size().y};
	button_texture = image.texture;
	isIcon = false;
}

void Button::set(ImageStruct& image, int new_size) {
	size = SizeStruct{new_size, new_size};
	button_texture = image.texture.resized(size.width, size.height).texture;
	isIcon = false;
}

void Button::set(ImageStruct& image, int new_size, String new_label, Font& new_font) {
	size = SizeStruct{new_size, new_size};
	button_texture = image.texture.resized(size.width, size.height).texture;
	isIcon = false;
	font = new_font;
	label = new_label;
}

void Button::set(IconFont::ID icon_id) {
	size = SizeStruct{64, 64};
	button_texture = Texture(Icon(icon_id, size.width));
	isIcon = true;
}

void Button::set(IconFont::ID icon_id, int new_size, int new_icon_size, PositionStruct new_shift) {
	size = SizeStruct{new_size, new_size};
	icon_size = SizeStruct{new_icon_size, new_icon_size};
	button_texture = Texture(Icon(icon_id, icon_size.width));
	shift = new_shift;
	isIcon = true;
}

void Button::set(IconFont::ID icon_id, int new_size, int new_icon_size, PositionStruct new_shift, String new_label, Font& new_font) {
	size = SizeStruct{new_size, new_size};
	icon_size = SizeStruct{new_icon_size, new_icon_size};
	button_texture = Texture(Icon(icon_id, icon_size.width));
	isIcon = true;
	font = new_font;
	label = new_label;
	shift = new_shift;
}

void Button::put(PositionStruct new_position) {
	position = new_position;
	
	if (Cursor::Pos().x >= position.x && Cursor::Pos().y >= position.y && Cursor::Pos().x <= position.x+size.width && Cursor::Pos().y <= position.y+size.height) {
		Rect(position.x, position.y, size.width, size.height).draw(Color(9, 132, 227, 128));
		
		if (label.length() > 0) {
			font(label).draw(position.x+size.width/2-font(label).region(Scene::Width()/2, Scene::Height()/2).w/2, position.y+size.height);
		}
	}
	if (active) {
		Rect(position.x, position.y, size.width, size.height).draw(Color(9, 132, 227));
	}
	
	if (isIcon) {
		position.x += shift.x;
		position.y += shift.y;
	}
	button_texture.draw(position.x, position.y);
}

void Button::putRelative(PositionStruct new_position, PositionStruct left_top) {
	position = new_position;
	
	if (Cursor::Pos().x-left_top.x >= position.x && Cursor::Pos().y-left_top.y >= position.y &&
		Cursor::Pos().x-left_top.x <= position.x+size.width && Cursor::Pos().y-left_top.y <= position.y+size.height) {
		Rect(position.x, position.y, size.width, size.height).draw(Color(9, 132, 227, 128));
		
		if (label.length() > 0) {
			font(label).draw(position.x+size.width/2-font(label).region(Scene::Width()/2, Scene::Height()/2).w/2, position.y+size.height);
		}
	}
	if (active) {
		Rect(position.x, position.y, size.width, size.height).draw(Color(9, 132, 227));
	}
	
	if (isIcon) {
		position.x += shift.x;
		position.y += shift.y;
	}
	button_texture.draw(position.x, position.y);
}

bool Button::push() {
	if (Cursor::Pos().x >= position.x && Cursor::Pos().y >= position.y && Cursor::Pos().x <= position.x+size.width && Cursor::Pos().y <= position.y+size.height) {
		if (MouseL.down()) {
			active = !active;
			return true;
		}
	}
	
	return false;
}

bool Button::pushRelative(PositionStruct left_top) {
	if (Cursor::Pos().x-left_top.x >= position.x && Cursor::Pos().y-left_top.y >= position.y &&
		Cursor::Pos().x-left_top.x <= position.x+size.width && Cursor::Pos().y-left_top.y <= position.y+size.height) {
		if (MouseL.down()) {
			active = !active;
			return true;
		}
	}
	
	return false;
}

bool Button::isActive() {
	return active;
}

void Button::release() {
	active = false;
}
