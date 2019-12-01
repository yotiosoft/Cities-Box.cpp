#include "Header.hpp"

#include "StartUp.hpp"
#include "TitleMenu.hpp"
#include "Images.hpp"
#include "Addon.hpp"
#include "CityMap.hpp"
#include "IOfunctions.hpp"

void Main() {
	Window::SetTitle(U"Cities Box.cpp");
	Scene::SetBackground(Color(50, 50, 50));
	
	/*---------------------------------------------
	 				ファイル読み込み
	 --------------------------------------------*/	
	// 画像の読み込み
	Images images;
	loadImages(images);
	
	// タイトルメニュー画面
	titleMenu(images);
	
	map<string, Addon*> addons;
	vector<FileStruct> addons_path = getAllFilesName("../addons", "adat");
	
	for (int i=0; i<addons_path.size(); i++) {
		FileStruct file_temp = addons_path[i];
		addons[split(file_temp.file_name, ".")[0]] = new Addon();
		addons[split(file_temp.file_name, ".")[0]]->load(addons_path[i]);
		
		specific::sleep(100);
	}
	
	
	CityMap map;
	vector<FileStruct> maps_path = getAllFilesName("../data/maps", "cbd");
	map.load(maps_path[0], addons);
	
	
	Font font16(16);
	
	// カメラの初期位置
	CameraStruct camera;
	camera.position = PositionStruct{-Scene::Width()/2+64/2, -150};
	
	DynamicTexture dtexture(Scene::Width(), Scene::Height());
	
	Image buffer;
	
	
	while (System::Update()) {
		for (int y=map.getDrawArea(camera)[0].y; y<map.getDrawArea(camera)[1].y; y++) {
			for (int x=map.getDrawArea(camera)[0].x; x<map.getDrawArea(camera)[1].x; x++) {
				map.draw_square(CoordinateStruct{x, y}, camera);
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
	}
	
	for (auto i = addons.begin(); i != addons.end() ; i++) {
		free(i->second);
	}
	
	map.free();
	
	specific::sleep(50);
}
