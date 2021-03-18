//
//  TitleMenu.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/12/01.
//

#include "TitleMenu.hpp"

bool titleMenu(ImagesStruct& images, Font& font16, String& filePath) {
	Scene::SetBackground(Color(50, 50, 50));
	
	ImageStruct* logo = &images.images["title_menu"]["logo"];
	
	// ボタンの宣言
	Button load_map_button(IconFont::Floppy, 50, 50, PositionStruct{3, 3}, U"読み込み", font16);			// 読み込み
	Button new_map_button(IconFont::Plus, 50, 50, PositionStruct{3, 3}, U"新しいマップ", font16);			// 新しいマップ
	Button new_addon_button(IconFont::Compass, 50, 50, PositionStruct{3, 3}, U"アドオンを作成する", font16);	// 新しいアドオン
	Button setting_button(IconFont::Setting, 32, 32, PositionStruct{0, 0}, U"設定", font16);				// 設定画面へ
	
	bool b;
	Color color_white = Color(Palette::White);
	
	while (System::Update()) {
		logo->texture.draw(Scene::Width()/2-logo->texture.width()/2, Scene::Height()/2-logo->texture.height()*1.75);
		
		//font16(U"ver.2.0 alpha").draw(Scene::Width()/2-)
		cMes(font16, U"ver.2.0 alpha", PositionStruct{0, Scene::Height()/2-(int)(logo->texture.height()*1.75)+150}, Size{Scene::Width(), 20}, color_white);
		
		load_map_button.put(PositionStruct{Scene::Width()/2-75-50, Scene::Height()*3/5});
		new_map_button.put(PositionStruct{Scene::Width()/2-25, Scene::Height()*3/5});
		new_addon_button.put(PositionStruct{Scene::Width()/2+75, Scene::Height()*3/5});
		setting_button.put(PositionStruct{Scene::Width()/2-16, Scene::Height()*3/4});
		
		if (load_map_button.push()) {
			// ファイル選択ダイアログ
			Array<FileFilter> ff = {{U"セーブデータ", {U"cbd", U"cbj"}}};
			String filePathTemp;
			if (const auto open = Dialog::OpenFile(ff, U"./data/maps/")) {
				filePath = open.value();
				
				b = System::Update();
				loadingScreen(font16);
				return true;
			}
		}
		if (new_map_button.push()) {
			return false;
		}
		if (new_addon_button.push()) {
			new_addon_button.release();
			AddonMaker addonMaker;
			addonMaker.sMenu(font16);
		}
		if (setting_button.push()) {
			setting_button.release();
			settingMenu(font16);
		}
		
		System::Sleep(50);
	}
	
	return false;
}

void settingMenu(Font& font16) {
	Button returnButton(IconFont::LeftWithCircle, 30, 30, PositionStruct{0, 0});
	
	while (System::Update()) {
		Rect(0, 0, Scene::Size().x/5, Scene::Size().y).draw(Color(40, 40, 40));
		font16(U"設定").draw(60, 20, Palette::White);
		
		// メニューボタンの設置
		returnButton.put(PositionStruct{20, 20});				// 戻る
		font16(U"👤開発者設定").draw(20, 80, Palette::White);		// 開発者設定
		
		// 戻るボタンの処理
		if (returnButton.push()) {
			returnButton.release();
			return;
		}
	}
}

bool loadingScreen(Font& font16) {
	font16(U"Loading...").draw(Scene::Width()/2-35, Scene::Height()/2-8, Color(Palette::White));
	return System::Update();
}
