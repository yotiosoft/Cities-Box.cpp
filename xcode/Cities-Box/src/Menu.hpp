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
	void set(PositionStruct new_position, SizeStruct new_size, CityMap* new_map, Font* new_font8, Font* new_font12, Font* new_font16);
	
	void releaseBeforeButton(MenuMode::Type before_selected_button);
	
	void getSelectedAddon();
	void update();
	Addon* draw(bool& need_update);
	
	void addonMenu();
	
	bool rateMenu();
	
private:
	PositionStruct position;
	SizeStruct size;
	
	RenderTexture render;
	
	map<String, Button> button;
	map<String, Button> rate_button;
	
	MenuMode::Type menu_mode;
	
	Array<Addon*> show_addons;
	Addon* selected_addon;
	
	Font* font8;
	Font* font12;
	Font* font16;
	
	CityMap* map;
	
	::map<String, Texture> effect_icons;
	
	String selected_addon_name;
	
	Array<pair<String, Button>> category_buttons;
	String mode_str;
	
	bool show_rate_menu;
	String show_rate_name;
	
	Texture population;
};

#endif /* Menu_hpp */
