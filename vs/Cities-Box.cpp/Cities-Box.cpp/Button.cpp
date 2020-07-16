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

Button::Button(ImageStruct& image, int newSize) {
	active = false;
	set(image, newSize);
}

Button::Button(ImageStruct& image, int newSize, String newLabel, Font& newFont) {
	active = false;
	set(image, newSize, newLabel, newFont);
}

Button::Button(IconFont::ID iconID) {
	active = false;
	set(iconID);
}

Button::Button(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift) {
	active = false;
	set(iconID, newSize, newIconSize, newShift);
}

Button::Button(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift, String newLabel, Font& newFont) {
	active = false;
	set(iconID, newSize, newIconSize, newShift, newLabel, newFont);
}

void Button::set(ImageStruct& image) {
	size = SizeStruct{image.texture.size().x, image.texture.size().y};
	buttonTexture = image.texture;
	isIcon = false;
}

void Button::set(ImageStruct& image, int newSize) {
	size = SizeStruct{newSize, newSize};
	buttonTexture = image.texture.resized(size.width, size.height).texture;
	isIcon = false;
}

void Button::set(ImageStruct& image, int newSize, String newLabel, Font& newFont) {
	size = SizeStruct{newSize, newSize};
	buttonTexture = image.texture.resized(size.width, size.height).texture;
	isIcon = false;
	font = newFont;
	label = newLabel;
}

void Button::set(IconFont::ID iconID) {
	size = SizeStruct{64, 64};
	buttonTexture = Texture(Icon(iconID, size.width));
	isIcon = true;
}

void Button::set(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift) {
	size = SizeStruct{newSize, newSize};
	iconSize = SizeStruct{newIconSize, newIconSize};
	buttonTexture = Texture(Icon(iconID, iconSize.width));
	shift = newShift;
	isIcon = true;
}

void Button::set(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift, String newLabel, Font& newFont) {
	size = SizeStruct{newSize, newSize};
	iconSize = SizeStruct{newIconSize, newIconSize};
	buttonTexture = Texture(Icon(iconID, iconSize.width));
	isIcon = true;
	font = newFont;
	label = newLabel;
	shift = newShift;
}

void Button::put(PositionStruct newPosition) {
	position = newPosition;
	
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
	buttonTexture.draw(position.x, position.y);
}

void Button::putRelative(PositionStruct newPosition, PositionStruct leftTop) {
	position = newPosition;
	
	if (Cursor::Pos().x-leftTop.x >= position.x && Cursor::Pos().y-leftTop.y >= position.y &&
		Cursor::Pos().x-leftTop.x <= position.x+size.width && Cursor::Pos().y-leftTop.y <= position.y+size.height) {
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
	buttonTexture.draw(position.x, position.y);
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

bool Button::pushRelative(PositionStruct leftTop) {
	if (Cursor::Pos().x-leftTop.x >= position.x && Cursor::Pos().y-leftTop.y >= position.y &&
		Cursor::Pos().x-leftTop.x <= position.x+size.width && Cursor::Pos().y-leftTop.y <= position.y+size.height) {
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
