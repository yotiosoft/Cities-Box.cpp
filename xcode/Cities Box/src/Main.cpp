#include "Header.hpp"
#include "Images.hpp"
#include "Addon.hpp"
#include "IOfunctions.hpp"

void Main() {
	Window::SetTitle(U"Cities Box.cpp");
	Scene::SetBackground(Color(50, 50, 50));
	
	/*---------------------------------------------
	 				ファイル読み込み
	 --------------------------------------------*/	
	Images images;
	images.load("../data/images", "images");
	
	vector<Addon> addons;
	vector<FileStruct> addons_path = getAllFilesName("../addons", "adat");
	
	for (int i=0; i<addons_path.size(); i++) {
		addons.push_back(Addon());
		addons.back().load(addons_path[i]);
	}
	
	Font font16(16);
	
	while (System::Update()) {
		/*
		// 背景画像
		images.images["images"]["title_background"].texture.resized(Scene::Width(), Scene::Height()).draw(0, 0);
		
		// ロゴ画像
		Texture logo_texture = images.images["images"]["logo"].texture;
		logo_texture.draw(Scene::Width()/2-logo_texture.width()/2, Scene::Height()/2-logo_texture.height()*1.75);
		 */
		
		font16(U"Addon Name     : "+Unicode::Widen(addons[0].getName())).draw(10, 0);
		font16(U"Addon Name(JP) : "+Unicode::Widen(addons[0].getNameJP())).draw(10, 20);
		font16(U"Author Name    : "+Unicode::Widen(addons[0].getAuthorName())).draw(10, 40);
		font16(U"Summary        : "+Unicode::Widen(addons[0].getSummary())).draw(10, 60);
		
		addons[0].draw("null", "left", PositionStruct{10, 100});
		font16(U"Type      : null").draw(80, 110);
		font16(U"Direction : left").draw(80, 130);
		
		addons[0].draw("null", "top", PositionStruct{10, 150});
		font16(U"Type      : null").draw(80, 160);
		font16(U"Direction : top").draw(80, 180);
		
		addons[0].draw("null", "right", PositionStruct{10, 200});
		font16(U"Type      : null").draw(80, 210);
		font16(U"Direction : right").draw(80, 230);
		
		addons[0].draw("null", "bottom", PositionStruct{10, 250});
		font16(U"Type      : null").draw(80, 260);
		font16(U"Direction : bottom").draw(80, 280);
		
	}
}
