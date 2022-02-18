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
	m_button_texture = Texture{Icon(iconID), m_size.x};
	m_button_mode = ButtonMode::IconMode;
}

void Button::set(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift) {
	m_size = Size(newSize, newSize);
	m_icon_size = Size(newIconSize, newIconSize);
	m_button_texture = Texture{Icon(iconID), m_icon_size.x};
	m_shift = newShift;
	m_button_mode = ButtonMode::IconMode;
}

void Button::set(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift, String newLabel, Font& newFont) {
	m_size = Size(newSize, newSize);
	m_icon_size = Size(newIconSize, newIconSize);
	m_button_texture = Texture{Icon(iconID), m_icon_size.x};
	m_button_mode = ButtonMode::IconMode;
	m_font = newFont;
	m_label = newLabel;
	m_shift = newShift;
}

void Button::put(PositionStruct newPosition) {
	m_position = newPosition;
	
	if (Cursor::Pos().x >= m_position.x && Cursor::Pos().y >= m_position.y && Cursor::Pos().x <= m_position.x+m_size.x && Cursor::Pos().y <= m_position.y+m_size.y) {
		Rect(m_position.x, m_position.y, m_size.x, m_size.y).draw(Color(9, 132, 227, 128));
		
		if (m_label.length() > 0) {
			m_font(m_label).draw(m_position.x+m_size.x/2-m_font(m_label).region(Scene::Width()/2, Scene::Height()/2).w/2, m_position.y+m_size.y);
		}
	}
	if (m_active) {
		Rect(m_position.x, m_position.y, m_size.x, m_size.y).draw(Color(9, 132, 227));
	}
	
	if (m_button_mode == ButtonMode::IconMode) {
		m_position.x += m_shift.x;
		m_position.y += m_shift.y;
	}
	m_button_texture.draw(m_position.x, m_position.y);
}

void Button::putRelative(PositionStruct newPosition, PositionStruct leftTop) {
	m_position = newPosition;
	
	if (Cursor::Pos().x-leftTop.x >= m_position.x && Cursor::Pos().y-leftTop.y >= m_position.y &&
		Cursor::Pos().x-leftTop.x <= m_position.x+m_size.x && Cursor::Pos().y-leftTop.y <= m_position.y+m_size.y) {
		Rect(m_position.x, m_position.y, m_size.x, m_size.y).draw(Color(9, 132, 227, 128));
		
		if (m_label.length() > 0) {
			m_font(m_label).draw(m_position.x+m_size.x/2-m_font(m_label).region(Scene::Width()/2, Scene::Height()/2).w/2, m_position.y+m_size.y);
		}
	}
	if (m_active) {
		Rect(m_position.x, m_position.y, m_size.x, m_size.y).draw(Color(9, 132, 227));
	}
	
	if (m_button_mode == ButtonMode::IconMode) {
		m_position.x += m_shift.x;
		m_position.y += m_shift.y;
	}
	m_button_texture.draw(m_position.x, m_position.y);
}

bool Button::push() {
	if (Cursor::Pos().x >= m_position.x && Cursor::Pos().y >= m_position.y && Cursor::Pos().x <= m_position.x+m_size.x && Cursor::Pos().y <= m_position.y+m_size.y) {
		if (MouseL.down()) {
			m_active = !m_active;
			return true;
		}
	}
	
	return false;
}

bool Button::pushRelative(PositionStruct leftTop) {
	if (Cursor::Pos().x-leftTop.x >= m_position.x && Cursor::Pos().y-leftTop.y >= m_position.y &&
		Cursor::Pos().x-leftTop.x <= m_position.x+m_size.x && Cursor::Pos().y-leftTop.y <= m_position.y+m_size.y) {
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
