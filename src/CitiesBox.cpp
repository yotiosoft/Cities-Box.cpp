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
	pair<bool, GeneralSetting> title_menu_ret = titleMenu(images, font16, mapFilePath);
	if (!title_menu_ret.first) {
		return;				// タイトル画面でウィンドウを閉じたらプログラム終了
	}
	GeneralSetting general_setting = title_menu_ret.second;
	Size before_window_size = Scene::Size();
	
	// マップとアドオンの読み込み
	CityMap map;
	map.load(mapFilePath);
	
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
	CoordinateStruct beforeMousePressedCoordinate = {0, 0};
	bool pressing = false;
	
	// 時間
	TimeStruct time{0, 0, 0, 0, 0};
	
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
		
		
		//cout << "cursor: " << cursor.coordinate.x << "," << cursor.coordinate.y << " : " << map.getAddon(cursor.coordinate)[0].getName() << endl;
		
		// マップなどを更新する必要がある場合はバッファに描画（更新）する
		if (updateMap) {
			bufferTexture.clear(Color(30, 30, 30));
			
			// マップの描画
			ScopedRenderTarget2D target(bufferTexture);
			map.draw(camera, cursor, window_size_changed);
			
			menu.update();
			
			if (firstLoop) {
				firstLoop = false;
				
				// サブウィンドウの描画
				/*
				sub_window.getRenderTexture()->clear(Color(Palette::White));
				ScopedRenderTarget2D target(*sub_window.getRenderTexture());
				font16(U"テストですと").draw(0, 0, Color(Palette::Black));
				sub_window.update();
				
				sub_window2.getRenderTexture()->clear(Color(Palette::White));
				target = ScopedRenderTarget2D(*sub_window2.getRenderTexture());
				font16(U"テストですよ").draw(0, 0, Color(Palette::Black));
				sub_window2.update();*/
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
		//sub_window.draw();
		//sub_window2.draw();
		
		// 時間を進ませる
		time = map.cityTime(1);
		
		// Details Barの表示
		detailsBar.printWeather(Weather::Sunny);
		detailsBar.printTemperature(map.getTemperature());
		detailsBar.printTime(time);
		detailsBar.printPopulation(map.getPopulation());
		detailsBar.printMoney(map.getMoney());
		
		// メニュー及びアドオン選択メニューの表示
		// アドオンが選択されたら、選択されたアドオンのポインタを返す
		selectedAddon = menu.draw(updateMap);
		menu.addonMenu();
		
		//font16(U"{:04d}"_fmt(time.year)+U"/"+U"{:02d}"_fmt(time.month)+U"/"+U"{:02d}"_fmt(time.date)+U" "+U"{:02d}"_fmt(time.hour)+U":"+U"{:02d}"_fmt(time.minutes)).draw(230, Scene::Height()-25-3);
		
		// マップ上でクリックされたらアドオンを設置
		if (selectedAddon != nullptr && MouseL.pressed() && cursor.position.y <= Scene::Height()-60-80) {
			// 除去モードなら除去
			if (selectedAddon->isInCategories(CategoryID::Tile)) {
				map.breaking(cursor.coordinate, false, true, true);
			}

			// 設置モードなら設置
			if (cursor.coordinate.x != beforeMousePressedCoordinate.x || cursor.coordinate.y != beforeMousePressedCoordinate.y) {
				map.build(cursor, cursor_before, selectedAddon, true);
				beforeMousePressedCoordinate = cursor.coordinate;
				updateMap = true;
				pressing = true;
			}
		}
		if (pressing) {
			if (!MouseL.pressed()) {
				beforeMousePressedCoordinate = {-1, -1};
				pressing = false;
			}
		}
		
		// BGMの再生中の処理
		bgm.playingBGM();
		
		System::Sleep(20);
	}
	
	map.freeMapAndAddons();
	
	return;
}
