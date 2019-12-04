#include "Specific.hpp"

#include "StartUp.hpp"
#include "TitleMenu.hpp"
#include "Images.hpp"
#include "Addon.hpp"
#include "CityMap.hpp"
#include "SubWindow.hpp"

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
	
	// タイトルメニュー画面
	if (!titleMenu(images, font16)) {
		return;				// タイトル画面でウィンドウを閉じたらプログラム終了
	}
	
	CityMap map;
	Array<FileStruct> maps_path = specific::getAllFilesName("./data/maps", "cbd");
	map.load(maps_path[0]);
	
	// カメラの初期位置
	CameraStruct camera;
	camera.position = PositionStruct{-Scene::Width()/2+64/2, -150};
	
	// カーソルの位置
	CursorStruct cursor;
	cursor.texture = &(images.images["pointer"]["blue"].texture);
	
	PositionStruct cursor_position, cursor_position_before;
	
	// 描画処理
	RenderTexture buffer_texture(Scene::Width(), Scene::Height(), Color(50, 50, 50));
	bool update_map = true, first_loop = true;
	
	// サブウィンドウ
	SubWindow sub_window(U"Test Window", &font16, SizeStruct{400, 200}, Color(Palette::White));
	SubWindow sub_window2(U"Test Window2", &font16, SizeStruct{300, 150}, Color(Palette::White));
	
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
		cursor_position = PositionStruct{Cursor::Pos().x, Cursor::Pos().y};
		
		if (update_map || (cursor_position.x != cursor_position_before.x && cursor_position.y != cursor_position_before.y)) {
			cursor.coordinate = map.positionToCoordinate(cursor_position, camera);
			cursor.position = map.coordinateToPosition(cursor.coordinate, camera);
			
			cursor_position_before = cursor_position;
			update_map = true;
		}
		
		//cout << cursor.coordinate.x << "," << cursor.coordinate.y << endl;
		
		// マップの描画
		// マップを更新する必要がある場合はバッファに描画（更新）する
		if (update_map) {
			buffer_texture.clear(Color(50, 50, 50));
			
			ScopedRenderTarget2D target(buffer_texture);
			map.draw(camera, cursor);
			
			if (first_loop) {
				first_loop = false;
				
				// サブウィンドウの描画
				sub_window.getRenderTexture()->clear(Color(Palette::White));
				ScopedRenderTarget2D target(*sub_window.getRenderTexture());
				font16(U"テストですと").draw(0, 0, Color(Palette::Black));
				sub_window.update();
				
				sub_window2.getRenderTexture()->clear(Color(Palette::White));
				target = ScopedRenderTarget2D(*sub_window2.getRenderTexture());
				font16(U"テストですよ").draw(0, 0, Color(Palette::Black));
				sub_window2.update();
			}
			else {
				update_map = false;
			}
		}
		
		// バッファを描画
		buffer_texture.draw(0, 0);
		sub_window.draw();
		sub_window2.draw();
		
		System::Sleep(20);
	}
	
	map.freeMapAndAddons();
	
	return;
}
