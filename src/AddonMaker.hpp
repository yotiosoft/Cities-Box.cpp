//
//  AddonMaker.hpp
//  Addon Maker on Command Line
//
//  Created by YotioSoft on 2020/02/13.
//  Copyright c 2020 YotioSoft. All rights reserved.
//

#ifndef AddonMaker_hpp
#define AddonMaker_hpp

#include <Specific.hpp>
#include "Addon.hpp"
#include "PulldownMenu.hpp"
#include "Button.hpp"

class AddonMaker : public Addon {
public:
	string getVersion() {
		return "1.0";
	}
	
	void setAddonNameEN(String arg) {
		m_addon_name = arg;
		
		if (arg.size() > 0) {
			m_am_check_addon_name_en = true;
		}
	}
	
	bool isAddonNameENSet() {
		return m_am_check_addon_name_en;
	}
	
	String printAddonNameEN() {
		return m_addon_name;
	}
	
	void setAddonNameJP(String arg) {
		m_addon_jp_name = arg;
		
		if (arg.size() > 0) {
			m_am_check_addon_name_jp = true;
		}
	}
	
	bool isAddonNameJPSet() {
		return m_am_check_addon_name_jp;
	}
	
	String printAddonNameJP() {
		return m_addon_jp_name;
	}
	
	void setAuthor(String arg) {
		m_addon_author = arg;
		
		if (arg.size() > 0) {
			m_am_check_author = true;
		}
	}
	
	bool isAuthorSet() {
		return m_am_check_author;
	}
	
	String printAuthor() {
		return m_addon_author;
	}
	
	void setSummary(String arg) {
		m_addon_summary = arg;
		
		if (arg.size() > 0) {
			m_am_check_summary = true;
		}
	}
	
	bool isSummarySet() {
		return m_am_check_summary;
	}
	
	String printSummary() {
		return m_addon_summary;
	}
	
	void setAddonSet(String arg) {
		m_belong_addons_set_name.push_back(arg);
		
		if (arg.size() > 0) {
			m_am_check_addon_set = true;
		}
	}
	
	bool isAddonSetSet() {
		return m_am_check_addon_set;
	}
	
	Array<String> printAddonSet() {
		return m_belong_addons_set_name;
	}
	
	void setCategory(CategoryID::Type arg) {
		m_addon_categories.push_back(arg);
		
		m_am_check_category = true;
	}
	
	bool isCategorySet() {
		return m_am_check_category;
	}
	
	Array<CategoryID::Type> printCategories() {
		return m_addon_categories;
	}
	
	void setIcon(String arg) {
		m_addon_icon = arg;
		
		if (arg.size() > 0) {
			m_am_check_icon = true;
		}
	}
	
	bool isIconSet() {
		return m_am_check_icon;
	}
	
	String printIcon() {
		return m_addon_icon;
	}
	
	void setMaximumCapacity(int arg) {
		m_maximum_capacity = arg;
		
		m_am_check_maximum_capacity = true;
	}
	
	bool isMaximumCapacitySet() {
		return m_am_check_maximum_capacity;
	}
	
	int printMaximumCapacity() {
		return m_maximum_capacity;
	}
	
	void setEffect(RateID::Type rate_id, int influence, int grid) {
		m_effects[rate_id].influence = influence;
		m_effects[rate_id].grid = grid;
		
		if (rate_id == RateID::None) {
			m_am_check_effect = true;
		}
	}
	
	bool isEffectSet() {
		return m_am_check_effect;
	}
	
	map<RateID::Type, EffectStruct> printEffect() {
		return m_effects;
	}
	/*
	void setType(String typeName, String image, String nightMask, RGBstruct transparentColor) {
		m_types[typeName].image = image;
		m_types[typeName].nightMask = nightMask;
		m_types[typeName].transparentColor = transparentColor;
		
		m_am_check_type = true;
	}
	
	void setDirection(String typeName, String directionName, int sizeWidth, int sizeHeight, int chipX, int chipY, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY) {
		m_types[typeName].directionNames.push_back(directionName);
		m_types[typeName].directions[directionName].size.x = sizeWidth;
		m_types[typeName].directions[directionName].size.y = sizeHeight;
		m_types[typeName].directions[directionName].requiredTiles.x = chipX;
		m_types[typeName].directions[directionName].requiredTiles.y = chipY;
		m_types[typeName].directions[directionName].topLeft.x = topLeftX;
		m_types[typeName].directions[directionName].topLeft.y = topLeftY;
		m_types[typeName].directions[directionName].bottomRight.x = bottomRightX;
		m_types[typeName].directions[directionName].bottomRight.y = bottomRightY;
	}
	*/
	bool isTypeSet() {
		return m_am_check_type;
	}
	/*
	map<String, AddonType> printTypes() {
		return m_types;
	}
	*/
	void sMenu(Font& font16);
	void sName(Font& font16);
	
protected:
	bool m_am_check_addon_name_en = false;
	bool m_am_check_addon_name_jp = false;
	bool m_am_check_author = false;
	bool m_am_check_summary = false;
	bool m_am_check_addon_set = false;
	bool m_am_check_category = false;
	bool m_am_check_icon = false;
	bool m_am_check_maximum_capacity = false;
	bool m_am_check_effect = false;
	bool m_am_check_type = false;
};

#endif /* AddonMaker_hpp */
