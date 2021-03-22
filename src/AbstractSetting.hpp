//
//  AbstractSetting.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/20.
//
//	設定の抽象クラス
//

#ifndef AbstractSetting_hpp
#define AbstractSetting_hpp

#include <Specific.hpp>
#include "Button.hpp"

class AbstractSetting {
public:
	// 設定画面
	virtual void screen(Font& font16) = 0;
	
	// 保存
	virtual void save() = 0;
};

#endif /* AbstractSetting_hpp */
