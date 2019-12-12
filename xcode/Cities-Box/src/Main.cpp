#include "Specific.hpp"

#include "StartUp.hpp"
#include "TitleMenu.hpp"
#include "Images.hpp"
#include "Addon.hpp"
#include "CityMap.hpp"
#include "SubWindow.hpp"
#include "Menu.hpp"

void Main() {
	Window::SetTitle(U"Cities Box.cpp");
	Scene::SetBackground(Color(50, 50, 50));
	
	/*---------------------------------------------
	 				ファイル読み込み
	 --------------------------------------------*/	
	// 画像の読み込み
	Images images;
	loadImages(images);
	
	// フォントの宣言
	Font font16(16);
	Font font12(12);
	Font font8(8);
	
	// タイトルメニュー画面
	String map_file_path;
	if (!titleMenu(images, font16, map_file_path)) {
		return;				// タイトル画面でウィンドウを閉じたらプログラム終了
	}
	
	// マップとアドオンの読み込み
	CityMap map;
	map.load(map_file_path);
	
	//map.save();
	
	// カメラの初期位置
	CameraStruct camera;
	camera.position = PositionStruct{-Scene::Width()/2+64/2, -150};
	
	// カーソルの位置
	CursorStruct cursor;
	cursor.texture = &(images.images["pointer"]["blue"].texture);
	
	// 描画処理
	RenderTexture buffer_texture(Scene::Width(), Scene::Height(), Color(30, 30, 30));
	bool update_map = true, first_loop = true;
	
	// サブウィンドウ
	SubWindow sub_window(U"Test Window", &font16, SizeStruct{400, 200}, Color(Palette::White));
	SubWindow sub_window2(U"Test Window2", &font16, SizeStruct{300, 150}, Color(Palette::White));
	
	// メニュー
	Menu menu;
	menu.set(PositionStruct{0, Scene::Height()-60}, SizeStruct{Scene::Width(), 60}, &map, &font8, &font12, &font16);
	
	// 選択されたアドオン
	Addon* selected_addon;
	
	// 前回左クリックしたときのマップ上の座標
	CoordinateStruct before_mouse_pressed_coordinate;
	bool pressing = false;
	
	while (System::Update()) {
		// カメラの操作
		if (KeyLeft.pressed()) {
			camera.position.x -= 20;
			update_map = true;
		}
		if (KeyRight.pressed()) {
			camera.position.x += 20;
			update_map = true;
		}
		if (KeyUp.pressed()) {
			camera.position.y -= 10;
			update_map = true;
		}
		if (KeyDown.pressed()) {
			camera.position.y += 10;
			update_map = true;
		}
		
		// カーソルの位置を取得
		if (update_map || Cursor::Delta().x != 0 || Cursor::Delta().y != 0) {
			cursor.position = PositionStruct{Cursor::Pos().x, Cursor::Pos().y};
			cursor.coordinate = map.positionToCoordinate(cursor.position, camera);
			cursor.position_per_tiles = map.coordinateToPosition(cursor.coordinate, camera);
			
			update_map = true;
		}
		
		//cout << cursor.coordinate.x << "," << cursor.coordinate.y << endl;
		
		// マップなどを更新する必要がある場合はバッファに描画（更新）する
		if (update_map) {
			buffer_texture.clear(Color(30, 30, 30));
			
			// マップの描画
			ScopedRenderTarget2D target(buffer_texture);
			map.draw(camera, cursor);
			
			menu.update();
			
			if (first_loop) {
				first_loop = false;
				
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
				update_map = false;
			}
		}
		
		// バッファを描画
		//buffer_texture.draw(0, 0);
		//sub_window.draw();
		//sub_window2.draw();
		
		// メニュー及びアドオン選択メニューの表示
		// アドオンが選択されたら、選択されたアドオンのポインタを返す
		selected_addon = menu.draw(update_map);
		menu.addonMenu();
		
		// マップ上でクリックされたらアドオンを設置
		if (selected_addon != nullptr && MouseL.pressed() && cursor.position.y <= Scene::Height()-60-80) {
			if (cursor.coordinate.x != before_mouse_pressed_coordinate.x || cursor.coordinate.y != before_mouse_pressed_coordinate.y) {
				map.build(cursor.coordinate, selected_addon);
				before_mouse_pressed_coordinate = cursor.coordinate;
				update_map = true;
				pressing = true;
			}
		}
		if (pressing) {
			if (!MouseL.pressed()) {
				before_mouse_pressed_coordinate = {-1, -1};
				pressing = false;
			}
		}
		
		System::Sleep(20);
	}
	
	map.freeMapAndAddons();
	
	return;
}
