//
//  Weather.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/01/18.
//

#include "Weather.hpp"

void Weather::init(bool arg_change_weather) {
	change_weather = arg_change_weather;
	amount_of_rain = 0;
	
	switch(random() % 4) {
		case 0:
			weather_now = WeatherTypes::Sunny;
			break;
		case 1:
			weather_now = WeatherTypes::Cloudy;
			break;
		default:
			weather_now = WeatherTypes::Rainny;
			amount_of_rain = 100;
			break;
	}
	
	temperature = random() % 60 - 30.0;
	
	if (temperature < 0.0 && weather_now == WeatherTypes::Rainny) {
		weather_now = WeatherTypes::Snowy;
	}
}

void Weather::changeWeather() {
	double delta_temperature = random() % 20/10 - 1;
	temperature += delta_temperature;
	
	if (random() % 10 == 0) {
		switch(random() % 4) {
			case 0:
				weather_now = WeatherTypes::Sunny;
				amount_of_rain = 0;
				break;
			case 1:
				weather_now = WeatherTypes::Cloudy;
				amount_of_rain = 0;
				break;
			default:
				weather_now = WeatherTypes::Rainny;
				amount_of_rain = 1;
				break;
		}
	}
	
	if (amount_of_rain > 0) {
		amount_of_rain += random() % 10/10 - 1;
	}
	
	temperature = random() % 60 - 30.0;
	
	if (temperature < 0.0 && weather_now == WeatherTypes::Rainny) {
		weather_now = WeatherTypes::Snowy;
	}
}
