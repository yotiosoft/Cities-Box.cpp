//
//  DetailsBar.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/03/10.
//

#ifndef DetailsBar_hpp
#define DetailsBar_hpp

#include "Specific.hpp"
#include "CityMap.hpp"

class DetailsBar {
public:
	DetailsBar(PositionStruct argPosition, Font* argFont);
	
	void printWeather(WeatherStruct::Type weather);
	void printTemperature(int temperature);
	void printPopulation(int totalPopulation);
	void printMoney(int money);
	void printTime(TimeStruct time);
	
private:
	Font* font;
	PositionStruct position;
	
	Texture temperatureLowIcon;
	Texture temperatureMiddleIcon;
	Texture temperatureHighIcon;
	
	Texture populationIcon;
	Texture timeIcon;
	
	Texture weatherSunnyIcon;
	Texture weatherCloudyIcon;
	Texture weatherRainyIcon;
	Texture weatherSnowyIcon;
	
	int weatherRegion;
	int temperatureRegion;
	int populationPrintRegion;
	int moneyPrintRegion;
	int timePrintRegion;
};

#endif /* DetailsBar_hpp */
