//
//  Menu.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/07.
//

#include "Menu.hpp"

void Menu::set(PositionStruct newPosition, Size newSize, CityMap* newMap, Font* newFont8, Font* newFont12, Font* newFont16) {
	m_position = newPosition;
	m_size = newSize;
	
	m_font8 = newFont8;
	m_font12 = newFont12;
	m_font16 = newFont16;
	
	m_map = newMap;
	
	m_selected_addon = nullptr;
	
	m_button[U"cursor"].set(IconFont::Cursor, 30, 26, PositionStruct{8, 2});
	
	m_button[U"road"].set(IconFont::Road, 30, 26, PositionStruct{ 0, 6 });
	m_button[U"train"].set(IconFont::Railroad, 30, 28, PositionStruct{ 2, 2 });
	m_button[U"residential"].set(IconFont::Residential, 30, 30, PositionStruct{-3, 2});
	m_button[U"commercial"].set(IconFont::Commercial, 30, 26, PositionStruct{-3, 2});
	m_button[U"office"].set(IconFont::Office, 30, 30, PositionStruct{0, 1});
	m_button[U"industrial"].set(IconFont::Industrial, 30, 30, PositionStruct{0, 2});
	m_button[U"farm"].set(IconFont::Farm, 30, 26, PositionStruct{-3, 2});
	m_button[U"public"].set(IconFont::Public, 30, 30, PositionStruct{0, 2});
	
	m_button[U"park"].set(IconFont::Park, 30, 30, PositionStruct{2, 2});
	m_button[U"ship"].set(IconFont::WaterWay, 30, 26, PositionStruct{0, 2});
	m_button[U"air_port"].set(IconFont::AirPort, 30, 24, PositionStruct{-1, 4});
	m_button[U"tile"].set(IconFont::Field, 30, 30, PositionStruct{2, 2});
	
	m_button[U"information"].set(IconFont::Information, 30, 26, PositionStruct{2, 2});
	m_button[U"rate"].set(IconFont::Rate, 30, 30, PositionStruct{0, 2});
	m_button[U"delete"].set(IconFont::Delete, 30, 30, PositionStruct{0, 2});
	m_button[U"budget"].set(IconFont::Budget, 30, 26, PositionStruct{-1, 4});
	m_button[U"setting"].set(IconFont::Setting, 30, 28, PositionStruct{1, 1});
	m_button[U"save"].set(IconFont::Save, 30, 30, PositionStruct{2, 2});
	
	m_menu_mode = MenuMode::None;
	
	m_selected_addon_name = U"";
	
	m_population = Texture{Icon(IconFont::Population), 20};
	
	m_render = RenderTexture(m_size.x, m_size.y, Color(45, 52, 54));
	
	m_show_rate_menu = false;
	
	// 効果テクスチャを用意
	m_effect_icons[RateID::CrimeRate] = Texture{Icon(IconFont::Crime), 16};
	m_effect_icons[RateID::EducationRate] = Texture{Icon(IconFont::Education), 16};
	m_effect_icons[RateID::LandPrice] = Texture{Icon(IconFont::LandPrice), 16};
	/*
	 m_effect_icons[U"durability"] = Texture(Icon(IconFont::Durability, 16));
	m_effect_icons[U"firing_rate"] = Texture(Icon(IconFont::Firing, 16));
	m_effect_icons[U"garbage_disposal"] = Texture(Icon(IconFont::Garbage, 16));
	m_effect_icons[U"land_price"] = Texture(Icon(IconFont::LandPrice, 16));
	m_effect_icons[U"mobile_communication"] = Texture(Icon(IconFont::MobileCommunication, 16));
	m_effect_icons[U"noise"] = Texture(Icon(IconFont::Noise, 16));
	m_effect_icons[U"post"] = Texture(Icon(IconFont::Post, 16));
	m_effect_icons[U"television"] = Texture(Icon(IconFont::Television, 16));
	m_effect_icons[U"tourist_attraction"] = Texture(Icon(IconFont::Tourist, 16));
	m_effect_icons[U"radio"] = Texture(Icon(IconFont::Radio, 16));
	*/
	// レート表示用ボタン
	m_rate_button[RateID::LandPrice].set(IconFont::LandPrice, 16, 16, PositionStruct{0, 0});
	m_rate_button[RateID::CrimeRate].set(IconFont::Crime, 16, 16, PositionStruct{0, 0});
	m_rate_button[RateID::EducationRate].set(IconFont::Education, 16, 16, PositionStruct{0, 0});
	m_rate_button[RateID::HappinessRate].set(IconFont::Happiness, 16, 16, PositionStruct{0, 0});
}

void Menu::releaseBeforeButton(MenuMode::Type beforeSelectedButton) {
	switch (beforeSelectedButton) {
		case MenuMode::Cursor:
			m_button[U"cursor"].release();
			return;
		case MenuMode::Road:
			m_button[U"road"].release();
			return;
		case MenuMode::Train:
			m_button[U"train"].release();
			return;
		case MenuMode::Residential:
			m_button[U"residential"].release();
			return;
		case MenuMode::Commercial:
			m_button[U"commercial"].release();
			return;
		case MenuMode::Office:
			m_button[U"office"].release();
			return;
		case MenuMode::Industrial:
			m_button[U"industrial"].release();
			return;
		case MenuMode::Farm:
			m_button[U"farm"].release();
			return;
		case MenuMode::Public:
			m_button[U"public"].release();
			return;
		case MenuMode::Park:
			m_button[U"park"].release();
			return;
		case MenuMode::Ship:
			m_button[U"ship"].release();
			return;
		case MenuMode::AirPort:
			m_button[U"air_port"].release();
			return;
		case MenuMode::Tile:
			m_button[U"tile"].release();
			return;
		case MenuMode::Delete:
			m_button[U"delete"].release();
			return;
		case MenuMode::Save:
			m_button[U"save"].release();
			return;
		case MenuMode::None:
		default:
			return;
	}
}

void Menu::getSelectedAddon() {
	
}

void Menu::update() {
	m_render.clear(Color(45, 52, 54));
	ScopedRenderTarget2D rt(m_render);
	
	m_button[U"cursor"].putRelative(PositionStruct{10, 0}, m_position);
	 
	m_button[U"road"].putRelative(PositionStruct{55, 0}, m_position);
	m_button[U"train"].putRelative(PositionStruct{55+35, 0}, m_position);
	m_button[U"residential"].putRelative(PositionStruct{55+70, 0}, m_position);
	m_button[U"commercial"].putRelative(PositionStruct{55+105, 0}, m_position);
	m_button[U"office"].putRelative(PositionStruct{55+140, 0}, m_position);
	m_button[U"industrial"].putRelative(PositionStruct{55+175, 0}, m_position);
	m_button[U"farm"].putRelative(PositionStruct{55+210, 0}, m_position);
	m_button[U"public"].putRelative(PositionStruct{55+245, 0}, m_position);
	
	m_button[U"park"].putRelative(PositionStruct{345, 0}, m_position);
	m_button[U"ship"].putRelative(PositionStruct{345+35, 0}, m_position);
	m_button[U"air_port"].putRelative(PositionStruct{345+70, 0}, m_position);
	m_button[U"tile"].putRelative(PositionStruct{345+105, 0}, m_position);
	
	m_button[U"information"].putRelative(PositionStruct{495, 0}, m_position);
	m_button[U"rate"].putRelative(PositionStruct{495+35, 0}, m_position);
	m_button[U"delete"].putRelative(PositionStruct{495+70, 0}, m_position);
	m_button[U"budget"].putRelative(PositionStruct{495+105, 0}, m_position);
	m_button[U"setting"].putRelative(PositionStruct{495+140, 0}, m_position);
	m_button[U"save"].putRelative(PositionStruct{495+175, 0}, m_position);
	
	// RCOIFメータを表示
	(*m_font8)(U"R").draw(m_size.x-80+3, m_size.y-15+2, Color(Palette::White));
	(*m_font8)(U"C").draw(m_size.x-80+3+8, m_size.y-15+2, Color(Palette::White));
	(*m_font8)(U"O").draw(m_size.x-80+3+16, m_size.y-15+2, Color(Palette::White));
	(*m_font8)(U"I").draw(m_size.x-80+3+24, m_size.y-15+2, Color(Palette::White));
	(*m_font8)(U"F").draw(m_size.x-80+3+32, m_size.y-15+2, Color(Palette::White));
	
	Rect(m_size.x-80, m_size.y-15, 8, -max(m_map->getDemand().residential*0.3, 1.0)).draw(Color(39, 174, 96));
	Rect(m_size.x-80+8, m_size.y-15, 8, -max(m_map->getDemand().commercial*0.3, 1.0)).draw(Color(9, 132, 227));
	Rect(m_size.x-80+16, m_size.y-15, 8, -max(m_map->getDemand().office*0.3, 1.0)).draw(Color(0, 206, 201));
	Rect(m_size.x-80+24, m_size.y-15, 8, -max(m_map->getDemand().industrial*0.3, 1.0)).draw(Color(253, 203, 110));
	Rect(m_size.x-80+32, m_size.y-15, 8, -max(m_map->getDemand().farm*0.3, 1.0)).draw(Color(211, 84, 0));
	
	//population.draw(10, size.height-25);
	//(*font16)(Format(map->getPopulation())).draw(10+30, size.height-25-3, Color(Palette::White));
	 
	//(*font16)(U"§ "+Format(map->getMoney())).draw(10+100, size.height-25-3, Color(Palette::White));
}

CBAddon* Menu::draw(bool& needUpdate) {
	m_render.draw(m_position.x, m_position.y);
	
	// ボタンが押されたときの動作
	if (m_button[U"cursor"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Cursor) {
			releaseBeforeButton(m_menu_mode);
            if (m_menu_mode == MenuMode::Road || m_menu_mode == MenuMode::Train) {
                m_map->breakUnconnectedRoads();         // メニューを閉じた時点で建設が完了していない道路（＝孤立した道路）は除去
            }
		}
		
		m_menu_mode = MenuMode::Cursor;
		m_selected_addon = nullptr;
		m_show_addons.clear();
		m_button[U"cursor"].release();
		needUpdate = true;
	}
	if (m_button[U"road"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Road) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Road;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Road;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
			m_category_buttons << pair<CategoryID::Type, Button>(CategoryID::Promenade, Button(IconFont::WALK, 16, 16, PositionStruct{2, 0}));
			m_category_buttons << pair<CategoryID::Type, Button>(CategoryID::Car, Button(IconFont::CAR, 16, 16, PositionStruct{0, 0}));
		}
		else {
			m_menu_mode = MenuMode::Cursor;
            m_map->breakUnconnectedRoads();         // メニューを閉じた時点で建設が完了していない道路（＝孤立した道路）は除去
		}
		needUpdate = true;
	}
	if (m_button[U"train"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Train) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Train;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Train;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
		}
		else {
			m_menu_mode = MenuMode::Cursor;
            m_map->breakUnconnectedRoads();         // メニューを閉じた時点で建設が完了していない線路（＝孤立した線路）は除去
		}
		needUpdate = true;
	}
	if (m_button[U"residential"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Residential){
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Residential;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Residential;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
			m_category_buttons << pair<CategoryID::Type, Button>(CategoryID::LowDensity, Button(IconFont::Residential, 16, 16, PositionStruct{0, 0}));
			m_category_buttons << pair<CategoryID::Type, Button>(CategoryID::HighDensity, Button(IconFont::Office, 16, 16, PositionStruct{2, 0}));
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (m_button[U"commercial"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Commercial) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Commercial;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Commecial;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
			m_category_buttons << pair<CategoryID::Type, Button>(CategoryID::LowDensity, Button(IconFont::Commercial, 16, 14, PositionStruct{0, 0}));
			m_category_buttons << pair<CategoryID::Type, Button>(CategoryID::HighDensity, Button(IconFont::Office, 16, 16, PositionStruct{2, 0}));
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (m_button[U"office"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Office) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Office;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Office;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (m_button[U"industrial"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Industrial) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Industrial;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Industrial;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (m_button[U"farm"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Farm) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Farm;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Farm;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (m_button[U"public"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Public) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Public;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Public;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (m_button[U"park"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Park) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Park;
			m_selected_addon = nullptr;
			m_current_category =CategoryID::Park;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (m_button[U"ship"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Ship) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Ship;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Ship;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (m_button[U"air_port"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::AirPort) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::AirPort;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Airport;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	if (m_button[U"tile"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Tile) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Tile;
			m_selected_addon = nullptr;
			m_current_category = CategoryID::Tile;
			m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
			
			m_category_buttons.clear();
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	
	if (m_button[U"delete"].pushRelative(m_position)) {
		if (m_menu_mode != MenuMode::Delete) {
			releaseBeforeButton(m_menu_mode);
			m_menu_mode = MenuMode::Delete;
			m_selected_addon = m_map->getAllAddons()[U"tile_greenfield"];
			m_selected_addon_name = m_selected_addon->getName(NameMode::English);
			m_current_category = CategoryID::Disabled;
			m_show_addons.clear();
		}
		else {
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	
	if (m_button[U"rate"].pushRelative(m_position)) {
		m_show_rate_menu = !m_show_rate_menu;
		needUpdate = true;
	}
	
	if (m_button[U"save"].pushRelative(m_position)) {
		m_map->save();
		m_button[U"save"].release();
	}
	
	if (m_show_rate_menu) {
		if (rateMenu()) {
			needUpdate = true;
		}
	}
	
	return m_selected_addon;
}

void Menu::addonMenu() {
	if (m_show_addons.size() == 0) {
		return;
	}
	
	Rect(m_position.x+16, m_position.y-42, m_size.x, 42).draw(Color(9, 132, 227, 200));
	Rect(m_position.x+16, m_position.y-80, m_size.x, 38).draw(Color(45, 52, 54, 200));
	Rect(m_position.x, m_position.y-80, 16, 80).draw(Color(45, 52, 54, 200));
	
	if (m_show_addons.size() > 0) {
		int selectedI = -1, cursorI = -1;
		for (int i=0; i<m_show_addons.size(); i++) {
			String addonName = m_show_addons[i]->getName(NameMode::English);
			
			bool cursorOn = (Cursor::Pos().x >= 30+32*i && Cursor::Pos().y >= m_position.y-40 && Cursor::Pos().x < 30+32*(i+1) && Cursor::Pos().y <= m_position.y-40+32);
			if (cursorOn || m_selected_addon_name == addonName) {
				m_show_addons[i]->drawIcon(PositionStruct{m_position.x+30+32*i, m_position.y-37}, PositionStruct{0, 32}, Size(32, 32));
				
				if (MouseL.down() &&
					Cursor::Pos().x >= m_position.x+30 && Cursor::Pos().x <= m_position.x-30+m_size.x &&
					Cursor::Pos().y >= m_position.y-37 && Cursor::Pos().y <= m_position.y-37+32) {
					if (m_selected_addon_name == addonName) {
						m_selected_addon_name = U"";
						m_selected_addon = nullptr;
					}
					else {
						m_selected_addon_name = addonName;
						m_selected_addon = m_show_addons[i];
					}
				}
				
				selectedI = i;
				
				if (cursorOn || cursorI == -1) {
					cursorI = i;
				}
			}
			else {
				m_show_addons[i]->drawIcon(PositionStruct{m_position.x+30+32*i, m_position.y-37}, PositionStruct{0, 0}, Size(32, 32));
			}
		}
		
		if (cursorI == selectedI && selectedI >= 0) {
			// 名前と説明
			String nameJP = m_show_addons[selectedI]->getName(NameMode::Japanese);
			(*m_font16)(nameJP).draw(m_position.x+30, m_position.y-80+2);
			(*m_font12)(m_show_addons[selectedI]->getSummary()).draw(m_position.x+30, m_position.y-60+2);
			
			// 効果アイコン
			::map<RateID::Type, EffectStruct> effects = m_show_addons[selectedI]->getEffects();
			int i = 0;
			int leftmost = (*m_font16)(nameJP).region(Scene::Width() / 2, Scene::Height() / 2).w+50;
			for (auto effect = effects.begin(); effect != effects.end(); effect++) {
				if (!m_effect_icons[effect->first].isEmpty()) {
					m_effect_icons[effect->first].draw(leftmost+i*48, m_position.y-75);
					(*m_font12)(U"{:+}"_fmt(effect->second.influence)).draw(leftmost+i*48+22, m_position.y-75);
					i++;
				}
			}
		}
		else if (cursorI >= 0) {
			String nameJP = m_show_addons[cursorI]->getName(NameMode::Japanese);
			(*m_font16)(nameJP).draw(m_position.x+30, m_position.y-80+2);
			(*m_font12)(m_show_addons[cursorI]->getSummary()).draw(m_position.x+30, m_position.y-60+2);
			
			// 効果アイコン
			::map<RateID::Type, EffectStruct> effects = m_show_addons[cursorI]->getEffects();
			int i = 0;
			int leftmost = (*m_font16)(nameJP).region(Scene::Width() / 2, Scene::Height() / 2).w+50;
			for (auto effect = effects.begin(); effect != effects.end(); effect++) {
				if (!m_effect_icons[effect->first].isEmpty()) {
					m_effect_icons[effect->first].draw(leftmost+i*48, m_position.y-75);
					(*m_font12)(U"{:+}"_fmt(effect->second.influence)).draw(leftmost+i*48+22, m_position.y-75);
					i++;
				}
			}
		}
		
		for (int i=0; i<m_category_buttons.size(); i++) {
			m_category_buttons[i].second.put(PositionStruct{0, m_position.y-37+16*i});
			
			if (m_category_buttons[i].second.push()) {
				if (m_category_buttons[i].second.isActive()) {
					m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category, m_category_buttons[i].first});
					
					for (int j=0; j<m_category_buttons.size(); j++) {
						if (j != i) {
							m_category_buttons[j].second.release();
						}
					}
				}
				else {
					m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category});
				}
			}
		}
	}
}

bool Menu::rateMenu() {
	Rect(m_position.x+495+35+16-32*4/2, m_position.y-32*3, 32*4, 32*3).draw(Color(100, 100, 100));
	
	m_rate_button[RateID::LandPrice].put(PositionStruct{m_position.x+495+35+16-32*4/2+7, m_position.y-32*3+5});
	m_rate_button[RateID::CrimeRate].put(PositionStruct{m_position.x+495+35+16-32*4/2+7+32, m_position.y-32*3+5});
	m_rate_button[RateID::EducationRate].put(PositionStruct{m_position.x+495+35+16-32*4/2+7+32*2, m_position.y-32*3+5});
	m_rate_button[RateID::HappinessRate].put(PositionStruct{m_position.x+495+35+16-32*4/2+7+32*3, m_position.y-32*3+5});
	
	if (m_rate_button[RateID::LandPrice].push()) {
		if (m_show_rate_id != RateID::LandPrice) {
			m_show_rate_id = RateID::LandPrice;
			for (auto b = m_rate_button.begin(); b != m_rate_button.end(); b++) {
				if (b->first != m_show_rate_id) {
					b->second.release();
				}
			}
			
			m_map->setShowRate(m_show_rate_id);
			return true;
		}
		else {
			m_show_rate_id = RateID::None;
			m_map->setShowRate(m_show_rate_id);
			m_rate_button[RateID::LandPrice].release();
			return true;
		}
	}
	
	if (m_rate_button[RateID::CrimeRate].push()) {
		if (m_show_rate_id != RateID::CrimeRate) {
			m_show_rate_id = RateID::CrimeRate;
			for (auto b = m_rate_button.begin(); b != m_rate_button.end(); b++) {
				if (b->first != m_show_rate_id) {
					b->second.release();
				}
			}
			
			m_map->setShowRate(m_show_rate_id);
			return true;
		}
		else {
			m_show_rate_id = RateID::None;
			m_map->setShowRate(m_show_rate_id);
			m_rate_button[RateID::CrimeRate].release();
			return true;
		}
	}
	
	if (m_rate_button[RateID::EducationRate].push()) {
		if (m_show_rate_id != RateID::EducationRate) {
			m_show_rate_id = RateID::EducationRate;
			for (auto b = m_rate_button.begin(); b != m_rate_button.end(); b++) {
				if (b->first != m_show_rate_id) {
					b->second.release();
				}
			}
			
			m_map->setShowRate(m_show_rate_id);
			return true;
		}
		else {
			m_show_rate_id = RateID::None;
			m_map->setShowRate(m_show_rate_id);
			m_rate_button[RateID::EducationRate].release();
			return true;
		}
	}
	
	if (m_rate_button[RateID::HappinessRate].push()) {
		if (m_show_rate_id != RateID::HappinessRate) {
			m_show_rate_id = RateID::HappinessRate;
			for (auto b = m_rate_button.begin(); b != m_rate_button.end(); b++) {
				if (b->first != m_show_rate_id) {
					b->second.release();
				}
			}
			
			m_map->setShowRate(m_show_rate_id);
			return true;
		}
		else {
			m_show_rate_id = RateID::None;
			m_map->setShowRate(m_show_rate_id);
			m_rate_button[RateID::HappinessRate].release();
			return true;
		}
	}
	
	return false;
}
