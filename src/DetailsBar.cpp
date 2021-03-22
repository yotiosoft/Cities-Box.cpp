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
	
	m_population_icon_texture = Texture(Icon(IconFont::Population, 20));
	time_icon_texture = Texture(Icon(IconFont::Clock, 20));
	
	m_temperature_low_icon_texture = Texture(Icon(IconFont::TemperatureLow, 20));
	m_temperature_middle_icon_texture = Texture(Icon(IconFont::TemperatureMiddle, 20));
	m_temperature_high_icon_texture = Texture(Icon(IconFont::TemperatureHigh, 20));
	
	m_weather_sunny_icon = Texture(Icon(IconFont::Sunny, 20));
	m_weather_cloudy_icon = Texture(Icon(IconFont::Cloudy, 20));
	m_weather_rainy_icon = Texture(Icon(IconFont::Rainy, 20));
	m_weather_snowy_icon = Texture(Icon(IconFont::Snowy, 20));
}

void DetailsBar::printWeather(Weather::Type weather) {
	switch (weather) {
		case Weather::Sunny:
			m_weather_sunny_icon.draw(m_position.x+1, m_position.y+2+1, ColorF(0.0, 0.5));
			m_weather_sunny_icon.draw(m_position.x, m_position.y+2, Color(240));
			break;
	};
	
	m_weather_region = m_position.x+20;
}

void DetailsBar::printTemperature(int temperature) {
	if (temperature < 0) {
		m_temperature_low_icon_texture.draw(m_weather_region+10+1, m_position.y+2+1, ColorF(0.0, 0.5));
		m_temperature_low_icon_texture.draw(m_weather_region+10, m_position.y+2, Color(240));
	}
	else if (temperature > 25) {
		m_temperature_high_icon_texture.draw(m_weather_region+10+1, m_position.y+2+1, ColorF(0.0, 0.5));
		m_temperature_high_icon_texture.draw(m_weather_region+10, m_position.y+2, Color(240));
	}
	else {
		m_temperature_middle_icon_texture.draw(m_weather_region+10+1, m_position.y+2+1, ColorF(0.0, 0.5));
		m_temperature_middle_icon_texture.draw(m_weather_region+10, m_position.y+2, Color(240));
	}
	
	(*m_font)(Format(temperature)+U"℃").draw(m_weather_region+30+1, m_position.y+1, ColorF(0.0, 0.5));
	(*m_font)(Format(temperature)+U"℃").draw(m_weather_region+30, m_position.y, Color(240));
	
	m_temperature_region = m_weather_region+30+(*m_font)(Format(temperature)+U"℃").region().w;
}

void DetailsBar::printPopulation(int totalPopulation) {
	m_population_icon_texture.draw(m_temperature_region+10+1, m_position.y+2+1, ColorF(0.0, 0.5));
	m_population_icon_texture.draw(m_temperature_region+10, m_position.y+2, Color(240));
	
	(*m_font)(Format(totalPopulation)).draw(m_temperature_region+40+1, m_position.y+1, ColorF(0.0, 0.5));
	(*m_font)(Format(totalPopulation)).draw(m_temperature_region+40, m_position.y, Color(240));
	
	m_population_print_region = m_temperature_region+40+(*m_font)(Format(totalPopulation)).region().w;
}

void DetailsBar::printMoney(int money) {
	(*m_font)(U"§ "+Format(money)).draw(m_population_print_region+10+1, m_position.y+1, ColorF(0.0, 0.5));
	(*m_font)(U"§ "+Format(money)).draw(m_population_print_region+10, m_position.y, Color(240));
	
	m_money_print_region = m_population_print_region+10+(*m_font)(U"§ "+Format(money)).region().w;
}

void DetailsBar::printTime(TimeStruct time) {
	time_icon_texture.draw(m_money_print_region+10+1, m_position.y+2+1, ColorF(0.0, 0.5));
	time_icon_texture.draw(m_money_print_region+10, m_position.y+2, Color(240));
	
	(*m_font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).draw(m_money_print_region+35+1, m_position.y+1, ColorF(0.0, 0.5));
	(*m_font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).draw(m_money_print_region+35, m_position.y, Color(240));
	
	m_time_print_region = m_money_print_region+35+(*m_font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).region().w;
}
