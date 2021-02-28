//
//  DetailsBar.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2020/03/10.
//

#include "DetailsBar.hpp"

DetailsBar::DetailsBar(PositionStruct argPosition, Font* argFont) {
	position = argPosition;
	font = argFont;
	
	populationIcon = Texture(Icon(IconFont::Population, 20));
	timeIcon = Texture(Icon(IconFont::Clock, 20));
	
	temperatureLowIcon = Texture(Icon(IconFont::TemperatureLow, 20));
	temperatureMiddleIcon = Texture(Icon(IconFont::TemperatureMiddle, 20));
	temperatureHighIcon = Texture(Icon(IconFont::TemperatureHigh, 20));
	
	weatherSunnyIcon = Texture(Icon(IconFont::Sunny, 20));
	weatherCloudyIcon = Texture(Icon(IconFont::Cloudy, 20));
	weatherRainyIcon = Texture(Icon(IconFont::Rainy, 20));
	weatherSnowyIcon = Texture(Icon(IconFont::Snowy, 20));
}

void DetailsBar::printWeather(Weather::Type weather) {
	switch (weather) {
		case Weather::Sunny:
			weatherSunnyIcon.draw(position.x+1, position.y+2+1, ColorF(0.0, 0.5));
			weatherSunnyIcon.draw(position.x, position.y+2, Color(240));
			break;
	};
	
	weatherRegion = position.x+20;
}

void DetailsBar::printTemperature(int temperature) {
	if (temperature < 0) {
		temperatureLowIcon.draw(weatherRegion+10+1, position.y+2+1, ColorF(0.0, 0.5));
		temperatureLowIcon.draw(weatherRegion+10, position.y+2, Color(240));
	}
	else if (temperature > 25) {
		temperatureHighIcon.draw(weatherRegion+10+1, position.y+2+1, ColorF(0.0, 0.5));
		temperatureHighIcon.draw(weatherRegion+10, position.y+2, Color(240));
	}
	else {
		temperatureMiddleIcon.draw(weatherRegion+10+1, position.y+2+1, ColorF(0.0, 0.5));
		temperatureMiddleIcon.draw(weatherRegion+10, position.y+2, Color(240));
	}
	
	(*font)(Format(temperature)+U"℃").draw(weatherRegion+30+1, position.y+1, ColorF(0.0, 0.5));
	(*font)(Format(temperature)+U"℃").draw(weatherRegion+30, position.y, Color(240));
	
	temperatureRegion = weatherRegion+30+(*font)(Format(temperature)+U"℃").region().w;
}

void DetailsBar::printPopulation(int totalPopulation) {
	populationIcon.draw(temperatureRegion+10+1, position.y+2+1, ColorF(0.0, 0.5));
	populationIcon.draw(temperatureRegion+10, position.y+2, Color(240));
	
	(*font)(Format(totalPopulation)).draw(temperatureRegion+40+1, position.y+1, ColorF(0.0, 0.5));
	(*font)(Format(totalPopulation)).draw(temperatureRegion+40, position.y, Color(240));
	
	populationPrintRegion = temperatureRegion+40+(*font)(Format(totalPopulation)).region().w;
}

void DetailsBar::printMoney(int money) {
	(*font)(U"§ "+Format(money)).draw(populationPrintRegion+10+1, position.y+1, ColorF(0.0, 0.5));
	(*font)(U"§ "+Format(money)).draw(populationPrintRegion+10, position.y, Color(240));
	
	moneyPrintRegion = populationPrintRegion+10+(*font)(U"§ "+Format(money)).region().w;
}

void DetailsBar::printTime(TimeStruct time) {
	timeIcon.draw(moneyPrintRegion+10+1, position.y+2+1, ColorF(0.0, 0.5));
	timeIcon.draw(moneyPrintRegion+10, position.y+2, Color(240));
	
	(*font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).draw(moneyPrintRegion+35+1, position.y+1, ColorF(0.0, 0.5));
	(*font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).draw(moneyPrintRegion+35, position.y, Color(240));
	
	timePrintRegion = moneyPrintRegion+35+(*font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).region().w;
}
