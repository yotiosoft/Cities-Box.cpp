//
//  Menu.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/07.
//

#ifndef Menu_hpp
#define Menu_hpp

#include "Specific.hpp"
#include "Addon.hpp"
#include "Button.hpp"
#include "CityMap.hpp"

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
	void set(PositionStruct newPosition, SizeStruct newSize, CityMap* newMap, Font* newFont8, Font* newFont12, Font* newFont16);
	
	void releaseBeforeButton(MenuMode::Type beforeSelectedButton);
	
	void getSelectedAddon();
	void update();
	Addon* draw(bool& needUpdate);
	
	void addonMenu();
	
	bool rateMenu();
	
private:
	PositionStruct m_position;
	SizeStruct m_size;
	
	RenderTexture m_render;
	
	map<String, Button> m_button;
	map<String, Button> m_rate_button;
	
	MenuMode::Type m_menu_mode;
	
	Array<Addon*> m_show_addons;
	Addon* m_selected_addon;
	
	Font* m_font8;
	Font* m_font12;
	Font* m_font16;
	
	CityMap* m_map;
	
	::map<String, Texture> m_effect_icons;
	
	String _mselected_addon_name;
	
	Array<pair<String, Button>> m_category_buttons;
	String m_mode_str;
	
	bool m_show_rate_menu;
	String m_show_rate_name;
	
	Texture m_population;
};

#endif /* Menu_hpp */
