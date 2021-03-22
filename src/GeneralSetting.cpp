//
//  GeneralSetting.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/20.
//

#include "GeneralSetting.hpp"

void GeneralSetting::screen(Font& font16) {
	Button return_button(IconFont::LeftWithCircle, 30, 30, PositionStruct{0, 0});
	
	while (System::Update()) {
		Rect(0, 0, Scene::Size().x/5, Scene::Size().y).draw(Color(40, 40, 40));
		font16(U"設定").draw(60, 23, Palette::White);
		
		// メニューボタンの設置
		return_button.put(PositionStruct{20, 20});				// 戻る
		font16(U"開発者設定").draw(20, 80, Palette::White);		// 開発者設定
		
		// 画面本体
		int leftmost = Scene::Size().x/5 + 20;
		SimpleGUI::CheckBox(m_developer_mode, U"", Vec2(leftmost, 30));
		
		// 戻るボタンの処理
		if (return_button.push()) {
			return_button.release();
			return;
		}
	}
}

bool GeneralSetting::isDeveloperMode() {
	return m_developer_mode;
}

void GeneralSetting::save() {
	
}
