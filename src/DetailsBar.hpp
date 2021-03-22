//
//  DetailsBar.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/03/10.
//

#ifndef DetailsBar_hpp
#define DetailsBar_hpp

#include <Specific.hpp>
#include "CityMap.hpp"

class DetailsBar {
public:
	DetailsBar(PositionStruct argPosition, Font* argFont);
	
	void printWeather(Weather::Type weather);
	void printTemperature(int temperature);
	void printPopulation(int totalPopulation);
	void printMoney(int money);
	void printTime(TimeStruct time);
	
private:
	Font* m_font;
	PositionStruct m_position;
	
	Texture m_temperature_low_icon_texture;
	Texture m_temperature_middle_icon_texture;
	Texture m_temperature_high_icon_texture;
	
	Texture m_population_icon_texture;
	Texture time_icon_texture;
	
	Texture m_weather_sunny_icon;
	Texture m_weather_cloudy_icon;
	Texture m_weather_rainy_icon;
	Texture m_weather_snowy_icon;
	
	int m_weather_region;
	int m_temperature_region;
	int m_population_print_region;
	int m_money_print_region;
	int m_time_print_region;
};

#endif /* DetailsBar_hpp */
