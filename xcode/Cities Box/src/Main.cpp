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
	
	map<string, Addon> addons;
	vector<FileStruct> addons_path = getAllFilesName("../addons", "adat");
	
	for (int i=0; i<addons_path.size(); i++) {
		FileStruct file_temp = addons_path[i];
		addons[split(file_temp.file_name, ".")[0]].load(addons_path[i]);
	}
	
	
	CityMap map;
	vector<FileStruct> maps_path = getAllFilesName("../data/maps", "cbd");
	map.load(maps_path[0]);
	
	
	Font font16(16);
	
	while (System::Update()) {
		/*
		// 背景画像
		images.images["images"]["title_background"].texture.resized(Scene::Width(), Scene::Height()).draw(0, 0);
		
		// ロゴ画像
		Texture logo_texture = images.images["images"]["logo"].texture;
		logo_texture.draw(Scene::Width()/2-logo_texture.width()/2, Scene::Height()/2-logo_texture.height()*1.75);
		 */
		
		font16(U"Addon Name     : "+Unicode::Widen(addons["red_roof_house"].getName())).draw(10, 0);
		font16(U"Addon Name(JP) : "+Unicode::Widen(addons["red_roof_house"].getNameJP())).draw(10, 20);
		font16(U"Author Name    : "+Unicode::Widen(addons["red_roof_house"].getAuthorName())).draw(10, 40);
		font16(U"Summary        : "+Unicode::Widen(addons["red_roof_house"].getSummary())).draw(10, 60);
		
		addons["red_roof_house"].draw("null", "left", PositionStruct{10, 100});
		font16(U"Type      : null").draw(80, 110);
		font16(U"Direction : left").draw(80, 130);
		
		addons["red_roof_house"].draw("null", "top", PositionStruct{10, 150});
		font16(U"Type      : null").draw(80, 160);
		font16(U"Direction : top").draw(80, 180);
		
		addons["red_roof_house"].draw("null", "right", PositionStruct{10, 200});
		font16(U"Type      : null").draw(80, 210);
		font16(U"Direction : right").draw(80, 230);
		
		addons["red_roof_house"].draw("null", "bottom", PositionStruct{10, 250});
		font16(U"Type      : null").draw(80, 260);
		font16(U"Direction : bottom").draw(80, 280);
		
		images.images["area"]["commercial"].texture.draw(10, 300);
	}
}
