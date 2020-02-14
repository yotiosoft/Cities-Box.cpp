//
//  AddonMaker.hpp
//  Addon Maker on Command Line
//
//  Created by YotioSoft on 2020/02/13.
//  Copyright © 2020 YotioSoft. All rights reserved.
//

#ifndef AddonMaker_hpp
#define AddonMaker_hpp

#include "Header.hpp"
#include "Addon.hpp"
#include "PulldownMenu.hpp"
#include "Button.hpp"

class AddonMaker : public Addon {
public:
	string getVersion() {
		return "1.0";
	}
	
	void setAddonNameEN(String arg) {
		addon_name = arg;
		
		if (arg.size() > 0) {
			check_addon_name_en = true;
		}
	}
	
	bool isAddonNameENSet() {
		return check_addon_name_en;
	}
	
	String printAddonNameEN() {
		return addon_name;
	}
	
	void setAddonNameJP(String arg) {
		addon_jp_name = arg;
		
		if (arg.size() > 0) {
			check_addon_name_jp = true;
		}
	}
	
	bool isAddonNameJPSet() {
		return check_addon_name_jp;
	}
	
	String printAddonNameJP() {
		return addon_jp_name;
	}
	
	void setAuthor(String arg) {
		addon_author = arg;
		
		if (arg.size() > 0) {
			check_author = true;
		}
	}
	
	bool isAuthorSet() {
		return check_author;
	}
	
	String printAuthor() {
		return addon_author;
	}
	
	void setSummary(String arg) {
		addon_summary = arg;
		
		if (arg.size() > 0) {
			check_summary = true;
		}
	}
	
	bool isSummarySet() {
		return check_summary;
	}
	
	String printSummary() {
		return addon_summary;
	}
	
	void setAddonSet(String arg) {
		belong_addons_set_name.push_back(arg);
		
		if (arg.size() > 0) {
			check_addon_set = true;
		}
	}
	
	bool isAddonSetSet() {
		return check_addon_set;
	}
	
	Array<String> printAddonSet() {
		return belong_addons_set_name;
	}
	
	void setCategory(String arg) {
		addon_categories.push_back(arg);
		
		if (arg.size() > 0) {
			check_category = true;
		}
	}
	
	bool isCategorySet() {
		return check_category;
	}
	
	Array<String> printCategories() {
		return addon_categories;
	}
	
	void setIcon(String arg) {
		addon_icon = arg;
		
		if (arg.size() > 0) {
			check_icon = true;
		}
	}
	
	bool isIconSet() {
		return check_icon;
	}
	
	String printIcon() {
		return addon_icon;
	}
	
	void setMaximumCapacity(int arg) {
		maximum_capacity = arg;
		
		check_maximum_capacity = true;
	}
	
	bool isMaximumCapacitySet() {
		return check_maximum_capacity;
	}
	
	int printMaximumCapacity() {
		return maximum_capacity;
	}
	
	void setEffect(String name, int influence, int grid) {
		effects[name].influence = influence;
		effects[name].grid = grid;
		
		if (name.size() > 0) {
			check_effect = true;
		}
	}
	
	bool isEffectSet() {
		return check_effect;
	}
	
	map<String, EffectStruct> printEffect() {
		return effects;
	}
	
	void setType(String type_name, String image, String night_mask, RGBstruct transparent_color) {
		types[type_name].image = image;
		types[type_name].night_mask = night_mask;
		types[type_name].transparent_color = transparent_color;
		
		check_type = true;
	}
	
	void setDirection(String type_name, String direction_name, int size_width, int size_height, int chip_x, int chip_y, int top_left_x, int top_left_y, int bottom_right_x, int bottom_right_y) {
		types[type_name].direction_names.push_back(direction_name);
		types[type_name].directions[direction_name].size_width = size_width;
		types[type_name].directions[direction_name].size_height = size_height;
		types[type_name].directions[direction_name].chip_x = chip_x;
		types[type_name].directions[direction_name].chip_y = chip_y;
		types[type_name].directions[direction_name].top_left_x = top_left_x;
		types[type_name].directions[direction_name].top_left_y = top_left_y;
		types[type_name].directions[direction_name].bottom_right_x = bottom_right_x;
		types[type_name].directions[direction_name].bottom_right_y = bottom_right_y;
	}
	
	bool isTypeSet() {
		return check_type;
	}
	
	map<String, AddonTypeStruct> printTypes() {
		return types;
	}
	
	void menu(Font& font16);
	
protected:
	bool check_addon_name_en = false;
	bool check_addon_name_jp = false;
	bool check_author = false;
	bool check_summary = false;
	bool check_addon_set = false;
	bool check_category = false;
	bool check_icon = false;
	bool check_maximum_capacity = false;
	bool check_effect = false;
	bool check_type = false;
};

#endif /* AddonMaker_hpp */
