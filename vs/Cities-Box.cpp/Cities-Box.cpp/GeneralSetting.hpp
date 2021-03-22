//
//  GeneralSetting.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/20.
//

#ifndef GeneralSetting_hpp
#define GeneralSetting_hpp

#include "AbstractSetting.hpp"

class GeneralSetting : public AbstractSetting {
public:
	// 設定画面
	void screen(Font& font16);
	
	// 開発者モードの取得
	bool isDeveloperMode();
	
	// 保存
	void save();
	
private:
	bool m_developer_mode = false;
};

#endif /* GeneralSetting_hpp */
