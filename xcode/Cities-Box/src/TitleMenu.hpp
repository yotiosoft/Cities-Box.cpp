//
//  TitleMenu.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/12/01.
//

#ifndef TitleMenu_hpp
#define TitleMenu_hpp

#include "Specific.hpp"
#include "ImagesStruct.hpp"
#include "Button.hpp"
#include "AddonMaker.hpp"
#include "GeneralSetting.hpp"

pair<bool, GeneralSetting> titleMenu(ImagesStruct& images, Font& font16, String& filePath);

void developerScreen();

bool loadingScreen(Font& font16);

#endif /* TitleMenu_hpp */
