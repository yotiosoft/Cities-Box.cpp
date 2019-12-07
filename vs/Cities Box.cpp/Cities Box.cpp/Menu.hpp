//
//  Menu.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/07.
//

#ifndef Menu_hpp
#define Menu_hpp

#include "Specific.hpp"
#include "Button.hpp"
#include "CityMap.hpp"

class Menu {
public:
	void set(PositionStruct new_position, SizeStruct new_size, Font* new_font8, Font* new_font16);
	
	void draw(RCOIFstruct demand, int population_count, int money);
	
private:
	PositionStruct position;
	SizeStruct size;
	
	RenderTexture render;
	
	Button cursorButton;
	
	Button residentialButton;
	Button commercialButton;
	Button officeButton;
	Button industrialButton;
	Button farmButton;
	Button publicButton;
	
	Button parkButton;
	Button waterWayButton;
	Button airPortButton;
	Button fieldButton;
	
	Button informationButton;
	Button rateButton;
	Button deleteButton;
	Button budgetButton;
	Button settingButton;
	Button saveButton;
	
	Font* font8;
	Font* font16;
	
	Texture population;
};

#endif /* Menu_hpp */
