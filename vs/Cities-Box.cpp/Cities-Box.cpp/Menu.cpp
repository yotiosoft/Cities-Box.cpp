//
//  Menu.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/07.
//

#include "Menu.hpp"

void Menu::set(PositionStruct newPosition, SizeStruct newSize, CityMap* newMap, Font* newFont8, Font* newFont12, Font* newFont16) {
	position = newPosition;
	size = newSize;
	
	font8 = newFont8;
	font12 = newFont12;
	font16 = newFont16;
	
	map = newMap;
	
	selectedAddon = nullptr;
	
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
	
	menuMode = MenuMode::None;
	
	selectedAddonName = U"";
	
	population = Texture(Icon(IconFont::Population, 20));
	
	render = RenderTexture(size.width, size.height, Color(45, 52, 54));
	
	showRateMenu = false;
	
	// 効果テクスチャを用意
	effectIcons[U"crime_rate"] = Texture(Icon(IconFont::Crime, 16));
	effectIcons[U"durability"] = Texture(Icon(IconFont::Durability, 16));
	effectIcons[U"education_rate"] = Texture(Icon(IconFont::Education, 16));
	effectIcons[U"firing_rate"] = Texture(Icon(IconFont::Firing, 16));
	effectIcons[U"garbage_disposal"] = Texture(Icon(IconFont::Garbage, 16));
	effectIcons[U"land_price"] = Texture(Icon(IconFont::LandPrice, 16));
	effectIcons[U"mobile_communication"] = Texture(Icon(IconFont::MobileCommunication, 16));
	effectIcons[U"noise"] = Texture(Icon(IconFont::Noise, 16));
	effectIcons[U"post"] = Texture(Icon(IconFont::Post, 16));
	effectIcons[U"television"] = Texture(Icon(IconFont::Television, 16));
	effectIcons[U"tourist_attraction"] = Texture(Icon(IconFont::Tourist, 16));
	effectIcons[U"radio"] = Texture(Icon(IconFont::Radio, 16));
	
	// レート表示用ボタン
	rateButton[U"land_price"].set(IconFont::LandPrice, 16, 16, PositionStruct{0, 0});
	rateButton[U"crime_rate"].set(IconFont::Crime, 16, 16, PositionStruct{0, 0});
	rateButton[U"education_rate"].set(IconFont::Education, 16, 16, PositionStruct{0, 0});
	rateButton[U"happiness_rate"].set(IconFont::Happiness, 16, 16, PositionStruct{0, 0});
}

void Menu::releaseBeforeButton(MenuMode::Type beforeSelectedButton) {
	switch (beforeSelectedButton) {
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
		case MenuMode::Delete:
			button[U"delete"].release();
			return;
		case MenuMode::Save:
			button[U"save"].release();
			return;
		case MenuMode::None:
			return;
	}
}

void Menu::getSelectedAddon() {
	
}

void Menu::update() {
	render.clear(Color(45, 52, 54));
	ScopedRenderTarget2D rt(render);
	
	button[U"cursor"].putRelative(PositionStruct{10, 0}, position);
	 
	button[U"road"].putRelative(PositionStruct{55, 0}, position);
	button[U"train"].putRelative(PositionStruct{55+35, 0}, position);
	button[U"residential"].putRelative(PositionStruct{55+70, 0}, position);
	button[U"commercial"].putRelative(PositionStruct{55+105, 0}, position);
	button[U"office"].putRelative(PositionStruct{55+140, 0}, position);
	button[U"industrial"].putRelative(PositionStruct{55+175, 0}, position);
	button[U"farm"].putRelative(PositionStruct{55+210, 0}, position);
	button[U"public"].putRelative(PositionStruct{55+245, 0}, position);
	
	button[U"park"].putRelative(PositionStruct{345, 0}, position);
	button[U"ship"].putRelative(PositionStruct{345+35, 0}, position);
	button[U"air_port"].putRelative(PositionStruct{345+70, 0}, position);
	button[U"tile"].putRelative(PositionStruct{345+105, 0}, position);
	
	button[U"information"].putRelative(PositionStruct{495, 0}, position);
	button[U"rate"].putRelative(PositionStruct{495+35, 0}, position);
	button[U"delete"].putRelative(PositionStruct{495+70, 0}, position);
	button[U"budget"].putRelative(PositionStruct{495+105, 0}, position);
	button[U"setting"].putRelative(PositionStruct{495+140, 0}, position);
	button[U"save"].putRelative(PositionStruct{495+175, 0}, position);
	
	// RCOIFメータを表示
	(*font8)(U"R").draw(size.width-80+3, size.height-15+2, Color(Palette::White));
	(*font8)(U"C").draw(size.width-80+3+8, size.height-15+2, Color(Palette::White));
	(*font8)(U"O").draw(size.width-80+3+16, size.height-15+2, Color(Palette::White));
	(*font8)(U"I").draw(size.width-80+3+24, size.height-15+2, Color(Palette::White));
	(*font8)(U"F").draw(size.width-80+3+32, size.height-15+2, Color(Palette::White));
	
	Rect(size.width-80, size.height-15, 8, -max(map->getDemand().residential*0.3, 1.0)).draw(Color(39, 174, 96));
	Rect(size.width-80+8, size.height-15, 8, -max(map->getDemand().commercial*0.3, 1.0)).draw(Color(9, 132, 227));
	Rect(size.width-80+16, size.height-15, 8, -max(map->getDemand().office*0.3, 1.0)).draw(Color(0, 206, 201));
	Rect(size.width-80+24, size.height-15, 8, -max(map->getDemand().industrial*0.3, 1.0)).draw(Color(253, 203, 110));
	Rect(size.width-80+32, size.height-15, 8, -max(map->getDemand().farm*0.3, 1.0)).draw(Color(211, 84, 0));
	
	//population.draw(10, size.height-25);
	//(*font16)(Format(map->getPopulation())).draw(10+30, size.height-25-3, Color(Palette::White));
	 
	//(*font16)(U"§ "+Format(map->getMoney())).draw(10+100, size.height-25-3, Color(Palette::White));
}

Addon* Menu::draw(bool& needUpdate) {
	render.draw(position.x, position.y);
	
	// ボタンが押されたときの動作
	if (button[U"cursor"].pushRelative(position)) {
		if (menuMode != MenuMode::Cursor) {
			releaseBeforeButton(menuMode);
		}
		
		menuMode = MenuMode::Cursor;
		selectedAddon = nullptr;
		showAddons.clear();
		button[U"cursor"].release();
		needUpdate = true;
	}
	if (button[U"road"].pushRelative(position)) {
		if (menuMode != MenuMode::Road) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Road;
			selectedAddon = nullptr;
			modeStr = U"road";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
			categoryButtons << pair<String, Button>(U"promenade", Button(IconFont::WALK, 16, 16, PositionStruct{2, 0}));
			categoryButtons << pair<String, Button>(U"car", Button(IconFont::CAR, 16, 16, PositionStruct{0, 0}));
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"train"].pushRelative(position)) {
		if (menuMode != MenuMode::Train) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Train;
			selectedAddon = nullptr;
			modeStr = U"train";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"residential"].pushRelative(position)) {
		if (menuMode != MenuMode::Residential){
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Residential;
			selectedAddon = nullptr;
			modeStr = U"residential";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
			categoryButtons << pair<String, Button>(U"low_density", Button(IconFont::Residential, 16, 16, PositionStruct{0, 0}));
			categoryButtons << pair<String, Button>(U"high_density", Button(IconFont::Office, 16, 16, PositionStruct{2, 0}));
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"commercial"].pushRelative(position)) {
		if (menuMode != MenuMode::Commercial) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Commercial;
			selectedAddon = nullptr;
			modeStr = U"commercial";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
			categoryButtons << pair<String, Button>(U"low_density", Button(IconFont::Commercial, 16, 14, PositionStruct{0, 0}));
			categoryButtons << pair<String, Button>(U"high_density", Button(IconFont::Office, 16, 16, PositionStruct{2, 0}));
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"office"].pushRelative(position)) {
		if (menuMode != MenuMode::Office) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Office;
			selectedAddon = nullptr;
			modeStr = U"office";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"industrial"].pushRelative(position)) {
		if (menuMode != MenuMode::Industrial) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Industrial;
			selectedAddon = nullptr;
			modeStr = U"industrial";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"farm"].pushRelative(position)) {
		if (menuMode != MenuMode::Farm) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Farm;
			selectedAddon = nullptr;
			modeStr = U"farm";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"public"].pushRelative(position)) {
		if (menuMode != MenuMode::Public) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Public;
			selectedAddon = nullptr;
			modeStr = U"public";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"park"].pushRelative(position)) {
		if (menuMode != MenuMode::Park) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Park;
			selectedAddon = nullptr;
			modeStr = U"park";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"ship"].pushRelative(position)) {
		if (menuMode != MenuMode::Ship) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Ship;
			selectedAddon = nullptr;
			modeStr = U"ship";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"air_port"].pushRelative(position)) {
		if (menuMode != MenuMode::AirPort) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::AirPort;
			selectedAddon = nullptr;
			modeStr = U"air_port";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (button[U"tile"].pushRelative(position)) {
		if (menuMode != MenuMode::Tile) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Tile;
			selectedAddon = nullptr;
			modeStr = U"tile";
			showAddons = map->getFitAddons(Array<String>{modeStr});
			
			categoryButtons.clear();
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	
	if (button[U"delete"].pushRelative(position)) {
		if (menuMode != MenuMode::Delete) {
			releaseBeforeButton(menuMode);
			menuMode = MenuMode::Delete;
			selectedAddon = map->getAllAddons()[U"tile_greenfield"];
			selectedAddonName = selectedAddon->getName();
			modeStr = U"delete";
			showAddons.clear();
		}
		else {
			menuMode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	
	if (button[U"rate"].pushRelative(position)) {
		showRateMenu = !showRateMenu;
		needUpdate = true;
	}
	
	if (button[U"save"].pushRelative(position)) {
		map->save();
		button[U"save"].release();
	}
	
	if (showRateMenu) {
		if (rateMenu()) {
			needUpdate = true;
		}
	}
	
	return selectedAddon;
}

void Menu::addonMenu() {
	if (showAddons.size() == 0) {
		return;
	}
	
	Rect(position.x+16, position.y-42, size.width, 42).draw(Color(9, 132, 227, 200));
	Rect(position.x+16, position.y-80, size.width, 38).draw(Color(45, 52, 54, 200));
	Rect(position.x, position.y-80, 16, 80).draw(Color(45, 52, 54, 200));
	
	if (showAddons.size() > 0) {
		int selectedI = -1, cursorI = -1;
		for (int i=0; i<showAddons.size(); i++) {
			String addonName = showAddons[i]->getName();
			
			bool cursorOn = (Cursor::Pos().x >= 30+32*i && Cursor::Pos().y >= position.y-40 && Cursor::Pos().x < 30+32*(i+1) && Cursor::Pos().y <= position.y-40+32);
			if (cursorOn || selectedAddonName == addonName) {
				showAddons[i]->drawIcon(PositionStruct{position.x+30+32*i, position.y-37}, PositionStruct{0, 32}, SizeStruct{32, 32});
				
				if (MouseL.down() &&
					Cursor::Pos().x >= position.x+30 && Cursor::Pos().x <= position.x-30+size.width &&
					Cursor::Pos().y >= position.y-37 && Cursor::Pos().y <= position.y-37+32) {
					if (selectedAddonName == addonName) {
						selectedAddonName = U"";
						selectedAddon = nullptr;
					}
					else {
						selectedAddonName = addonName;
						selectedAddon = showAddons[i];
					}
				}
				
				selectedI = i;
				
				if (cursorOn || cursorI == -1) {
					cursorI = i;
				}
			}
			else {
				showAddons[i]->drawIcon(PositionStruct{position.x+30+32*i, position.y-37}, PositionStruct{0, 0}, SizeStruct{32, 32});
			}
		}
		
		if (cursorI == selectedI && selectedI >= 0) {
			// 名前と説明
			String nameJP = showAddons[selectedI]->getNameJP();
			(*font16)(nameJP).draw(position.x+30, position.y-80+2);
			(*font12)(showAddons[selectedI]->getSummary()).draw(position.x+30, position.y-60+2);
			
			// 効果アイコン
			::map<String, EffectStruct> effects = showAddons[selectedI]->getEffects();
			int i = 0;
			int leftmost = (*font16)(nameJP).region(Scene::Width() / 2, Scene::Height() / 2).w+50;
			for (auto effect = effects.begin(); effect != effects.end(); effect++) {
				if (!effectIcons[effect->first].isEmpty()) {
					effectIcons[effect->first].draw(leftmost+i*48, position.y-75);
					(*font12)(U"{:+}"_fmt(effect->second.influence)).draw(leftmost+i*48+22, position.y-75);
					i++;
				}
			}
		}
		else if (cursorI >= 0) {
			String nameJP = showAddons[cursorI]->getNameJP();
			(*font16)(nameJP).draw(position.x+30, position.y-80+2);
			(*font12)(showAddons[cursorI]->getSummary()).draw(position.x+30, position.y-60+2);
			
			// 効果アイコン
			::map<String, EffectStruct> effects = showAddons[cursorI]->getEffects();
			int i = 0;
			int leftmost = (*font16)(nameJP).region(Scene::Width() / 2, Scene::Height() / 2).w+50;
			for (auto effect = effects.begin(); effect != effects.end(); effect++) {
				if (!effectIcons[effect->first].isEmpty()) {
					effectIcons[effect->first].draw(leftmost+i*48, position.y-75);
					(*font12)(U"{:+}"_fmt(effect->second.influence)).draw(leftmost+i*48+22, position.y-75);
					i++;
				}
			}
		}
		
		for (int i=0; i<categoryButtons.size(); i++) {
			categoryButtons[i].second.put(PositionStruct{0, position.y-37+16*i});
			
			if (categoryButtons[i].second.push()) {
				if (categoryButtons[i].second.isActive()) {
					showAddons = map->getFitAddons(Array<String>{modeStr, categoryButtons[i].first});
					
					for (int j=0; j<categoryButtons.size(); j++) {
						if (j != i) {
							categoryButtons[j].second.release();
						}
					}
				}
				else {
					showAddons = map->getFitAddons(Array<String>{modeStr});
				}
			}
		}
	}
}

bool Menu::rateMenu() {
	Rect(position.x+495+35+16-32*4/2, position.y-32*3, 32*4, 32*3).draw(Color(100, 100, 100));
	
	rateButton[U"land_price"].put(PositionStruct{position.x+495+35+16-32*4/2+7, position.y-32*3+5});
	rateButton[U"crime_rate"].put(PositionStruct{position.x+495+35+16-32*4/2+7+32, position.y-32*3+5});
	rateButton[U"education_rate"].put(PositionStruct{position.x+495+35+16-32*4/2+7+32*2, position.y-32*3+5});
	rateButton[U"happiness_rate"].put(PositionStruct{position.x+495+35+16-32*4/2+7+32*3, position.y-32*3+5});
	
	if (rateButton[U"land_price"].push()) {
		if (showRateName != U"land_price") {
			showRateName = U"land_price";
			for (auto b = rateButton.begin(); b != rateButton.end(); b++) {
				if (b->first != showRateName) {
					b->second.release();
				}
			}
			
			map->setShowRate(showRateName);
			return true;
		}
		else {
			showRateName = U"";
			map->setShowRate(showRateName);
			rateButton[U"land_price"].release();
			return true;
		}
	}
	
	if (rateButton[U"crime_rate"].push()) {
		if (showRateName != U"crime_rate") {
			showRateName = U"crime_rate";
			for (auto b = rateButton.begin(); b != rateButton.end(); b++) {
				if (b->first != showRateName) {
					b->second.release();
				}
			}
			
			map->setShowRate(showRateName);
			return true;
		}
		else {
			showRateName = U"";
			map->setShowRate(showRateName);
			rateButton[U"crime_rate"].release();
			return true;
		}
	}
	
	if (rateButton[U"education_rate"].push()) {
		if (showRateName != U"education_rate") {
			showRateName = U"education_rate";
			for (auto b = rateButton.begin(); b != rateButton.end(); b++) {
				if (b->first != showRateName) {
					b->second.release();
				}
			}
			
			map->setShowRate(showRateName);
			return true;
		}
		else {
			showRateName = U"";
			map->setShowRate(showRateName);
			rateButton[U"education_rate"].release();
			return true;
		}
	}
	
	if (rateButton[U"happiness_rate"].push()) {
		if (showRateName != U"happiness_rate") {
			showRateName = U"happiness_rate";
			for (auto b = rateButton.begin(); b != rateButton.end(); b++) {
				if (b->first != showRateName) {
					b->second.release();
				}
			}
			
			map->setShowRate(showRateName);
			return true;
		}
		else {
			showRateName = U"";
			map->setShowRate(showRateName);
			rateButton[U"happiness_rate"].release();
			return true;
		}
	}
	
	return false;
}
