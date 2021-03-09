#include "Specific.hpp"

#include "StartUp.hpp"
#include "TitleMenu.hpp"
#include "Images.hpp"
#include "Sound.hpp"
#include "Addon.hpp"
#include "CityMap.hpp"
#include "SubWindow.hpp"
#include "DetailsBar.hpp"
#include "Menu.hpp"

void Main() {
	//Window::SetTitle(U"Cities Box.cpp");
	specific::chdir("./Cities-Box.app/Contents/Resources/assets");
	
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetScaleMode(ScaleMode::ResizeFill);
	
	Scene::SetBackground(Color(50, 50, 50));
	
	/*---------------------------------------------
	 				ファイル読み込み
	 --------------------------------------------*/	
	// 画像の読み込み
	Images images;
	loadImages(images);
	
	// サウンドファイルの読み込み
	Sound bgm;
	bgm.searchSoundFiles("./sound/BGM");
	
	// フォントの宣言
	Font font16(16, U"./fonts/NotoSansCJKjp/NotoSansCJKjp-Bold.otf");
	Font font12(12, U"./fonts/NotoSansCJKjp/NotoSansCJKjp-Regular.otf");
	Font font8(8, U"./fonts/NotoSansCJKjp/NotoSansCJKjp-Regular.otf");
	
	// タイトルメニュー画面
	String mapFilePath;
	if (!titleMenu(images, font16, mapFilePath)) {
		return;				// タイトル画面でウィンドウを閉じたらプログラム終了
	}
	
	// マップとアドオンの読み込み
	CityMap map;
	map.load(mapFilePath);
	
	//map.save();
	
	// カメラの初期位置
	CameraStruct camera;
	camera.position = PositionStruct{-Scene::Width()/2+64/2, -150};
	
	// カーソルの位置
	CursorStruct cursor;
	cursor.texture = &(images.images["pointer"]["blue"].texture);
	
	// 描画処理
	RenderTexture bufferTexture(Scene::Width(), Scene::Height(), Color(30, 30, 30));
	bool updateMap = true, firstLoop = true;
	
	// サブウィンドウ
	SubWindow subWindow(U"Test Window", &font16, SizeStruct{400, 200}, Color(Palette::White));
	SubWindow subWindow2(U"Test Window2", &font16, SizeStruct{300, 150}, Color(Palette::White));
	
	// メニュー
	Menu menu;
	menu.set(PositionStruct{0, Scene::Height()-50}, SizeStruct{Scene::Width(), 50}, &map, &font8, &font12, &font16);
	
	// 選択されたアドオン
	Addon* selectedAddon;
	
	// 前回左クリックしたときのマップ上の座標
	CoordinateStruct beforeMousePressedCoordinate = {0, 0};
	bool pressing = false;
	
	// 時間
	TimeStruct time;
	
	// Details Barの設定
	DetailsBar detailsBar(PositionStruct{Scene::Size().x-450, 10}, &font16);
	
	// BGMの再生（ランダム）
	bgm.playBGM();
	
	while (System::Update()) {
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
		if (updateMap || Cursor::Delta().x != 0 || Cursor::Delta().y != 0) {
			cursor.position = PositionStruct{Cursor::Pos().x, Cursor::Pos().y};
			cursor.coordinate = map.positionToCoordinate(cursor.position, camera);
			cursor.position_per_tiles = map.coordinateToPosition(cursor.coordinate, camera);
			
			updateMap = true;
		}
		
		//cout << "cursor: " << cursor.coordinate.x << "," << cursor.coordinate.y << " : " << map.getAddon(cursor.coordinate)[0].getName() << endl;
		
		// マップなどを更新する必要がある場合はバッファに描画（更新）する
		if (updateMap) {
			bufferTexture.clear(Color(30, 30, 30));
			
			// マップの描画
			ScopedRenderTarget2D target(bufferTexture);
			map.draw(camera, cursor);
			
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
		
		// バッファを描画
		bufferTexture.draw(0, 0);
		//sub_window.draw();
		//sub_window2.draw();
		
		// 時間を進ませて表示する
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
			if (cursor.coordinate.x != beforeMousePressedCoordinate.x || cursor.coordinate.y != beforeMousePressedCoordinate.y) {
				map.build(cursor.coordinate, selectedAddon, true);
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
