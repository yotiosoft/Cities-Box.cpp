//
//  Weather.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/01/18.
//

#ifndef Weather_hpp
#define Weather_hpp

#include "Header.hpp"

namespace WeatherTypes {
	enum Type {
		Sunny		= 0,
		Cloudy		= 1,
		Rainny		= 2,
		Snowy		= 3,
	};
}

class Weather {
public:
	void init(bool arg_change_weather);
	
	void changeWeather();
	
	void draw();
	
private:
	bool change_weather;
	WeatherTypes::Type weather_now;
	int amount_of_rain;
	random_device random;
	double temperature;
};

#endif /* Weather_hpp */
