//
//  TitleMenu.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/12/01.
//

#include "TitleMenu.hpp"

void titleMenu(Images& images) {
	Scene::SetBackground(Color(50, 50, 50));
	
	ImageStruct logo = images.images["title_menu"]["logo"];
	
	// ボタンの宣言
	Button load_button(IconFont::Floppy, 50);			// 読み込み
	Button new_map_button(IconFont::Plus, 50);			// 新しいマップ
	
	while (System::Update()) {
		logo.texture.draw(Scene::Width()/2-logo.texture.width()/2, Scene::Height()/2-logo.texture.height()*1.75);
		
		load_button.put(PositionStruct{10, 10});
		new_map_button.put(PositionStruct{110, 10});
		
		if (load_button.push()) {
			break;
		}
		if (new_map_button.push()) {
			break;
		}
		
		specific::sleep(50);
	}
}
