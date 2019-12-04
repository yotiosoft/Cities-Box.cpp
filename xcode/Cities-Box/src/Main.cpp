#include "Specific.hpp"

#include "StartUp.hpp"
#include "TitleMenu.hpp"
#include "Images.hpp"
#include "Addon.hpp"
#include "CityMap.hpp"

int Main() {
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
		return 0;				// タイトル画面でウィンドウを閉じたらプログラム終了
	}
	
	CityMap map;
	Array<FileStruct> maps_path = specific::getAllFilesName("../data/maps", "cbd");
	map.load(maps_path[0]);
	
	// カメラの初期位置
	CameraStruct camera;
	camera.position = PositionStruct{-Scene::Width()/2+64/2, -150};
	
	DynamicTexture dtexture(Scene::Width(), Scene::Height());
	
	Image buffer;
	
	// 描画処理
	RenderTexture buffer_texture(Scene::Width(), Scene::Height(), Color(50, 50, 50));
	bool update_map = true, first_loop = true;
	
	while (System::Update()) {
		// マップの描画
		if (update_map) {
			buffer_texture.clear(Color(50, 50, 50));
			
			ScopedRenderTarget2D target(buffer_texture);
			map.draw(camera);
			
			if (first_loop) {
				first_loop = false;
			}
			else {
				update_map = false;
			}
		}
		
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
		
		buffer_texture.draw(0, 0);
		System::Sleep(20);
	}
	
	map.freeMapAndAddons();
	
	return 0;
}
