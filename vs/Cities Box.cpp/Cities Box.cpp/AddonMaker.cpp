//
//  AddonMaker.cpp
//  Addon Maker on Command Line
//
//  Created by YotioSoft on 2020/02/13.
//  Copyright c 2020 YotioSoft. All rights reserved.
//

#include "AddonMaker.hpp"

void AddonMaker::menu(Font& font16) {
	Array<String> categoriesList = {U"住居", U"商業", U"オフィス", U"工業", U"農業", U"公共施設", U"公園", U"道路", U"線路と駅", U"水路", U"空港"};
	PulldownMenu categoriesPulldownMenu(categoriesList, font16, SizeStruct{400, 40}, PositionStruct{30, 130});
	
	Array<String> densityList = {U"低密度", U"高密度"};
	PulldownMenu densityPulldownMenu(densityList, font16, SizeStruct{400, 40}, PositionStruct{30, 230});
	
	Array<String> lowDensityResidentialList = {U"一軒家", U"シェアハウス", U"二世帯住宅", U"別荘", U"その他"};
	PulldownMenu lowDensityResidentialPulldownMenu(lowDensityResidentialList, font16, SizeStruct{400, 40}, PositionStruct{30, 330});
	Array<String> highDensityResidentialList = {U"アパート", U"マンション", U"団地", U"その他"};
	PulldownMenu highDensityResidentialPulldownMenu(highDensityResidentialList, font16, SizeStruct{400, 40}, PositionStruct{30, 330});
	
	Array<String> lowDensityCommercialList = {U"小売店", U"飲食店", U"スーパーマーケット", U"コンビニ", U"駐車場", U"その他"};
	PulldownMenu lowDensityCommercialPulldownMenu(lowDensityCommercialList, font16, SizeStruct{400, 40}, PositionStruct{30, 330});
	Array<String> highDensityCommercialList = {U"百貨店", U"複合型商業施設", U"その他"};
	PulldownMenu highDensityCommercialPulldownMenu(highDensityCommercialList, font16, SizeStruct{400, 40}, PositionStruct{30, 330});
	
	Array<String> officeList = {U"オフィスビル", U"貸し会議室", U"塾", U"会議場", U"研修所", U"その他"};
	PulldownMenu officePulldownMenu(officeList, font16, SizeStruct{400, 40}, PositionStruct{30, 230});
	
	Array<String> lowDensityIndustrialList = {U"小規模な工場", U"倉庫", U"町工場", U"その他"};
	PulldownMenu lowDensityIndustrialPulldownMenu(lowDensityIndustrialList, font16, SizeStruct{400, 40}, PositionStruct{30, 330});
	Array<String> highDensityIndustrialList = {U"大規模な工場", U"その他"};
	PulldownMenu highDensityIndustrialPulldownMenu(highDensityIndustrialList, font16, SizeStruct{400, 40}, PositionStruct{30, 330});
	
	Array<String> farmList = {U"田", U"畑", U"牧場", U"その他"};
	PulldownMenu farmPulldownMenu(farmList, font16, SizeStruct{400, 40}, PositionStruct{30, 230});
	
	Array<String> publicList = {U"市役所", U"行政関連施設", U"省庁舎", U"警察署", U"消防車", U"公民館", U"学校", U"図書館", U"博物館", U"その他"};
	PulldownMenu publicPulldownMenu(publicList, font16, SizeStruct{400, 40}, PositionStruct{30, 230});
	
	Button returnButton(IconFont::LeftWithCircle, 30, 30, PositionStruct{0, 0});
	
	PulldownMenu* secondMenu = nullptr;
	PulldownMenu* thirdMenu = nullptr;
	
	while (System::Update()) {
		returnButton.put(PositionStruct{20, 20});
		
		if (returnButton.push()) {
			return;
		}
		
		font16(U"アドオンを作成する").draw(60, 25, Palette::White);
		font16(U"作成するアドオンのカテゴリを選んでください").draw(20, 100, Palette::White);
		
		switch (categoriesPulldownMenu.selectedNumber()) {
			case 0:			// 住居
				secondMenu = &densityPulldownMenu;
				if (secondMenu->selectedNumber() == 0) {
					thirdMenu = &lowDensityResidentialPulldownMenu;
				}
				else {
					thirdMenu = &highDensityResidentialPulldownMenu;
				}
				break;
			case 1:			// 商業
				secondMenu = &densityPulldownMenu;
				if (secondMenu->selectedNumber() == 0) {
					thirdMenu = &lowDensityCommercialPulldownMenu;
				}
				else {
					thirdMenu = &highDensityCommercialPulldownMenu;
				}
				break;
			case 2:			// オフィス
				secondMenu = &officePulldownMenu;
				thirdMenu = nullptr;
				break;
			case 3:			// 工業
				secondMenu = &densityPulldownMenu;
				if (secondMenu->selectedNumber() == 0) {
					thirdMenu = &lowDensityIndustrialPulldownMenu;
				}
				else {
					thirdMenu = &highDensityIndustrialPulldownMenu;
				}
				break;
			case 4:			// 農業
				secondMenu = &farmPulldownMenu;
				thirdMenu = nullptr;
				break;
			case 5:			// 公共施設
				secondMenu = &publicPulldownMenu;
				thirdMenu = nullptr;
				break;
			default:
				secondMenu = nullptr;
				thirdMenu = nullptr;
		}
		
		if (thirdMenu != nullptr) {
			font16(U"建物の種類を選んでください").draw(20, 300, Palette::White);
			thirdMenu->draw();
		}
		if (secondMenu != nullptr) {
			font16(U"アドオンの密度を選択してください").draw(20, 200, Palette::White);
			secondMenu->draw();
		}
		
		categoriesPulldownMenu.draw();
	}
}
