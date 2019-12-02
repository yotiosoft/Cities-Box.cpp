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
		return 0;				// 画面を閉じたらプログラム終了
	}
	
	
	CityMap map;
	vector<FileStruct> maps_path = getAllFilesName("../data/maps", "cbd");
	map.load(maps_path[0]);
	
	// カメラの初期位置
	CameraStruct camera;
	camera.position = PositionStruct{-Scene::Width()/2+64/2, -150};
	
	DynamicTexture dtexture(Scene::Width(), Scene::Height());
	
	Image buffer;
	
	while (System::Update()) {
		for (int y=map.getDrawArea(camera)[0].y; y<map.getDrawArea(camera)[1].y; y++) {
			for (int x=map.getDrawArea(camera)[0].x; x<map.getDrawArea(camera)[1].x; x++) {
				PositionStruct draw_pos = map.coordinateToPosition(CoordinateStruct{x, y}, camera);
				
				if (draw_pos.x >= -CHIP_SIZE && draw_pos.y >= -CHIP_SIZE/2 && draw_pos.x <= Scene::Width() && draw_pos.y <= Scene::Height() + CHIP_SIZE*2) {
					map.drawSquare(CoordinateStruct{x, y}, camera);
				}
			}
		}
		
		// カメラの操作
		if (KeyLeft.pressed()) {
			camera.position.x -= 10;
		}
		if (KeyRight.pressed()) {
			camera.position.x += 10;
		}
		if (KeyUp.pressed()) {
			camera.position.y -= 5;
		}
		if (KeyDown.pressed()) {
			camera.position.y += 5;
		}
		
		specific::sleep(3);
	}
	
	map.freeMapAndAddons();
	
	return 0;
}
