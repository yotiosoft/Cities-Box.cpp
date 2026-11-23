//
//  Menu.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/07.
//

#ifndef Menu_hpp
#define Menu_hpp

#include <Specific.hpp>
#include "CBAddon.hpp"
#include "Button.hpp"
#include "CityMap.hpp"

class Sound;

namespace MenuMode {
	enum Type {
		None = -1,
		Cursor = 0,
		Road = 1,
		Train = 2,
		Residential = 3,
		Commercial = 4,
		Office = 5,
		Industrial = 6,
		Farm = 7,
		Public = 8,
		Park = 9,
		Ship = 10,
		AirPort = 11,
		Tile = 12,
		
		Information = 13,
		Rate = 14,
		Delete = 15,
		Budget = 16,
		Setting = 17,
		Save = 18,
	};
}

class Menu {
public:
	void set(PositionStruct newPosition, Size newSize, CityMap* newMap, Sound* newSound, Font* newFont8, Font* newFont12, Font* newFont16);
	
	void releaseBeforeButton(MenuMode::Type beforeSelectedButton);
	
	void getSelectedAddon();
	void update(const SimulationSnapshot& simulation);
	CBAddon* draw(bool& needUpdate, SimulationSnapshot& simulation);
	
	void addonMenu();
	
	bool rateMenu();
	bool budgetMenu(SimulationSnapshot& simulation);
	bool bgmMenu();
	bool isPointerOverPopup() const;
	
private:
	PositionStruct m_position;
	Size m_size;
    PositionStruct m_menu_buttons_pos;
	
	RenderTexture m_render;
	
	map<String, Button> m_button;
	map<RateID::Type, Button> m_rate_button;
	
	MenuMode::Type m_menu_mode;
	
	Array<CBAddon*> m_show_addons;
	CBAddon* m_selected_addon;
	
	Font* m_font8;
	Font* m_font12;
	Font* m_font16;
	
	CityMap* m_map;
	Sound* m_sound;
	
	::map<RateID::Type, Texture> m_effect_icons;
	
	String m_selected_addon_name;
	
	Array<pair<CategoryID::Type, Button>> m_category_buttons;
	CategoryID::Type m_current_category;
	
	bool m_show_rate_menu;
	bool m_show_budget_menu;
	bool m_show_bgm_menu;
	int m_bgm_scroll;
	Rect m_bgm_popup_rect;
	RateID::Type m_show_rate_id;
	
};

#endif /* Menu_hpp */
