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
	
	button[U"cursor"].set(IconFont::Cursor, 30, 26, PositionStruct{8, 2});
	
	button[U"road"].set(IconFont::Road, 30, 26, PositionStruct{ 0, 6 });
	button[U"train"].set(IconFont::Railroad, 30, 28, PositionStruct{ 2, 2 });
	button[U"residential"].set(IconFont::Residential, 30, 30, PositionStruct{-3, 2});
	button[U"commercial"].set(IconFont::Commercial, 30, 26, PositionStruct{-3, 2});
	button[U"office"].set(IconFont::Office, 30, 30, PositionStruct{0, 1});
	button[U"industrial"].set(IconFont::Industrial, 30, 30, PositionStruct{0, 2});
	button[U"farm"].set(IconFont::Farm, 30, 26, PositionStruct{-3, 2});
	button[U"public"].set(IconFont::Public, 30, 30, PositionStruct{0, 2});
	
	button[U"park"].set(IconFont::Park, 30, 30, PositionStruct{2, 2});
	button[U"ship"].set(IconFont::WaterWay, 30, 26, PositionStruct{0, 2});
	button[U"air_port"].set(IconFont::AirPort, 30, 24, PositionStruct{-1, 4});
	button[U"tile"].set(IconFont::Field, 30, 30, PositionStruct{2, 2});
	
	button[U"information"].set(IconFont::Information, 30, 26, PositionStruct{2, 2});
	button[U"rate"].set(IconFont::Rate, 30, 30, PositionStruct{0, 2});
	button[U"delete"].set(IconFont::Delete, 30, 30, PositionStruct{0, 2});
	button[U"budget"].set(IconFont::Budget, 30, 26, PositionStruct{-1, 4});
	button[U"setting"].set(IconFont::Setting, 30, 28, PositionStruct{1, 1});
	button[U"save"].set(IconFont::Save, 30, 30, PositionStruct{2, 2});
	
	menu_mode = MenuMode::None;
	
	population = Texture(Icon(IconFont::Population, 20));
	
	// レンダテクスチャに描画
	ScopedRenderTarget2D target(render);
}

void Menu::releaseBeforeButton(MenuMode::Type before_selected_button) {
	switch (before_selected_button) {
		case MenuMode::Cursor:
			button[U"cursor"].release();
			return;
		case MenuMode::Road:
			button[U"road"].release();
			return;
		case MenuMode::Train:
			button[U"train"].release();
			return;
		case MenuMode::Residential:
			button[U"residential"].release();
			return;
		case MenuMode::Commercial:
			button[U"commercial"].release();
			return;
		case MenuMode::Office:
			button[U"office"].release();
			return;
		case MenuMode::Industrial:
			button[U"industrial"].release();
			return;
		case MenuMode::Farm:
			button[U"farm"].release();
			return;
		case MenuMode::Public:
			button[U"public"].release();
			return;
		case MenuMode::Park:
			button[U"park"].release();
			return;
		case MenuMode::Ship:
			button[U"ship"].release();
			return;
		case MenuMode::AirPort:
			button[U"air_port"].release();
			return;
		case MenuMode::Tile:
			button[U"tile"].release();
			return;
		case MenuMode::None:
			return;
	}
}

void Menu::draw(RCOIFstruct demand, int population_count, int money) {
	render.draw(position.x, position.y);
	
	button[U"cursor"].put(PositionStruct{position.x+10, position.y});
	
	button[U"road"].put(PositionStruct{ position.x + 55, position.y });
	button[U"train"].put(PositionStruct{ position.x + 55+35, position.y });
	button[U"residential"].put(PositionStruct{position.x+55+70, position.y});
	button[U"commercial"].put(PositionStruct{position.x+55+105, position.y});
	button[U"office"].put(PositionStruct{position.x+55+140, position.y});
	button[U"industrial"].put(PositionStruct{position.x+55+175, position.y});
	button[U"farm"].put(PositionStruct{position.x+55+210, position.y});
	button[U"public"].put(PositionStruct{position.x+55+245, position.y});
	
	button[U"park"].put(PositionStruct{position.x+345, position.y});
	button[U"ship"].put(PositionStruct{position.x+345+35, position.y});
	button[U"air_port"].put(PositionStruct{position.x+345+70, position.y});
	button[U"tile"].put(PositionStruct{position.x+345+105, position.y});
	
	button[U"information"].put(PositionStruct{position.x+495, position.y});
	button[U"rate"].put(PositionStruct{position.x+495+35, position.y});
	button[U"delete"].put(PositionStruct{position.x+495+70, position.y});
	button[U"budget"].put(PositionStruct{position.x+495+105, position.y});
	button[U"setting"].put(PositionStruct{position.x+495+140, position.y});
	button[U"save"].put(PositionStruct{position.x+495+175, position.y});
	
	(*font8)(U"R").draw(position.x+size.width-80+3, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"C").draw(position.x+size.width-80+3+8, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"O").draw(position.x+size.width-80+3+16, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"I").draw(position.x+size.width-80+3+24, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"F").draw(position.x+size.width-80+3+32, position.y+size.height-15+2, Color(Palette::White));
	
	Rect(position.x+size.width-80, position.y+size.height-15, 8, -max(demand.residential*0.4, 1.0)).draw(Color(34, 177, 76));
	Rect(position.x+size.width-80+8, position.y+size.height-15, 8, -max(demand.commercial*0.4, 1.0)).draw(Color(63, 72, 204));
	Rect(position.x+size.width-80+16, position.y+size.height-15, 8, -max(demand.office*0.4, 1.0)).draw(Color(0, 162, 232));
	Rect(position.x+size.width-80+24, position.y+size.height-15, 8, -max(demand.industrial*0.4, 1.0)).draw(Color(255, 242, 0));
	Rect(position.x+size.width-80+32, position.y+size.height-15, 8, -max(demand.farm*0.4, 1.0)).draw(Color(185, 122, 87));
	
	population.draw(position.x+10, position.y+size.height-25);
	(*font16)(Format(population_count)).draw(position.x+10+30, position.y+size.height-25-3, Color(Palette::White));
	
	(*font16)(U"§ "+Format(money)).draw(position.x+10+100, position.y+size.height-25-3, Color(Palette::White));
	
	// ボタンが押されたときの動作
	if (button[U"cursor"].push()) {
		if (menu_mode != MenuMode::Cursor)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Cursor;
	}
	if (button[U"road"].push()) {
		if (menu_mode != MenuMode::Road)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Road;
	}
	if (button[U"train"].push()) {
		if (menu_mode != MenuMode::Train)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Train;
	}
	if (button[U"residential"].push()) {
		if (menu_mode != MenuMode::Residential)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Residential;
	}
	if (button[U"commercial"].push()) {
		if (menu_mode != MenuMode::Commercial)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Commercial;
	}
	if (button[U"office"].push()) {
		if (menu_mode != MenuMode::Office)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Office;
	}
	if (button[U"industrial"].push()) {
		if (menu_mode != MenuMode::Industrial)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Industrial;
	}
	if (button[U"farm"].push()) {
		if (menu_mode != MenuMode::Farm)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Farm;
	}
	if (button[U"public"].push()) {
		if (menu_mode != MenuMode::Public)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Public;
	}
	if (button[U"park"].push()) {
		if (menu_mode != MenuMode::Park)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Park;
	}
	if (button[U"ship"].push()) {
		if (menu_mode != MenuMode::Ship)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Ship;
	}
	if (button[U"air_port"].push()) {
		if (menu_mode != MenuMode::AirPort)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::AirPort;
	}
	if (button[U"tile"].push()) {
		if (menu_mode != MenuMode::Tile)
			releaseBeforeButton(menu_mode);
		menu_mode = MenuMode::Tile;
	}
}
