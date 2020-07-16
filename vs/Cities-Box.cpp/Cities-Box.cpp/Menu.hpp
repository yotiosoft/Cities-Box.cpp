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
	PositionStruct position;
	SizeStruct size;
	
	RenderTexture render;
	
	map<String, Button> button;
	map<String, Button> rateButton;
	
	MenuMode::Type menuMode;
	
	Array<Addon*> showAddons;
	Addon* selectedAddon;
	
	Font* font8;
	Font* font12;
	Font* font16;
	
	CityMap* map;
	
	::map<String, Texture> effectIcons;
	
	String selectedAddonName;
	
	Array<pair<String, Button>> categoryButtons;
	String modeStr;
	
	bool showRateMenu;
	String showRateName;
	
	Texture population;
};

#endif /* Menu_hpp */
