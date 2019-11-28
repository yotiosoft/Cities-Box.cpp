#include "Header.hpp"
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
	Images images;
	images.load("../data/images/area", "area", 0, 0, 0);
	images.load("../data/images/arrows", "arrows");
	images.load("../data/images/menu_setting", "menu_setting");
	images.load("../data/images/pointer", "pointer");
	images.load("../data/images/quarter", "quarter");
	images.load("../data/images/show_detail", "show_detail");
	images.load("../data/images/title_menu", "title_menu");
	
	map<string, Addon*> addons;
	vector<FileStruct> addons_path = getAllFilesName("../addons", "adat");
	
	for (int i=0; i<addons_path.size(); i++) {
		FileStruct file_temp = addons_path[i];
		addons[split(file_temp.file_name, ".")[0]] = new Addon();
		addons[split(file_temp.file_name, ".")[0]]->load(addons_path[i]);
	}
	
	
	CityMap map;
	vector<FileStruct> maps_path = getAllFilesName("../data/maps", "cbd");
	map.load(maps_path[0], addons);
	
	
	Font font16(16);
	
	// カメラの初期位置
	CameraStruct camera;
	camera.position = PositionStruct{64*map.getMapSize().width/2-Scene::Width()/2, 0};
	
	while (System::Update()) {
		/*
		// 背景画像
		images.images["images"]["title_background"].texture.resized(Scene::Width(), Scene::Height()).draw(0, 0);
		
		// ロゴ画像
		Texture logo_texture = images.images["images"]["logo"].texture;
		logo_texture.draw(Scene::Width()/2-logo_texture.width()/2, Scene::Height()/2-logo_texture.height()*1.75);
		 */
		
		for (int y=0; y<5; y++) {
			for (int x=0; x<5; x++) {
				map.draw_square(CoordinateStruct{x, y}, camera);
			}
		}
		
		// カメラの操作
		if (KeyLeft.pressed()) {
			camera.position.x -= 5;
		}
		if (KeyRight.pressed()) {
			camera.position.x += 5;
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
}
