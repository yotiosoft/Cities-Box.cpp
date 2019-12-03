#include "Header.hpp"

#include "StartUp.hpp"
#include "TitleMenu.hpp"
#include "Images.hpp"
#include "Addon.hpp"
#include "CityMap.hpp"
#include "IOfunctions.hpp"

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
	Array<FileStruct> maps_path = getAllFilesName("../data/maps", "cbd");
	map.load(maps_path[0]);
	
	// カメラの初期位置
	CameraStruct camera;
	camera.position = PositionStruct{-Scene::Width()/2+64/2, -150};
	
	DynamicTexture dtexture(Scene::Width(), Scene::Height());
	
	Image buffer;
	
	while (System::Update()) {
		// マップの描画
		map.draw(camera);
		
		// カメラの操作
		if (KeyLeft.pressed()) {
			camera.position.x -= 20;
		}
		if (KeyRight.pressed()) {
			camera.position.x += 20;
		}
		if (KeyUp.pressed()) {
			camera.position.y -= 10;
		}
		if (KeyDown.pressed()) {
			camera.position.y += 10;
		}
		
		specific::sleep(20);
	}
	
	map.freeMapAndAddons();
	
	return 0;
}
