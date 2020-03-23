//
//  AddonMaker.hpp
//  Addon Maker on Command Line
//
//  Created by YotioSoft on 2020/02/13.
//  Copyright c 2020 YotioSoft. All rights reserved.
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
		addonName = arg;
		
		if (arg.size() > 0) {
			checkAddonNameEN = true;
		}
	}
	
	bool isAddonNameENSet() {
		return checkAddonNameEN;
	}
	
	String printAddonNameEN() {
		return addonName;
	}
	
	void setAddonNameJP(String arg) {
		addonJPName = arg;
		
		if (arg.size() > 0) {
			checkAddonNameJP = true;
		}
	}
	
	bool isAddonNameJPSet() {
		return checkAddonNameJP;
	}
	
	String printAddonNameJP() {
		return addonJPName;
	}
	
	void setAuthor(String arg) {
		addonAuthor = arg;
		
		if (arg.size() > 0) {
			checkAuthor = true;
		}
	}
	
	bool isAuthorSet() {
		return checkAuthor;
	}
	
	String printAuthor() {
		return addonAuthor;
	}
	
	void setSummary(String arg) {
		addonSummary = arg;
		
		if (arg.size() > 0) {
			checkSummary = true;
		}
	}
	
	bool isSummarySet() {
		return checkSummary;
	}
	
	String printSummary() {
		return addonSummary;
	}
	
	void setAddonSet(String arg) {
		belongAddonsSetName.push_back(arg);
		
		if (arg.size() > 0) {
			checkAddonSet = true;
		}
	}
	
	bool isAddonSetSet() {
		return checkAddonSet;
	}
	
	Array<String> printAddonSet() {
		return belongAddonsSetName;
	}
	
	void setCategory(String arg) {
		addonCategories.push_back(arg);
		
		if (arg.size() > 0) {
			checkCategory = true;
		}
	}
	
	bool isCategorySet() {
		return checkCategory;
	}
	
	Array<String> printCategories() {
		return addonCategories;
	}
	
	void setIcon(String arg) {
		addonIcon = arg;
		
		if (arg.size() > 0) {
			checkIcon = true;
		}
	}
	
	bool isIconSet() {
		return checkIcon;
	}
	
	String printIcon() {
		return addonIcon;
	}
	
	void setMaximumCapacity(int arg) {
		maximumCapacity = arg;
		
		checkMaximumCapacity = true;
	}
	
	bool isMaximumCapacitySet() {
		return checkMaximumCapacity;
	}
	
	int printMaximumCapacity() {
		return maximumCapacity;
	}
	
	void setEffect(String name, int influence, int grid) {
		effects[name].influence = influence;
		effects[name].grid = grid;
		
		if (name.size() > 0) {
			checkEffect = true;
		}
	}
	
	bool isEffectSet() {
		return checkEffect;
	}
	
	map<String, EffectStruct> printEffect() {
		return effects;
	}
	
	void setType(String typeName, String image, String nightMask, RGBstruct transparentColor) {
		types[typeName].image = image;
		types[typeName].nightMask = nightMask;
		types[typeName].transparentColor = transparentColor;
		
		checkType = true;
	}
	
	void setDirection(String typeName, String directionName, int sizeWidth, int sizeHeight, int chipX, int chipY, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY) {
		types[typeName].directionNames.push_back(directionName);
		types[typeName].directions[directionName].sizeWidth = sizeWidth;
		types[typeName].directions[directionName].sizeHeight = sizeHeight;
		types[typeName].directions[directionName].chipX = chipX;
		types[typeName].directions[directionName].chipY = chipY;
		types[typeName].directions[directionName].topLeftX = topLeftX;
		types[typeName].directions[directionName].topLeftY = topLeftY;
		types[typeName].directions[directionName].bottomRightX = bottomRightX;
		types[typeName].directions[directionName].bottomRightY = bottomRightY;
	}
	
	bool isTypeSet() {
		return checkType;
	}
	
	map<String, AddonTypeStruct> printTypes() {
		return types;
	}
	
	void menu(Font& font16);
	
protected:
	bool checkAddonNameEN = false;
	bool checkAddonNameJP = false;
	bool checkAuthor = false;
	bool checkSummary = false;
	bool checkAddonSet = false;
	bool checkCategory = false;
	bool checkIcon = false;
	bool checkMaximumCapacity = false;
	bool checkEffect = false;
	bool checkType = false;
};

#endif /* AddonMaker_hpp */
