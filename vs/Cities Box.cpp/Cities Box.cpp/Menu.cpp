//
//  Menu.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/07.
//

#include "Menu.hpp"

void Menu::set(PositionStruct new_position, SizeStruct new_size, Font* new_font8, Font* new_font16) {
	position = new_position;
	size = new_size;
	
	font8 = new_font8;
	font16 = new_font16;
	
	render = RenderTexture(size.width, size.height, Color(50, 50, 50));
	
	cursorButton.set(IconFont::Cursor, 30, 26, PositionStruct{8, 2});
	
	residentialButton.set(IconFont::Residential, 30, 30, PositionStruct{-3, 2});
	commercialButton.set(IconFont::Commercial, 30, 26, PositionStruct{-3, 2});
	officeButton.set(IconFont::Office, 30, 30, PositionStruct{-3, 2});
	industrialButton.set(IconFont::Industrial, 30, 30, PositionStruct{0, 2});
	farmButton.set(IconFont::Farm, 30, 26, PositionStruct{-3, 2});
	publicButton.set(IconFont::Public, 30, 30, PositionStruct{0, 2});
	
	parkButton.set(IconFont::Park, 30, 30, PositionStruct{2, 2});
	waterWayButton.set(IconFont::WaterWay, 30, 26, PositionStruct{0, 2});
	airPortButton.set(IconFont::AirPort, 30, 24, PositionStruct{-1, 4});
	fieldButton.set(IconFont::Field, 30, 30, PositionStruct{2, 2});
	
	informationButton.set(IconFont::Information, 30, 26, PositionStruct{2, 2});
	rateButton.set(IconFont::Rate, 30, 30, PositionStruct{0, 2});
	deleteButton.set(IconFont::Delete, 30, 30, PositionStruct{0, 2});
	budgetButton.set(IconFont::Budget, 30, 26, PositionStruct{-1, 4});
	settingButton.set(IconFont::Setting, 30, 28, PositionStruct{1, 1});
	saveButton.set(IconFont::Save, 30, 30, PositionStruct{2, 2});
	
	population = Texture(Icon(IconFont::Population, 20));
	
	// レンダテクスチャに描画
	ScopedRenderTarget2D target(render);
}

void Menu::draw(RCOIFstruct demand, int population_count, int money) {
	render.draw(position.x, position.y);
	
	cursorButton.put(PositionStruct{position.x+10, position.y});
	
	residentialButton.put(PositionStruct{position.x+55, position.y});
	commercialButton.put(PositionStruct{position.x+55+35, position.y});
	industrialButton.put(PositionStruct{position.x+55+70, position.y});
	farmButton.put(PositionStruct{position.x+55+105, position.y});
	publicButton.put(PositionStruct{position.x+55+140, position.y});
	
	parkButton.put(PositionStruct{position.x+240, position.y});
	waterWayButton.put(PositionStruct{position.x+240+35, position.y});
	airPortButton.put(PositionStruct{position.x+240+70, position.y});
	fieldButton.put(PositionStruct{position.x+240+105, position.y});
	
	informationButton.put(PositionStruct{position.x+390, position.y});
	rateButton.put(PositionStruct{position.x+390+35, position.y});
	deleteButton.put(PositionStruct{position.x+390+70, position.y});
	budgetButton.put(PositionStruct{position.x+390+105, position.y});
	settingButton.put(PositionStruct{position.x+390+140, position.y});
	saveButton.put(PositionStruct{position.x+390+175, position.y});
	
	(*font8)(U"R").draw(position.x+size.width-120+3, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"C").draw(position.x+size.width-120+3+8, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"O").draw(position.x+size.width-120+3+16, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"I").draw(position.x+size.width-120+3+24, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"F").draw(position.x+size.width-120+3+32, position.y+size.height-15+2, Color(Palette::White));
	
	Rect(position.x+size.width-120, position.y+size.height-15, 8, -max(demand.residential*0.4, 1.0)).draw(Color(34, 177, 76));
	Rect(position.x+size.width-120+8, position.y+size.height-15, 8, -max(demand.commercial*0.4, 1.0)).draw(Color(63, 72, 204));
	Rect(position.x+size.width-120+16, position.y+size.height-15, 8, -max(demand.office*0.4, 1.0)).draw(Color(0, 162, 232));
	Rect(position.x+size.width-120+24, position.y+size.height-15, 8, -max(demand.industrial*0.4, 1.0)).draw(Color(255, 242, 0));
	Rect(position.x+size.width-120+32, position.y+size.height-15, 8, -max(demand.farm*0.4, 1.0)).draw(Color(185, 122, 87));
	
	population.draw(position.x+10, position.y+size.height-25);
	(*font16)(Format(population_count)).draw(position.x+10+30, position.y+size.height-25-3, Color(Palette::White));
	
	(*font16)(U"§ "+Format(money)).draw(position.x+10+100, position.y+size.height-25-3, Color(Palette::White));
}
