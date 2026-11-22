//
//  DetailsBar.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/03/10.
//

#include "DetailsBar.hpp"

DetailsBar::DetailsBar(PositionStruct argPosition, Font* argFont) {
	m_position = argPosition;
	m_font = argFont;
	
	const int iconSize = UIConfig::DetailsIconSize;
	m_population_icon_texture = Texture{Icon(IconFont::Population), iconSize};
	time_icon_texture = Texture{Icon(IconFont::Clock), iconSize};
	
	m_temperature_low_icon_texture = Texture{Icon(IconFont::TemperatureLow), iconSize};
	m_temperature_middle_icon_texture = Texture{Icon(IconFont::TemperatureMiddle), iconSize};
	m_temperature_high_icon_texture = Texture{Icon(IconFont::TemperatureHigh), iconSize};
	
	m_weather_sunny_icon = Texture{Icon(IconFont::Sunny), iconSize};
}

void DetailsBar::setPos(PositionStruct argPosition) {
	m_position = argPosition;
}

void DetailsBar::printWeather() {
	m_weather_sunny_icon.draw(m_position.x+1, m_position.y+2+1, ColorF(0.0, 0.5));
	m_weather_sunny_icon.draw(m_position.x, m_position.y+2, Color(240));
	
	m_weather_region = m_position.x + UIConfig::DetailsIconSize;
}

void DetailsBar::printTemperature(int temperature) {
	const int iconX = m_weather_region + UIConfig::DetailsItemGap;
	const int textX = iconX + UIConfig::DetailsIconSize + UIConfig::DetailsTemperatureTextGap;

	if (temperature < 0) {
		m_temperature_low_icon_texture.draw(iconX+1, m_position.y+2+1, ColorF(0.0, 0.5));
		m_temperature_low_icon_texture.draw(iconX, m_position.y+2, Color(240));
	}
	else if (temperature > 25) {
		m_temperature_high_icon_texture.draw(iconX+1, m_position.y+2+1, ColorF(0.0, 0.5));
		m_temperature_high_icon_texture.draw(iconX, m_position.y+2, Color(240));
	}
	else {
		m_temperature_middle_icon_texture.draw(iconX+1, m_position.y+2+1, ColorF(0.0, 0.5));
		m_temperature_middle_icon_texture.draw(iconX, m_position.y+2, Color(240));
	}
	
	(*m_font)(Format(temperature)+U"℃").draw(textX+1, m_position.y+1, ColorF(0.0, 0.5));
	(*m_font)(Format(temperature)+U"℃").draw(textX, m_position.y, Color(240));
	
	m_temperature_region = textX + (*m_font)(Format(temperature)+U"℃").region().w;
}

void DetailsBar::printPopulation(int totalPopulation) {
	const int iconX = m_temperature_region + UIConfig::DetailsItemGap;
	const int textX = iconX + UIConfig::DetailsIconSize + UIConfig::DetailsPopulationTextGap;
	m_population_icon_texture.draw(iconX+1, m_position.y+2+1, ColorF(0.0, 0.5));
	m_population_icon_texture.draw(iconX, m_position.y+2, Color(240));
	
	(*m_font)(Format(totalPopulation)).draw(textX+1, m_position.y+1, ColorF(0.0, 0.5));
	(*m_font)(Format(totalPopulation)).draw(textX, m_position.y, Color(240));
	
	m_population_print_region = textX + (*m_font)(Format(totalPopulation)).region().w;
}

void DetailsBar::printMoney(int money) {
	const int textX = m_population_print_region + UIConfig::DetailsItemGap;
	(*m_font)(U"§ "+Format(money)).draw(textX+1, m_position.y+1, ColorF(0.0, 0.5));
	(*m_font)(U"§ "+Format(money)).draw(textX, m_position.y, Color(240));
	
	m_money_print_region = textX + (*m_font)(U"§ "+Format(money)).region().w;
}

void DetailsBar::printTime(TimeStruct time) {
	const int iconX = m_money_print_region + UIConfig::DetailsItemGap;
	const int textX = iconX + UIConfig::DetailsIconSize + UIConfig::DetailsTimeTextGap;
	time_icon_texture.draw(iconX+1, m_position.y+2+1, ColorF(0.0, 0.5));
	time_icon_texture.draw(iconX, m_position.y+2, Color(240));
	
	(*m_font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).draw(textX+1, m_position.y+1, ColorF(0.0, 0.5));
	(*m_font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).draw(textX, m_position.y, Color(240));
}
