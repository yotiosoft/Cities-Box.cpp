//
//  TitleMenu.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/12/01.
//

#include "TitleMenu.hpp"

pair<bool, GeneralSetting> titleMenu(ImagesStruct& images, Font& font16, String& filePath, bool& createMap, String& cityName, Size& mapSize) {
	Scene::SetBackground(Color(50, 50, 50));
	
	ImageStruct* logo = &images.images["title_menu"]["logo"];
	
	// ボタンの宣言
	Button load_map_button(IconFont::Floppy, 50, 50, PositionStruct{3, 3}, U"読み込み", font16);			// 読み込み
	Button new_map_button(IconFont::Plus, 50, 50, PositionStruct{3, 3}, U"新しいマップ", font16);			// 新しいマップ
	Button new_addon_button(IconFont::Compass, 50, 50, PositionStruct{3, 3}, U"アドオンを作成する", font16);	// 新しいアドオン
	Button setting_button(IconFont::Setting, 32, 32, PositionStruct{0, 0}, U"設定", font16);				// 設定画面へ
	Button developer_screen_button(IconFont::User, 32, 32, PositionStruct{0, 0}, U"開発者画面", font16);				// 開発者画面へ
	
	// 設定画面の宣言
	GeneralSetting general_setting;
	
	Color color_white = Color(Palette::White);

	// カーソル変更用の変数
	// ライブラリ側のWindows版でウィンドウのリサイズ後にカーソルがもとに戻らないバグに対する応急措置
	Vec2 before_cursor_pos = Cursor::Pos();
	bool changed_cursor_style = true;
	
	while (System::Update()) {
		// ウィンドウ内にカーソルが戻ったときに一度隠したカーソルをもとに戻す（Windowsのみ）
		if (OS == "Windows" && changed_cursor_style) {
			specific::changeCursor();
		}

		logo->texture.draw(Scene::Width()/2-logo->texture.width()/2, Scene::Height()/2-logo->texture.height()*1.75);
		
		UnitaryTools::cMes(font16, U"ver.{} (r{})"_fmt(VERSION, RELEASE_NUMBER), PositionStruct{0, Scene::Height()/2-(int)(logo->texture.height()*1.75)+150}, Size{Scene::Width(), 20}, color_white);
		
		load_map_button.put(PositionStruct{Scene::Width()/2-75-50, Scene::Height()*3/5});
		new_map_button.put(PositionStruct{Scene::Width()/2-25, Scene::Height()*3/5});
		new_addon_button.put(PositionStruct{Scene::Width()/2+75, Scene::Height()*3/5});
		
		// 開発者モードの場合、配置を変更
		if (general_setting.isDeveloperMode()) {
			setting_button.put(PositionStruct{Scene::Width()/2-16-32*1, Scene::Height()*3/4});
			developer_screen_button.put(PositionStruct{Scene::Width()/2-16+32*1, Scene::Height()*3/4});
		}
		else {
			setting_button.put(PositionStruct{Scene::Width()/2-16, Scene::Height()*3/4});
		}
		
		if (load_map_button.push()) {
			// ファイル選択ダイアログ
			String filePathTemp;
			if (specific::openSaveData(filePathTemp)) {
				filePath = filePathTemp;
				
				System::Update();
				loadingScreen(font16);
				return pair<bool, GeneralSetting>(true, general_setting);
			}
			/*
			if (OS == "Linux" || const auto open = Dialog::OpenFile(ff, specific::SaveDataFilePath)) {
				if (OS == "Linux") {
					filePath = U"./data/maps/Sample_City.cbd.cbj";
				}
				else {
					//filePath = open.value();
				}
				
				b = System::Update();
				loadingScreen(font16);
				return pair<bool, GeneralSetting>(true, general_setting);
			}*/
		}
		if (new_map_button.push()) {
			new_map_button.release();
			TextEditState nameInput;
			TextEditState widthInput;
			TextEditState heightInput;
			nameInput.text = U"新しい都市";
			widthInput.text = U"100";
			heightInput.text = U"100";
			String error;
			while (System::Update()) {
				const int left = Scene::Width() / 2 - 220;
				font16(U"新しいマップ").draw(left, 80, Palette::White);
				font16(U"都市名").draw(left, 140, Palette::White);
				SimpleGUI::TextBox(nameInput, Vec2(left, 170), 440);
				font16(U"幅（1～500）").draw(left, 230, Palette::White);
				SimpleGUI::TextBox(widthInput, Vec2(left, 260), 200);
				font16(U"高さ（1～500）").draw(left + 240, 230, Palette::White);
				SimpleGUI::TextBox(heightInput, Vec2(left + 240, 260), 200);
				font16(error).draw(left, 315, Palette::Orange);
				if (SimpleGUI::Button(U"作成", Vec2(left, 370), 200)) {
					const auto width = ParseOpt<int>(widthInput.text);
					const auto height = ParseOpt<int>(heightInput.text);
					if (nameInput.text.isEmpty() || !width || !height || *width < 1 || *width > 500 || *height < 1 || *height > 500) {
						error = U"都市名と1～500のサイズを入力してください。";
						continue;
					}
					Array<FileFilter> filters = { { U"セーブデータ", { U"cbj" } } };
					if (const auto selected = Dialog::SaveFile(filters, specific::SaveDataFilePath)) {
						filePath = selected.value();
						cityName = nameInput.text;
						mapSize = Size(*width, *height);
						createMap = true;
						return pair<bool, GeneralSetting>(true, general_setting);
					}
				}
				if (SimpleGUI::Button(U"戻る", Vec2(left + 240, 370), 200)) break;
			}
		}
		if (new_addon_button.push()) {
			new_addon_button.release();
			AddonMaker addonMaker;
			addonMaker.sMenu(font16);
		}
		if (setting_button.push()) {
			setting_button.release();
			general_setting.screen(font16);
		}
		if (developer_screen_button.push()) {
			developerScreen();
		}
		
		System::Sleep(50);

		// ウィンドウ内にカーソルが戻ったときに矢印カーソルに戻すために一度カーソルを隠す（Windowsのみ）
		if (OS == "Windows") {
			changed_cursor_style = specific::isCursorEntered(before_cursor_pos);
		}
	}
	
	return pair<bool, GeneralSetting>(false, general_setting);
}

void developerScreen() {
	// カーソル変更用の変数
	// ライブラリ側のWindows版でウィンドウのリサイズ後にカーソルがもとに戻らないバグに対する応急措置
	Vec2 before_cursor_pos = Cursor::Pos();
	bool changed_cursor_style = true;

	while (System::Update()) {
		// ウィンドウ内にカーソルが戻ったときに一度隠したカーソルをもとに戻す（Windowsのみ）
		if (OS == "Windows" && changed_cursor_style) {
			specific::changeCursor();
		}

		// ウィンドウ内にカーソルが戻ったときに矢印カーソルに戻すために一度カーソルを隠す（Windowsのみ）
		if (OS == "Windows") {
			changed_cursor_style = specific::isCursorEntered(before_cursor_pos);
		}
	}
}

bool loadingScreen(Font& font16) {
	font16(U"Loading...").draw(Scene::Width()/2-35, Scene::Height()/2-8, Color(Palette::White));
	return System::Update();
}
