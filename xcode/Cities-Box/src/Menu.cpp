//
//  Menu.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/07.
//

#include "Menu.hpp"

void Menu::set(PositionStruct new_position, SizeStruct new_size, CityMap* new_map, Font* new_font8, Font* new_font12, Font* new_font16) {
	position = new_position;
	size = new_size;
	
	font8 = new_font8;
	font12 = new_font12;
	font16 = new_font16;
	
	map = new_map;
	
	render = RenderTexture(size.width, size.height, Color(45, 52, 54));
	
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
	
	selected_addon_name = U"";
	
	population = Texture(Icon(IconFont::Population, 20));
	
	// 効果テクスチャを用意
	effect_icons[U"crime_rate"] = Texture(Icon(IconFont::Crime, 16));
	effect_icons[U"durability"] = Texture(Icon(IconFont::Durability, 16));
	effect_icons[U"education_rate"] = Texture(Icon(IconFont::Education, 16));
	effect_icons[U"firing_rate"] = Texture(Icon(IconFont::Firing, 16));
	effect_icons[U"garbage_disposal"] = Texture(Icon(IconFont::Garbage, 16));
	effect_icons[U"land_price"] = Texture(Icon(IconFont::LandPrice, 16));
	effect_icons[U"mobile_communication"] = Texture(Icon(IconFont::MobileCommunication, 16));
	effect_icons[U"noise"] = Texture(Icon(IconFont::Noise, 16));
	effect_icons[U"post"] = Texture(Icon(IconFont::Post, 16));
	effect_icons[U"television"] = Texture(Icon(IconFont::Television, 16));
	effect_icons[U"tourist_attraction"] = Texture(Icon(IconFont::Tourist, 16));
	effect_icons[U"radio"] = Texture(Icon(IconFont::Radio, 16));
	
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

Addon* Menu::draw(RCOIFstruct demand, int population_count, int money) {
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
	
	// RCOIFメータを表示
	(*font8)(U"R").draw(position.x+size.width-80+3, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"C").draw(position.x+size.width-80+3+8, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"O").draw(position.x+size.width-80+3+16, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"I").draw(position.x+size.width-80+3+24, position.y+size.height-15+2, Color(Palette::White));
	(*font8)(U"F").draw(position.x+size.width-80+3+32, position.y+size.height-15+2, Color(Palette::White));
	
	Rect(position.x+size.width-80, position.y+size.height-15, 8, -max(demand.residential*0.4, 1.0)).draw(Color(39, 174, 96));
	Rect(position.x+size.width-80+8, position.y+size.height-15, 8, -max(demand.commercial*0.4, 1.0)).draw(Color(9, 132, 227));
	Rect(position.x+size.width-80+16, position.y+size.height-15, 8, -max(demand.office*0.4, 1.0)).draw(Color(0, 206, 201));
	Rect(position.x+size.width-80+24, position.y+size.height-15, 8, -max(demand.industrial*0.4, 1.0)).draw(Color(253, 203, 110));
	Rect(position.x+size.width-80+32, position.y+size.height-15, 8, -max(demand.farm*0.4, 1.0)).draw(Color(211, 84, 0));
	
	population.draw(position.x+10, position.y+size.height-25);
	(*font16)(Format(population_count)).draw(position.x+10+30, position.y+size.height-25-3, Color(Palette::White));
	
	(*font16)(U"§ "+Format(money)).draw(position.x+10+100, position.y+size.height-25-3, Color(Palette::White));
	
	// ボタンが押されたときの動作
	if (button[U"cursor"].push()) {
		if (menu_mode != MenuMode::Cursor)
			releaseBeforeButton(menu_mode);
		
		menu_mode = MenuMode::Cursor;
		selected_addon = nullptr;
		button[U"cursor"].release();
	}
	if (button[U"road"].push()) {
		if (menu_mode != MenuMode::Road) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Road;
			selected_addon = nullptr;
			mode_str = U"road";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
			category_buttons << pair<String, Button>(U"promenade", Button(IconFont::WALK, 16, 16, PositionStruct{2, 0}));
			category_buttons << pair<String, Button>(U"car", Button(IconFont::CAR, 16, 16, PositionStruct{0, 0}));
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"train"].push()) {
		if (menu_mode != MenuMode::Train) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Train;
			selected_addon = nullptr;
			mode_str = U"train";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"residential"].push()) {
		if (menu_mode != MenuMode::Residential){
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Residential;
			selected_addon = nullptr;
			mode_str = U"residential";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
			category_buttons << pair<String, Button>(U"low_density", Button(IconFont::Residential, 16, 16, PositionStruct{0, 0}));
			category_buttons << pair<String, Button>(U"high_density", Button(IconFont::Office, 16, 16, PositionStruct{2, 0}));
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"commercial"].push()) {
		if (menu_mode != MenuMode::Commercial) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Commercial;
			selected_addon = nullptr;
			mode_str = U"commercial";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
			category_buttons << pair<String, Button>(U"low_density", Button(IconFont::Commercial, 16, 14, PositionStruct{0, 0}));
			category_buttons << pair<String, Button>(U"high_density", Button(IconFont::Office, 16, 16, PositionStruct{2, 0}));
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"office"].push()) {
		if (menu_mode != MenuMode::Office) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Office;
			selected_addon = nullptr;
			mode_str = U"office";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"industrial"].push()) {
		if (menu_mode != MenuMode::Industrial) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Industrial;
			selected_addon = nullptr;
			mode_str = U"industrial";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"farm"].push()) {
		if (menu_mode != MenuMode::Farm) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Farm;
			selected_addon = nullptr;
			mode_str = U"farm";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"public"].push()) {
		if (menu_mode != MenuMode::Public) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Public;
			selected_addon = nullptr;
			mode_str = U"public";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"park"].push()) {
		if (menu_mode != MenuMode::Park) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Park;
			selected_addon = nullptr;
			mode_str = U"park";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"ship"].push()) {
		if (menu_mode != MenuMode::Ship) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Ship;
			selected_addon = nullptr;
			mode_str = U"ship";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"air_port"].push()) {
		if (menu_mode != MenuMode::AirPort) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::AirPort;
			selected_addon = nullptr;
			mode_str = U"air_port";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	if (button[U"tile"].push()) {
		if (menu_mode != MenuMode::Tile) {
			releaseBeforeButton(menu_mode);
			menu_mode = MenuMode::Tile;
			selected_addon = nullptr;
			mode_str = U"tile";
			show_addons = map->getFitAddons(Array<String>{mode_str});
			
			category_buttons.clear();
		}
		else {
			menu_mode = MenuMode::Cursor;
		}
	}
	
	if (menu_mode != MenuMode::None && menu_mode != MenuMode::Cursor) {
		addonMenu();
	}
	
	return selected_addon;
}

void Menu::addonMenu() {
	Rect(position.x+16, position.y-42, size.width, 42).draw(Color(9, 132, 227, 200));
	Rect(position.x+16, position.y-80, size.width, 38).draw(Color(45, 52, 54, 200));
	Rect(position.x, position.y-80, 16, 80).draw(Color(45, 52, 54, 200));
	
	if (show_addons.size() > 0) {
		int selected_i = -1, cursor_i = -1;
		for (int i=0; i<show_addons.size(); i++) {
			String addon_name = show_addons[i]->getName();
			
			bool cursor_on = (Cursor::Pos().x >= 30+32*i && Cursor::Pos().y >= position.y-40 && Cursor::Pos().x < 30+32*(i+1) && Cursor::Pos().y <= position.y-40+32);
			if (cursor_on || selected_addon_name == addon_name) {
				show_addons[i]->drawIcon(PositionStruct{position.x+30+32*i, position.y-37}, PositionStruct{0, 32}, SizeStruct{32, 32});
				
				if (MouseL.down() &&
					Cursor::Pos().x >= position.x+30 && Cursor::Pos().x <= position.x-30+size.width &&
					Cursor::Pos().y >= position.y-37 && Cursor::Pos().y <= position.y-37+32) {
					if (selected_addon_name == addon_name) {
						selected_addon_name = U"";
						selected_addon = nullptr;
					}
					else {
						selected_addon_name = addon_name;
						selected_addon = show_addons[i];
					}
				}
				
				selected_i = i;
				
				if (cursor_on || cursor_i == -1) {
					cursor_i = i;
				}
			}
			else {
				show_addons[i]->drawIcon(PositionStruct{position.x+30+32*i, position.y-37}, PositionStruct{0, 0}, SizeStruct{32, 32});
			}
		}
		
		if (cursor_i == selected_i && selected_i >= 0) {
			// 名前と説明
			String name_jp = show_addons[selected_i]->getNameJP();
			(*font16)(name_jp).draw(position.x+30, position.y-80+2);
			(*font12)(show_addons[selected_i]->getSummary()).draw(position.x+30, position.y-60+2);
			
			// 効果アイコン
			::map<String, EffectStruct> effects = show_addons[selected_i]->getEffects();
			int i = 0;
			int leftmost = (*font16)(name_jp).region(Scene::Width() / 2, Scene::Height() / 2).w+50;
			for (auto effect = effects.begin(); effect != effects.end(); effect++) {
				if (!effect_icons[effect->first].isEmpty()) {
					effect_icons[effect->first].draw(leftmost+i*48, position.y-75);
					(*font12)(U"{:+}"_fmt(effect->second.influence)).draw(leftmost+i*48+22, position.y-75);
					i++;
				}
			}
		}
		else if (cursor_i >= 0) {
			String name_jp = show_addons[selected_i]->getNameJP();
			(*font16)(name_jp).draw(position.x+30, position.y-80+2);
			(*font12)(show_addons[cursor_i]->getSummary()).draw(position.x+30, position.y-60+2);
			
			// 効果アイコン
			::map<String, EffectStruct> effects = show_addons[cursor_i]->getEffects();
			int i = 0;
			int leftmost = (*font16)(name_jp).region(Scene::Width() / 2, Scene::Height() / 2).w+50;
			for (auto effect = effects.begin(); effect != effects.end(); effect++) {
				if (!effect_icons[effect->first].isEmpty()) {
					effect_icons[effect->first].draw(leftmost+i*48, position.y-75);
					(*font12)(U"{:+}"_fmt(effect->second.influence)).draw(leftmost+i*48+22, position.y-75);
					i++;
				}
			}
		}
		
		for (int i=0; i<category_buttons.size(); i++) {
			category_buttons[i].second.put(PositionStruct{0, position.y-37+16*i});
			
			if (category_buttons[i].second.push()) {
				if (category_buttons[i].second.isActive()) {
					show_addons = map->getFitAddons(Array<String>{mode_str, category_buttons[i].first});
					
					for (int j=0; j<category_buttons.size(); j++) {
						if (j != i) {
							category_buttons[j].second.release();
						}
					}
				}
				else {
					show_addons = map->getFitAddons(Array<String>{mode_str});
				}
			}
		}
	}
}
