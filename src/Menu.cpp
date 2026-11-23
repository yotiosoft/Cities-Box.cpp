//
//  Menu.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/07.
//

#include "Menu.hpp"
#include "Sound.hpp"

void Menu::set(PositionStruct newPosition, Size newSize, CityMap* newMap, Sound* newSound, Font* newFont8, Font* newFont12, Font* newFont16) {
	m_position = newPosition;
	m_size = newSize;
	
	m_font8 = newFont8;
	m_font12 = newFont12;
	m_font16 = newFont16;
	
	m_map = newMap;
	m_sound = newSound;
	
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
	m_button[U"bgm"].set(IconFont::Music, 30, 28, PositionStruct{1, 1});
	m_button[U"save"].set(IconFont::Save, 30, 30, PositionStruct{2, 2});
	
	m_menu_mode = MenuMode::None;
	
	m_selected_addon_name = U"";

	m_render = RenderTexture(Scene::Width(), m_size.y, Color(45, 52, 54));
    m_menu_buttons_pos = PositionStruct{ Scene::Width()/2 - m_size.x/2, 0 };
	
	m_show_rate_menu = false;
	m_show_budget_menu = false;
	m_show_bgm_menu = false;
	m_bgm_scroll = 0;
	
	// 効果テクスチャを用意
	const int effectIconSize = UIConfig::scaleIcon(16);
	m_effect_icons[RateID::CrimeRate] = Texture{Icon(IconFont::Crime), effectIconSize};
	m_effect_icons[RateID::EducationRate] = Texture{Icon(IconFont::Education), effectIconSize};
	m_effect_icons[RateID::LandPrice] = Texture{Icon(IconFont::LandPrice), effectIconSize};
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

void Menu::update(const SimulationSnapshot& simulation) {
	m_render.clear(Color(45, 52, 54));
	ScopedRenderTarget2D rt(m_render);
	
	const int step = UIConfig::ToolbarIconStep;
	const int cursorX = m_menu_buttons_pos.x + 10;
	const int roadX = cursorX + step + 10;
	const int parkX = roadX + step * 8 + 10;
	const int actionX = parkX + step * 4 + 10;
	const int demandX = m_menu_buttons_pos.x + m_size.x - 80;
	const int actionStep = Min(step, Max(30, (demandX - actionX - 30) / 6));
	const int toolbarY = (m_size.y - UIConfig::scaleIcon(30)) / 2;

	m_button[U"cursor"].putRelative(PositionStruct{cursorX, toolbarY}, m_position);

	m_button[U"road"].putRelative(PositionStruct{roadX, toolbarY}, m_position);
	m_button[U"train"].putRelative(PositionStruct{roadX + step, toolbarY}, m_position);
	m_button[U"residential"].putRelative(PositionStruct{roadX + step * 2, toolbarY}, m_position);
	m_button[U"commercial"].putRelative(PositionStruct{roadX + step * 3, toolbarY}, m_position);
	m_button[U"office"].putRelative(PositionStruct{roadX + step * 4, toolbarY}, m_position);
	m_button[U"industrial"].putRelative(PositionStruct{roadX + step * 5, toolbarY}, m_position);
	m_button[U"farm"].putRelative(PositionStruct{roadX + step * 6, toolbarY}, m_position);
	m_button[U"public"].putRelative(PositionStruct{roadX + step * 7, toolbarY}, m_position);

	m_button[U"park"].putRelative(PositionStruct{parkX, toolbarY}, m_position);
	m_button[U"ship"].putRelative(PositionStruct{parkX + step, toolbarY}, m_position);
	m_button[U"air_port"].putRelative(PositionStruct{parkX + step * 2, toolbarY}, m_position);
	m_button[U"tile"].putRelative(PositionStruct{parkX + step * 3, toolbarY}, m_position);

	m_button[U"information"].putRelative(PositionStruct{actionX, toolbarY}, m_position);
	m_button[U"rate"].putRelative(PositionStruct{actionX + actionStep, toolbarY}, m_position);
	m_button[U"delete"].putRelative(PositionStruct{actionX + actionStep * 2, toolbarY}, m_position);
	m_button[U"budget"].putRelative(PositionStruct{actionX + actionStep * 3, toolbarY}, m_position);
	m_button[U"setting"].putRelative(PositionStruct{actionX + actionStep * 4, toolbarY}, m_position);
	m_button[U"bgm"].putRelative(PositionStruct{actionX + actionStep * 5, toolbarY}, m_position);
	m_button[U"save"].putRelative(PositionStruct{actionX + actionStep * 6, toolbarY}, m_position);
	
	// RCOIFメータを表示
	(*m_font8)(U"R").draw(m_menu_buttons_pos.x + m_size.x-80+3, m_size.y-15+2, Color(Palette::White));
	(*m_font8)(U"C").draw(m_menu_buttons_pos.x + m_size.x-80+3+8, m_size.y-15+2, Color(Palette::White));
	(*m_font8)(U"O").draw(m_menu_buttons_pos.x + m_size.x-80+3+16, m_size.y-15+2, Color(Palette::White));
	(*m_font8)(U"I").draw(m_menu_buttons_pos.x + m_size.x-80+3+24, m_size.y-15+2, Color(Palette::White));
	(*m_font8)(U"F").draw(m_menu_buttons_pos.x + m_size.x-80+3+32, m_size.y-15+2, Color(Palette::White));
	
	Rect(m_menu_buttons_pos.x + m_size.x-80, m_size.y-15, 8, -max(simulation.demand.residential*0.3, 1.0)).draw(Color(39, 174, 96));
	Rect(m_menu_buttons_pos.x + m_size.x-80+8, m_size.y-15, 8, -max(simulation.demand.commercial*0.3, 1.0)).draw(Color(9, 132, 227));
	Rect(m_menu_buttons_pos.x + m_size.x-80+16, m_size.y-15, 8, -max(simulation.demand.office*0.3, 1.0)).draw(Color(0, 206, 201));
	Rect(m_menu_buttons_pos.x + m_size.x-80+24, m_size.y-15, 8, -max(simulation.demand.industrial*0.3, 1.0)).draw(Color(253, 203, 110));
	Rect(m_menu_buttons_pos.x + m_size.x-80+32, m_size.y-15, 8, -max(simulation.demand.farm*0.3, 1.0)).draw(Color(211, 84, 0));
	
}

CBAddon* Menu::draw(bool& needUpdate, SimulationSnapshot& simulation) {
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
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
			m_show_addons.clear();
			m_menu_mode = MenuMode::Cursor;
		}
		needUpdate = true;
	}
	
	if (m_button[U"rate"].pushRelative(m_position)) {
		m_show_rate_menu = !m_show_rate_menu;
		if (m_show_rate_menu) {
			m_show_budget_menu = false;
			m_show_bgm_menu = false;
			m_button[U"budget"].release();
			m_button[U"bgm"].release();
		}
		needUpdate = true;
	}

	if (m_button[U"budget"].pushRelative(m_position)) {
		m_show_budget_menu = !m_show_budget_menu;
		if (m_show_budget_menu) {
			m_show_rate_menu = false;
			m_show_bgm_menu = false;
			m_button[U"rate"].release();
			m_button[U"bgm"].release();
		}
		needUpdate = true;
	}

	if (m_button[U"bgm"].pushRelative(m_position)) {
		m_show_bgm_menu = !m_show_bgm_menu;
		m_bgm_scroll = 0;
		if (m_show_bgm_menu) {
			m_show_rate_menu = false;
			m_show_budget_menu = false;
			m_button[U"rate"].release();
			m_button[U"budget"].release();
		}
		needUpdate = true;
	}
	
	if (m_button[U"save"].pushRelative(m_position)) {
		bool saveSucceeded = true;
#if SIV3D_PLATFORM(WEB)
		String savePath;
		if (specific::selectSaveDataDestination(savePath, m_map->getMapFilePath())) {
			saveSucceeded = m_map->saveAs(savePath);
		}
#else
		saveSucceeded = m_map->save();
#endif
		if (!saveSucceeded) {
			UnitaryTools::showErrorMessage(U"マップの保存に失敗しました。", m_map->getLastSaveError());
		}
		m_button[U"save"].release();
	}
	
	if (m_show_rate_menu) {
		if (rateMenu()) {
			needUpdate = true;
		}
	}

	if (m_show_budget_menu) {
		if (budgetMenu(simulation)) {
			needUpdate = true;
		}
	}

	return m_selected_addon;
}

bool Menu::isPointerOverPopup() const {
	return m_show_bgm_menu && m_bgm_popup_rect.mouseOver();
}

bool Menu::bgmMenu() {
	if (!m_show_bgm_menu || !m_sound) {
		return false;
	}

	const int panelWidth = 300;
	const int headerHeight = 42;
	const int toggleHeight = 38;
	const int rowHeight = 32;
	const int maxVisibleRows = 6;
	const int trackCount = static_cast<int>(m_sound->getBGMCount());
	const int visibleRows = Min(Max(trackCount, 1), maxVisibleRows);
	const int panelHeight = headerHeight + toggleHeight + visibleRows * rowHeight + 12;
	const int bgmCenterX = m_position.x + static_cast<int>(m_button[U"bgm"].getHitbox().center().x);
	const int left = Clamp(bgmCenterX - panelWidth / 2, 0, Scene::Width() - panelWidth);
	const int top = m_position.y - panelHeight;
	m_bgm_popup_rect = Rect(left, top, panelWidth, panelHeight);
	m_bgm_popup_rect.draw(Color(45, 52, 54, 245));
	m_bgm_popup_rect.drawFrame(1, Color(95, 102, 104));

	(*m_font16)(U"BGM設定").draw(left + 14, top + 9);

	const Rect toggleRow(left + 10, top + headerHeight, panelWidth - 20, toggleHeight - 4);
	if (toggleRow.mouseOver()) {
		toggleRow.draw(Color(65, 72, 74));
	}
	(*m_font12)(U"BGM").draw(left + 16, toggleRow.y + 8);
	const Rect toggle(left + panelWidth - 76, toggleRow.y + 5, 54, 24);
	toggle.draw(m_sound->isBGMEnabled() ? Color(9, 132, 227) : Color(90, 95, 96));
	(*m_font12)(m_sound->isBGMEnabled() ? U"ON" : U"OFF").drawAt(toggle.center());

	bool changed = false;
	if (toggleRow.leftClicked()) {
		m_sound->setBGMEnabled(!m_sound->isBGMEnabled());
		changed = true;
	}

	const int listTop = top + headerHeight + toggleHeight;
	const Rect listRect(left + 10, listTop, panelWidth - 20, visibleRows * rowHeight);
	const int maxScroll = Max(trackCount - visibleRows, 0);
	if (listRect.mouseOver() && maxScroll > 0) {
		const double wheel = Mouse::Wheel();
		if (wheel > 0) {
			m_bgm_scroll = Max(m_bgm_scroll - 1, 0);
		} else if (wheel < 0) {
			m_bgm_scroll = Min(m_bgm_scroll + 1, maxScroll);
		}
	}
	m_bgm_scroll = Clamp(m_bgm_scroll, 0, maxScroll);

	if (trackCount == 0) {
		(*m_font12)(U"再生できる曲がありません").draw(left + 16, listTop + 8, Color(190));
	} else {
		for (int row = 0; row < visibleRows; ++row) {
			const int index = m_bgm_scroll + row;
			if (index >= trackCount) {
				break;
			}
			const Rect trackRow(left + 10, listTop + row * rowHeight, panelWidth - 30, rowHeight);
			const bool selected = (index == m_sound->getPlayingBGMIndex());
			if (selected) {
				trackRow.draw(Color(9, 132, 227, 150));
			} else if (trackRow.mouseOver()) {
				trackRow.draw(Color(75, 82, 84));
			}
			(*m_font12)(selected ? U"▶ " : U"  ").draw(trackRow.x + 6, trackRow.y + 7);
			(*m_font12)(m_sound->getBGMTitle(static_cast<size_t>(index))).draw(trackRow.x + 28, trackRow.y + 7);
			if (trackRow.leftClicked()) {
				m_sound->playBGM(static_cast<size_t>(index));
				changed = true;
			}
		}
	}

	if (maxScroll > 0) {
		const Rect scrollTrack(left + panelWidth - 14, listTop, 4, visibleRows * rowHeight);
		scrollTrack.draw(Color(75, 82, 84));
		const int thumbHeight = Max(20, scrollTrack.h * visibleRows / trackCount);
		const int thumbTravel = scrollTrack.h - thumbHeight;
		const int thumbY = scrollTrack.y + (thumbTravel * m_bgm_scroll / maxScroll);
		Rect(scrollTrack.x, thumbY, scrollTrack.w, thumbHeight).draw(Color(160, 166, 168));
	}

	return changed;
}

bool Menu::budgetMenu(SimulationSnapshot& simulation) {
	BudgetSettings budget{
		simulation.budget_police,
		simulation.budget_fire,
		simulation.budget_post,
		simulation.budget_education
	};
	TaxSettings tax{
		simulation.tax_residential,
		simulation.tax_commercial,
		simulation.tax_office,
		simulation.tax_industrial,
		simulation.tax_farm
	};

	const int panelWidth = 360;
	const int rowHeight = 25;
	const int panelHeight = 10 * rowHeight + 42;
	const int budgetCenterX = m_position.x + static_cast<int>(m_button[U"budget"].getHitbox().center().x);
	const int left = Clamp(budgetCenterX - panelWidth / 2, 0, Scene::Width() - panelWidth);
	const int top = m_position.y - panelHeight;
	Rect(left, top, panelWidth, panelHeight).draw(Color(45, 52, 54, 235));
	(*m_font16)(U"予算・税金").draw(left + 12, top + 8);
	(*m_font12)(U"各月の予算と税金。税率10%超で需要・稼働率が低下").draw(left + 12, top + 27, Color(200));

	bool changed = false;
	auto drawRow = [&](const String& label, const String& value, int row, auto decrease, auto increase) {
		const int y = top + 46 + row * rowHeight;
		(*m_font12)(label).draw(left + 12, y + 3);
		const Rect minus(left + 205, y, 28, 21);
		const Rect plus(left + 316, y, 28, 21);
		minus.draw(Color(75, 82, 84));
		plus.draw(Color(75, 82, 84));
		(*m_font16)(U"-").drawAt(minus.center());
		(*m_font16)(U"+").drawAt(plus.center());
		(*m_font12)(value).drawAt(left + 274, y + 10);
		if (minus.leftClicked()) {
			decrease();
			changed = true;
		}
		if (plus.leftClicked()) {
			increase();
			changed = true;
		}
	};
	auto decreaseBudget = [](int32& value) { value = (value < 10) ? 0 : value - 10; };
	auto increaseBudget = [](int32& value) { value = Min(value + 10, 200); };
	auto decreaseTax = [](double& value) { value = Max(value - 1.0, 0.0); };
	auto increaseTax = [](double& value) { value = Min(value + 1.0, 100.0); };

	drawRow(U"Police", Format(budget.police) + U"%", 0, [&]{ decreaseBudget(budget.police); }, [&]{ increaseBudget(budget.police); });
	drawRow(U"Fire", Format(budget.fire) + U"%", 1, [&]{ decreaseBudget(budget.fire); }, [&]{ increaseBudget(budget.fire); });
	drawRow(U"Post", Format(budget.post) + U"%", 2, [&]{ decreaseBudget(budget.post); }, [&]{ increaseBudget(budget.post); });
	drawRow(U"Education", Format(budget.education) + U"%", 3, [&]{ decreaseBudget(budget.education); }, [&]{ increaseBudget(budget.education); });
	drawRow(U"Residential", U"{:.1f}%"_fmt(tax.residential), 4, [&]{ decreaseTax(tax.residential); }, [&]{ increaseTax(tax.residential); });
	drawRow(U"Commercial", U"{:.1f}%"_fmt(tax.commercial), 5, [&]{ decreaseTax(tax.commercial); }, [&]{ increaseTax(tax.commercial); });
	drawRow(U"Office", U"{:.1f}%"_fmt(tax.office), 6, [&]{ decreaseTax(tax.office); }, [&]{ increaseTax(tax.office); });
	drawRow(U"Industrial", U"{:.1f}%"_fmt(tax.industrial), 7, [&]{ decreaseTax(tax.industrial); }, [&]{ increaseTax(tax.industrial); });
	drawRow(U"Farm", U"{:.1f}%"_fmt(tax.farm), 8, [&]{ decreaseTax(tax.farm); }, [&]{ increaseTax(tax.farm); });

	if (changed) {
		simulation = m_map->setFinanceSettings(budget, tax);
	}
	return changed;
}

void Menu::addonMenu() {
	if (m_show_addons.size() == 0) {
		return;
	}
	
	const int iconRowTop = m_position.y - UIConfig::AddonMenuIconRowHeight;
	const int detailsTop = iconRowTop - UIConfig::AddonMenuDetailsHeight;
	const int menuHeight = UIConfig::AddonMenuIconRowHeight + UIConfig::AddonMenuDetailsHeight;
	Rect(m_position.x+16, iconRowTop, Scene::Width(), UIConfig::AddonMenuIconRowHeight).draw(Color(9, 132, 227, 200));
	Rect(m_position.x+16, detailsTop, Scene::Width(), UIConfig::AddonMenuDetailsHeight).draw(Color(45, 52, 54, 200));
	Rect(m_position.x, detailsTop, 16, menuHeight).draw(Color(45, 52, 54, 200));
	
	if (m_show_addons.size() > 0) {
		int selectedI = -1, cursorI = -1;
		for (int i=0; i<(int)m_show_addons.size(); i++) {
			String addonName = m_show_addons[i]->getName(NameMode::English);
			const int iconSize = UIConfig::AddonIconSize;
			const int iconStep = UIConfig::AddonIconStep;
			const int iconX = m_position.x + 30 + iconStep * i;
			const int iconY = iconRowTop + (UIConfig::AddonMenuIconRowHeight - iconSize) / 2;
			
			bool cursorOn = (Cursor::Pos().x >= iconX && Cursor::Pos().y >= iconY && Cursor::Pos().x < iconX+iconSize && Cursor::Pos().y <= iconY+iconSize);
			if (cursorOn || m_selected_addon_name == addonName) {
				m_show_addons[i]->drawIcon(PositionStruct{iconX, iconY}, PositionStruct{0, 32}, Size(iconSize, iconSize));
				
				if (MouseL.down() &&
					Cursor::Pos().x >= m_position.x+30 && Cursor::Pos().x <= m_position.x-30+m_size.x &&
					Cursor::Pos().y >= iconY && Cursor::Pos().y <= iconY+iconSize) {
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
				m_show_addons[i]->drawIcon(PositionStruct{iconX, iconY}, PositionStruct{0, 0}, Size(iconSize, iconSize));
			}
		}
		
		if (cursorI == selectedI && selectedI >= 0) {
			// 名前と説明
			String nameJP = m_show_addons[selectedI]->getName(NameMode::Japanese);
			const int nameY = detailsTop + 4;
			const int summaryY = nameY + static_cast<int>((*m_font16)(nameJP).region().h) + UIConfig::AddonMenuTextGap;
			(*m_font16)(nameJP).draw(m_position.x+30, nameY);
			(*m_font12)(m_show_addons[selectedI]->getSummary()).draw(m_position.x+30, summaryY);
			
			// 効果アイコン
			::map<RateID::Type, EffectStruct> effects = m_show_addons[selectedI]->getEffects();
			int i = 0;
			int leftmost = m_position.x + 50 + (*m_font16)(nameJP).region().w;
			for (auto effect = effects.begin(); effect != effects.end(); effect++) {
				if (!m_effect_icons[effect->first].isEmpty()) {
					m_effect_icons[effect->first].draw(leftmost+i*48, nameY);
					(*m_font12)(U"{:+}"_fmt(effect->second.influence)).draw(leftmost+i*48+22, nameY);
					i++;
				}
			}
		}
		else if (cursorI >= 0) {
			String nameJP = m_show_addons[cursorI]->getName(NameMode::Japanese);
			const int nameY = detailsTop + 4;
			const int summaryY = nameY + static_cast<int>((*m_font16)(nameJP).region().h) + UIConfig::AddonMenuTextGap;
			(*m_font16)(nameJP).draw(m_position.x+30, nameY);
			(*m_font12)(m_show_addons[cursorI]->getSummary()).draw(m_position.x+30, summaryY);
			
			// 効果アイコン
			::map<RateID::Type, EffectStruct> effects = m_show_addons[cursorI]->getEffects();
			int i = 0;
			int leftmost = m_position.x + 50 + (*m_font16)(nameJP).region().w;
			for (auto effect = effects.begin(); effect != effects.end(); effect++) {
				if (!m_effect_icons[effect->first].isEmpty()) {
					m_effect_icons[effect->first].draw(leftmost+i*48, nameY);
					(*m_font12)(U"{:+}"_fmt(effect->second.influence)).draw(leftmost+i*48+22, nameY);
					i++;
				}
			}
		}
		
		for (int i=0; i<(int)m_category_buttons.size(); i++) {
			const int categoryIconSize = UIConfig::scaleIcon(16);
			const int categoryY = iconRowTop + UIConfig::AddonCategoryButtonGap + i * (categoryIconSize + UIConfig::AddonCategoryButtonGap);
			m_category_buttons[i].second.put(PositionStruct{0, categoryY});
			
			if (m_category_buttons[i].second.push()) {
				if (m_category_buttons[i].second.isActive()) {
					m_show_addons = m_map->getFitAddons(Array<CategoryID::Type>{m_current_category, m_category_buttons[i].first});
					
					for (int j=0; j<(int)m_category_buttons.size(); j++) {
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
	const int buttonStep = UIConfig::RatePopupButtonStep;
	const int panelWidth = buttonStep * 4;
	const int panelHeight = 32 * 3;
	const int rateCenterX = m_position.x + static_cast<int>(m_button[U"rate"].getHitbox().center().x);
	const int left = Clamp(rateCenterX - panelWidth / 2, 0, Scene::Width() - panelWidth);
	const int top = m_position.y - panelHeight;
	Rect(left, top, panelWidth, panelHeight).draw(Color(100, 100, 100));
	
	m_rate_button[RateID::LandPrice].put(PositionStruct{left + 7, top + 5});
	m_rate_button[RateID::CrimeRate].put(PositionStruct{left + 7 + buttonStep, top + 5});
	m_rate_button[RateID::EducationRate].put(PositionStruct{left + 7 + buttonStep * 2, top + 5});
	m_rate_button[RateID::HappinessRate].put(PositionStruct{left + 7 + buttonStep * 3, top + 5});
	
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
