//
//  CitiesBox.cpp
//  Cities-Box
//
//	main関数で実行する内容
//
//  Created by YotioSoft on 2021/03/23.
//

#include "CitiesBox.hpp"

void CitiesBox() {
	Window::SetTitle(TITLE);
	specific::moveToCurrentDir();
	
	UnitaryTools::debugInit();
	
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Virtual);
	
	Scene::SetBackground(Color(50, 50, 50));
	
	/*---------------------------------------------
					ファイル読み込み
	 --------------------------------------------*/
	// 画像の読み込み
	ImagesStruct images;
	loadImages(images);
	
	// サウンドファイルの読み込み
	Sound bgm;
	bgm.searchSoundFiles("./sound/BGM");
	
	// フォントの宣言
	Font font16(16, U"{}/NotoSansCJKjp/NotoSansCJKjp-Bold.otf"_fmt(specific::getFontsDir()));
	Font font12(12, U"{}/NotoSansCJKjp/NotoSansCJKjp-Regular.otf"_fmt(specific::getFontsDir()));
	Font font8(8, U"{}/NotoSansCJKjp/NotoSansCJKjp-Regular.otf"_fmt(specific::getFontsDir()));
	
	// タイトルメニュー画面
	String mapFilePath;
	bool createMap = false;
	String cityName;
	Size newMapSize{ 100, 100 };
	pair<bool, GeneralSetting> title_menu_ret = titleMenu(images, font16, mapFilePath, createMap, cityName, newMapSize);
	if (!title_menu_ret.first) {
		return;				// タイトル画面でウィンドウを閉じたらプログラム終了
	}
	GeneralSetting general_setting = title_menu_ret.second;
	Size before_window_size = Scene::Size();
	
	// マップとアドオンの読み込み
	CityMap map;
	if (!(createMap ? map.create(mapFilePath, cityName, newMapSize) : map.load(mapFilePath))) {
		UnitaryTools::showErrorMessage(U"マップの読み込みに失敗しました。", map.getLastLoadError());
		return;
	}
	
	// カメラの初期位置
	CameraStruct camera;
	camera.position = PositionStruct{-Scene::Width()/2+64/2, -150};
	
	// カーソルの位置
	CursorStruct cursor;
	cursor.texture = &(images.images["pointer"]["blue"].texture);
	CursorStruct cursor_before = cursor;
	
	// 描画処理
	RenderTexture bufferTexture(Scene::Width(), Scene::Height(), Color(30, 30, 30));
	bool updateMap = true, firstLoop = true, window_size_changed;
	
	// サブウィンドウ
	//SubWindow subWindow(U"Test Window", &font16, Size(400, 200), Color(Palette::White));
	//SubWindow subWindow2(U"Test Window2", &font16, Size(300, 150), Color(Palette::White));
	
	// メニュー
	Menu menu;
	menu.set(PositionStruct{0, Scene::Height()-50}, Size(Scene::Width(), 50), &map, &font8, &font12, &font16);
    const int default_menu_size = Scene::Width();
	
	// 選択されたアドオン
	CBAddon* selectedAddon;
	
	// 前回左クリックしたときのマップ上の座標
	CoordinateStruct beforeMousePressedCoordinate = {-1, -1};
	bool pressing = false;
	bool buildErrorShown = false;
	CityMap::BuildingOverview buildingOverview;
	bool showBuildingOverview = false;
	bool editingBuildingName = false;
	TextEditState buildingNameInput;
	constexpr int overviewWidth = 280;
	constexpr int overviewHeight = 200;
	Vec2 overviewPosition{ 10, 65 };
	CoordinateStruct overviewCoordinate{ -1, -1 };
	Vec2 overviewTileOffset{ 32, 16 };
	
	// Rust側の都市状態（表示用スナップショット）
	SimulationSnapshot simulation = map.getSimulationSnapshot();
	TimeStruct time = simulation.time;
	
	// Details Barの設定
	DetailsBar detailsBar(PositionStruct{Scene::Size().x-450, 10}, &font16);
	
	// BGMの再生（ランダム）
	bgm.playBGM();
	
	// 更新前のLayerType
	LayerType::Type before_layer_type = UnitaryTools::timeToLayerType(time);

	// カーソル変更用の変数
	// ライブラリ側のWindows版でウィンドウのリサイズ後にカーソルがもとに戻らないバグに対する応急措置
	Vec2 before_cursor_pos = Cursor::Pos();
	bool changed_cursor_style = true;
	
	while (System::Update()) {
		window_size_changed = false;

		// ウィンドウサイズが変更されたら再描画
		if (before_window_size != Scene::Size()) {
			Size new_size = Scene::Size();

			// テクスチャをリサイズ
			bufferTexture = RenderTexture(new_size.x, new_size.y, Color(30, 30, 30));

			// メニューを再設置
			menu.set(PositionStruct{ 0, new_size.y - 50 }, Size(default_menu_size, 50), &map, &font8, &font12, &font16);

			// Details Barの位置を変更
			detailsBar.setPos(PositionStruct{ new_size.x - 450, 10 });

			before_window_size = new_size;
			updateMap = true;
			window_size_changed = true;
		}

		// カメラの操作
		if (KeyLeft.pressed()) {
			camera.position.x -= 20;
			updateMap = true;
		}
		if (KeyRight.pressed()) {
			camera.position.x += 20;
			updateMap = true;
		}
		if (KeyUp.pressed()) {
			camera.position.y -= 10;
			updateMap = true;
		}
		if (KeyDown.pressed()) {
			camera.position.y += 10;
			updateMap = true;
		}
		
		// カーソルの位置を取得
		if (updateMap || Cursor::Delta().x != 0 || Cursor::Delta().y != 0 || MouseL.pressed() != cursor.pressed) {
			cursor_before = cursor;
			
			cursor.position = PositionStruct{Cursor::Pos().x, Cursor::Pos().y};
			cursor.coordinate = map.positionToCoordinate(cursor.position, camera);
			cursor.position_per_tiles = map.coordinateToPosition(cursor.coordinate, camera);
			cursor.pressed = MouseL.pressed();
			
			updateMap = true;
		}
		
		// マップなどを更新する必要がある場合はバッファに描画（更新）する
		if (updateMap) {
			bufferTexture.clear(Color(30, 30, 30));
			
			// マップの描画
			ScopedRenderTarget2D target(bufferTexture);
			map.draw(camera, cursor, window_size_changed);
			
			if (firstLoop) {
				firstLoop = false;
			}
			else {
				updateMap = false;
			}
		}
		
		// 更新すべき時刻になったら更新を有効にする
		LayerType::Type layer_type = UnitaryTools::timeToLayerType(time);
		if (layer_type != before_layer_type) {
			updateMap = true;
			before_layer_type = layer_type;
		}
		
		// バッファを描画
		bufferTexture.draw(0, 0);
		
		// 時間を進ませる
		// 日付が変わる時、需要度も更新する
		simulation = map.updateWorld(1);
		time = simulation.time;
		menu.update(simulation);
		
		// Details Barの表示
		detailsBar.printWeather();
		detailsBar.printTemperature(simulation.temperature);
		detailsBar.printTime(time);
		detailsBar.printPopulation(simulation.population);
		detailsBar.printMoney(simulation.money);
		
		// メニュー及びアドオン選択メニューの表示
		// アドオンが選択されたら、選択されたアドオンのポインタを返す
		selectedAddon = menu.draw(updateMap, simulation);
		menu.addonMenu();
		if (MouseL.down() && selectedAddon == nullptr && cursor.position.y < Scene::Height() - 50) {
			const Rect panel(static_cast<int>(overviewPosition.x), static_cast<int>(overviewPosition.y), overviewWidth, overviewHeight);
			if (!showBuildingOverview || !panel.mouseOver()) {
				showBuildingOverview = map.getBuildingOverview(cursor.coordinate, buildingOverview);
				editingBuildingName = false;
				if (showBuildingOverview) {
					overviewCoordinate = cursor.coordinate;
					overviewTileOffset = Vec2{ cursor.position.x - cursor.position_per_tiles.x, cursor.position.y - cursor.position_per_tiles.y };
				}
			}
		}
		
		//font16(U"{:04d}"_fmt(time.year)+U"/"+U"{:02d}"_fmt(time.month)+U"/"+U"{:02d}"_fmt(time.date)+U" "+U"{:02d}"_fmt(time.hour)+U":"+U"{:02d}"_fmt(time.minutes)).draw(230, Scene::Height()-25-3);
		
		// マップ上でクリックされたらアドオンを設置
		if (selectedAddon != nullptr && MouseL.pressed() && cursor.position.y <= Scene::Height()-60-80) {
			// 除去モードなら除去
			if (selectedAddon->isInCategories(CategoryID::Tile)) {
				map.breaking(cursor.coordinate, false, true, true);
			}

			// 設置モードなら設置
			if (cursor.coordinate.x != beforeMousePressedCoordinate.x || cursor.coordinate.y != beforeMousePressedCoordinate.y) {
				if (!map.build(cursor, cursor_before, selectedAddon, true) && !buildErrorShown && !map.getLastBuildError().isEmpty()) {
					UnitaryTools::showErrorMessage(U"建設できません。", map.getLastBuildError());
					buildErrorShown = true;
				}
				beforeMousePressedCoordinate = cursor.coordinate;
				updateMap = true;
				pressing = true;
			}
		}
		if (pressing) {
			if (!MouseL.pressed()) {
				beforeMousePressedCoordinate = {-1, -1};
				pressing = false;
				buildErrorShown = false;
			}
		}

		if (showBuildingOverview) {
			const PositionStruct tilePosition = map.coordinateToPosition(overviewCoordinate, camera);
			const Vec2 anchor{ tilePosition.x + overviewTileOffset.x, tilePosition.y + overviewTileOffset.y };
			overviewPosition = Vec2{
				Clamp(anchor.x - overviewWidth / 2, 8.0, static_cast<double>(Max(8, Scene::Width() - overviewWidth - 8))),
				Clamp(anchor.y >= overviewHeight + 112 ? anchor.y - overviewHeight - 42 : anchor.y + 42,
					65.0, static_cast<double>(Max(65, Scene::Height() - overviewHeight - 60)))
			};
			const int x = static_cast<int>(overviewPosition.x);
			const int y = static_cast<int>(overviewPosition.y);
			const Vec2 lineStart{
				Clamp(anchor.x, static_cast<double>(x + 10), static_cast<double>(x + overviewWidth - 10)),
				anchor.y >= y + overviewHeight / 2 ? static_cast<double>(y + overviewHeight) : static_cast<double>(y)
			};
			Line(lineStart, anchor).draw(5, Color(15, 28, 35));
			Line(lineStart, anchor).draw(2, Color(109, 229, 222));
			Circle(anchor, 8).draw(Color(15, 28, 35));
			Circle(anchor, 6).draw(Palette::White);
			Circle(anchor, 3).draw(Color(71, 184, 184));
			Rect(x, y, overviewWidth, overviewHeight).draw(Color(26, 35, 44, 245));
			Rect(x, y, overviewWidth, 3).draw(Color(71, 184, 184));
			font12(buildingOverview.name).draw(x + 10, y + 11, Palette::White);
			if (SimpleGUI::Button(U"×", Vec2(x + overviewWidth - 36, y + 7), 28)) {
				showBuildingOverview = false;
				editingBuildingName = false;
			}
			font12(U"種類: {}"_fmt(buildingOverview.type)).draw(x + 10, y + 40, Palette::White);
			font12(U"人口: {} 人"_fmt(buildingOverview.population)).draw(x + 10, y + 60, Palette::White);
			const auto& effects = buildingOverview.effects;
			auto effectText = [&](RateID::Type rate, const String& label, int row) {
				const auto it = effects.find(rate);
				const int value = (it == effects.end()) ? 0 : it->second.influence;
				font12(U"{}: {:+d}"_fmt(label, value)).draw(x + 10, y + 84 + row * 18, Palette::White);
			};
			effectText(RateID::CrimeRate, U"治安への影響", 0);
			effectText(RateID::LandPrice, U"地価への影響", 1);
			effectText(RateID::EducationRate, U"教育への影響", 2);
			effectText(RateID::NoiseRate, U"騒音への影響", 3);
			if (editingBuildingName) {
				SimpleGUI::TextBox(buildingNameInput, Vec2(x + 10, y + 156), 178);
				if (SimpleGUI::Button(U"保存", Vec2(x + 194, y + 156), 76) && map.renameBuilding(buildingOverview.objectID, buildingNameInput.text)) {
					buildingOverview.name = buildingNameInput.text;
					editingBuildingName = false;
				}
			}
			else if (SimpleGUI::Button(U"建物名を編集", Vec2(x + 10, y + 156), overviewWidth - 20)) {
				buildingNameInput.text = buildingOverview.name;
				editingBuildingName = true;
			}
		}
		
		// BGMの再生中の処理
		bgm.playingBGM();
		
		System::Sleep(20);
	}
	
	map.freeMapAndAddons();
	
	return;
}
