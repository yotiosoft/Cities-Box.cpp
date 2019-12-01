//
//  Button.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/12/01.
//

#ifndef Button_hpp
#define Button_hpp

#include "Header.hpp"

class Button {
public:
	// コンストラクタ
	Button(ImageStruct& image);					// 画像から
	Button(ImageStruct& image, int new_size);	// 画像から
	Button(IconFont::ID icon_id);				// ロゴフォントから
	Button(IconFont::ID icon_id, int new_size);	// ロゴフォントから
	
	// 配置
	void put(PositionStruct new_position);
	
	// 押されたかどうか
	bool push();
	
private:
	Texture button_texture;
	
	bool isIcon;
	
	PositionStruct position;
	SizeStruct size;
};

#endif /* Button_hpp */
