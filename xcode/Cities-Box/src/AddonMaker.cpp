//
//  AddonMaker.cpp
//  Addon Maker on Command Line
//
//  Created by YotioSoft on 2020/02/13.
//  Copyright © 2020 YotioSoft. All rights reserved.
//

#include "AddonMaker.hpp"

void AddonMaker::menu(Font& font16) {
	Array<String> categories_list = {U"住居", U"商業", U"オフィス", U"工業", U"農業", U"公共施設", U"公園", U"道路", U"線路と駅", U"水路", U"空港"};
	PulldownMenu categories_pulldown_menu(categories_list, font16, SizeStruct{400, 40}, PositionStruct{30, 130});
	
	Array<String> density_list = {U"低密度", U"高密度"};
	PulldownMenu density_pulldown_menu(density_list, font16, SizeStruct{400, 40}, PositionStruct{30, 230});
	
	Array<String> residential_list = {U"一軒家", U"アパート", U"マンション", U"団地", U"別荘", U"シェアハウス", U"地下壕", U"その他"};
	PulldownMenu residential_pulldown_menu(residential_list, font16, SizeStruct{400, 40}, PositionStruct{30, 330});
	
	Array<String> commercial_list = {U"小売店", U"飲食店", U"スーパーマーケット", U"コンビニ", U"百貨店", U"駐車場", U"複合型商業施設", U"その他"};
	PulldownMenu commercial_pulldown_menu(commercial_list, font16, SizeStruct{400, 40}, PositionStruct{30, 330});
	
	Array<String> office_list = {U"オフィスビル", U"貸し会議室", U"塾", U"会議場", U"研修所", U"その他"};
	PulldownMenu office_pulldown_menu(office_list, font16, SizeStruct{400, 40}, PositionStruct{30, 230});
	
	Array<String> industrial_list = {U"工場", U"製鉄所", U"倉庫", U"町工場", U"その他"};
	PulldownMenu industrial_pulldown_menu(industrial_list, font16, SizeStruct{400, 40}, PositionStruct{30, 330});
	
	Array<String> farm_list = {U"田", U"畑", U"牧場", U"その他"};
	PulldownMenu farm_pulldown_menu(farm_list, font16, SizeStruct{400, 40}, PositionStruct{30, 230});
	
	Array<String> public_list = {U"市役所", U"行政関連施設", U"省庁舎", U"警察署", U"消防車", U"公民館", U"学校", U"図書館", U"博物館", U"その他"};
	PulldownMenu public_pulldown_menu(public_list, font16, SizeStruct{400, 40}, PositionStruct{30, 230});
	
	Button return_button(IconFont::LeftWithCircle, 30, 30, PositionStruct{0, 0});
	
	PulldownMenu* second_menu = nullptr;
	PulldownMenu* third_menu = nullptr;
	
	while (System::Update()) {
		return_button.put(PositionStruct{20, 20});
		
		if (return_button.push()) {
			return;
		}
		
		font16(U"アドオンを作成する").draw(60, 25, Palette::White);
		font16(U"作成するアドオンのカテゴリを選んでください").draw(20, 100, Palette::White);
		
		switch (categories_pulldown_menu.selected_number()) {
			case 0:			// 住居
				second_menu = &density_pulldown_menu;
				third_menu = &residential_pulldown_menu;
				break;
			case 1:			// 商業
				second_menu = &density_pulldown_menu;
				third_menu = &commercial_pulldown_menu;
				break;
			case 2:			// オフィス
				second_menu = &office_pulldown_menu;
				third_menu = nullptr;
				break;
			case 3:			// 工業
				second_menu = &density_pulldown_menu;
				third_menu = &industrial_pulldown_menu;
				break;
			case 4:			// 農業
				second_menu = &farm_pulldown_menu;
				third_menu = nullptr;
				break;
			case 5:			// 公共施設
				second_menu = &public_pulldown_menu;
				third_menu = nullptr;
				break;
			default:
				second_menu = nullptr;
				third_menu = nullptr;
		}
		
		if (third_menu != nullptr) {
			font16(U"建物の種類を選んでください").draw(20, 300, Palette::White);
			third_menu->draw();
		}
		if (second_menu != nullptr) {
			font16(U"アドオンの密度を選択してください").draw(20, 200, Palette::White);
			second_menu->draw();
		}
		
		categories_pulldown_menu.draw();
	}
}
