//
//  Button.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/12/01.
//

#include "Button.hpp"

Button::Button() {
	m_active = false;
}

Button::Button(ImageStruct& image) {
	m_active = false;
	set(image);
}

Button::Button(ImageStruct& image, int newSize) {
	m_active = false;
	set(image, newSize);
}

Button::Button(ImageStruct& image, int newSize, String newLabel, Font& newFont) {
	m_active = false;
	set(image, newSize, newLabel, newFont);
}

Button::Button(IconFont::ID iconID) {
	m_active = false;
	set(iconID);
}

Button::Button(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift) {
	m_active = false;
	set(iconID, newSize, newIconSize, newShift);
}

Button::Button(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift, String newLabel, Font& newFont) {
	m_active = false;
	set(iconID, newSize, newIconSize, newShift, newLabel, newFont);
}

void Button::set(ImageStruct& image) {
	m_size = Size(image.texture.size().x, image.texture.size().y);
	m_button_texture = image.texture;
	m_button_mode = ButtonMode::ImageMode;
}

void Button::set(ImageStruct& image, int newSize) {
	m_size = Size(newSize, newSize);
	m_button_texture = image.texture.resized(m_size.x, m_size.y).texture;
	m_button_mode = ButtonMode::ImageMode;
}

void Button::set(ImageStruct& image, int newSize, String newLabel, Font& newFont) {
	m_size = Size(newSize, newSize);
	m_button_texture = image.texture.resized(m_size.x, m_size.y).texture;
	m_button_mode = ButtonMode::ImageMode;
	m_font = newFont;
	m_label = newLabel;
}

void Button::set(IconFont::ID iconID) {
	m_size = Size(64, 64);
	const int iconSize = UIConfig::scaleIcon(m_size.x);
	m_button_texture = Texture{Icon(iconID), iconSize};
	m_shift = PositionStruct{ -(iconSize - m_size.x) / 2, -(iconSize - m_size.y) / 2 };
	m_button_mode = ButtonMode::IconMode;
}

void Button::set(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift) {
	m_size = Size(newSize, newSize);
	const int iconSize = UIConfig::scaleIcon(newIconSize);
	m_icon_size = Size(iconSize, iconSize);
	m_button_texture = Texture{Icon(iconID), iconSize};
	m_shift = PositionStruct{ newShift.x - (iconSize - newIconSize) / 2, newShift.y - (iconSize - newIconSize) / 2 };
	m_button_mode = ButtonMode::IconMode;
}

void Button::set(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift, String newLabel, Font& newFont) {
	m_size = Size(newSize, newSize);
	const int iconSize = UIConfig::scaleIcon(newIconSize);
	m_icon_size = Size(iconSize, iconSize);
	m_button_texture = Texture{Icon(iconID), iconSize};
	m_button_mode = ButtonMode::IconMode;
	m_font = newFont;
	m_label = newLabel;
	m_shift = PositionStruct{ newShift.x - (iconSize - newIconSize) / 2, newShift.y - (iconSize - newIconSize) / 2 };
}

void Button::put(PositionStruct newPosition) {
	m_position = newPosition;
	const PositionStruct drawPosition = (m_button_mode == ButtonMode::IconMode)
		? PositionStruct{m_position.x + m_shift.x, m_position.y + m_shift.y}
		: m_position;
	const Size hitboxSize = (m_button_mode == ButtonMode::IconMode)
		? m_button_texture.size()
		: m_size;
	m_hitbox = Rect(drawPosition.x, drawPosition.y, hitboxSize.x, hitboxSize.y);

	if (m_hitbox.mouseOver()) {
		m_hitbox.draw(Color(9, 132, 227, 128));
		
		if (m_label.length() > 0) {
			m_font(m_label).draw(m_hitbox.center().x-m_font(m_label).region().w/2, m_hitbox.y+m_hitbox.h);
		}
	}
	if (m_active) {
		m_hitbox.draw(Color(9, 132, 227));
	}

	m_button_texture.draw(drawPosition.x, drawPosition.y);
}

void Button::putRelative(PositionStruct newPosition, PositionStruct leftTop) {
	m_position = newPosition;
	const PositionStruct drawPosition = (m_button_mode == ButtonMode::IconMode)
		? PositionStruct{m_position.x + m_shift.x, m_position.y + m_shift.y}
		: m_position;
	const Size hitboxSize = (m_button_mode == ButtonMode::IconMode)
		? m_button_texture.size()
		: m_size;
	m_hitbox = Rect(drawPosition.x, drawPosition.y, hitboxSize.x, hitboxSize.y);
	const Vec2 relativeCursor = Cursor::Pos() - Vec2{leftTop.x, leftTop.y};

	if (m_hitbox.contains(relativeCursor)) {
		m_hitbox.draw(Color(9, 132, 227, 128));
		
		if (m_label.length() > 0) {
			m_font(m_label).draw(m_hitbox.center().x-m_font(m_label).region().w/2, m_hitbox.y+m_hitbox.h);
		}
	}
	if (m_active) {
		m_hitbox.draw(Color(9, 132, 227));
	}

	m_button_texture.draw(drawPosition.x, drawPosition.y);
}

bool Button::push() {
	if (m_hitbox.mouseOver()) {
		if (MouseL.down()) {
			m_active = !m_active;
			return true;
		}
	}
	
	return false;
}

bool Button::pushRelative(PositionStruct leftTop) {
	const Vec2 relativeCursor = Cursor::Pos() - Vec2{leftTop.x, leftTop.y};
	if (m_hitbox.contains(relativeCursor)) {
		if (MouseL.down()) {
			m_active = !m_active;
			return true;
		}
	}
	
	return false;
}

bool Button::isActive() {
	return m_active;
}

void Button::release() {
	m_active = false;
}
