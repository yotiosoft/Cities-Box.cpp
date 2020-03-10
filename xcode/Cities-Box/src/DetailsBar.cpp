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
}

void DetailsBar::printPopulation(int totalPopulation) {
	populationIcon.draw(position.x+1, position.y+2+1, ColorF(0.0, 0.5));
	populationIcon.draw(position.x, position.y+2, Color(240));
	
	(*font)(Format(totalPopulation)).draw(position.x+30+1, position.y+1, ColorF(0.0, 0.5));
	(*font)(Format(totalPopulation)).draw(position.x+30, position.y, Color(240));
	
	populationPrintRegion = 30+(*font)(Format(totalPopulation)).region().w;
}

void DetailsBar::printMoney(int money) {
	(*font)(U"§ "+Format(money)).draw(position.x+populationPrintRegion+20+1, position.y+1, ColorF(0.0, 0.5));
	(*font)(U"§ "+Format(money)).draw(position.x+populationPrintRegion+20, position.y, Color(240));
	
	moneyPrintRegion = (*font)(U"§ "+Format(money)).region().w+20;
}

void DetailsBar::printTime(TimeStruct time) {
	timeIcon.draw(position.x+populationPrintRegion+moneyPrintRegion+20+1, position.y+2+1, ColorF(0.0, 0.5));
	timeIcon.draw(position.x+populationPrintRegion+moneyPrintRegion+20, position.y+2, Color(240));
	
	(*font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).draw(position.x+populationPrintRegion+moneyPrintRegion+50+1, position.y+1, ColorF(0.0, 0.5));
	(*font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).draw(position.x+populationPrintRegion+moneyPrintRegion+50, position.y, Color(240));
	
	timePrintRegion = 30+(*font)(U"{:04d}/{:02d}/{:02d} {:02d}:{:02d}"_fmt(time.year, time.month, time.date, time.hour, time.minutes)).region().w+50;
}
