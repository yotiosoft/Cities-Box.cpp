//
//  Button.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/12/01.
//

#ifndef Button_hpp
#define Button_hpp

#include "Specific.hpp"

class Button {
public:
	// コンストラクタ
	Button();
	Button(ImageStruct& image);																										// 画像から
	Button(ImageStruct& image, int new_size);																						// 画像から
	Button(ImageStruct& image, int new_size, String new_label, Font& new_font);														// 画像から
	Button(IconFont::ID icon_id);																									// ロゴフォントから
	Button(IconFont::ID icon_id, int new_size, int new_icon_size, PositionStruct new_shift);										// ロゴフォントから
	Button(IconFont::ID icon_id, int new_size, int new_icon_size, PositionStruct new_shift, String new_label, Font& new_font);		// ロゴフォントから
	
	// 設定
	void set(ImageStruct& image);																									// 画像から
	void set(ImageStruct& image, int new_size);																						// 画像から
	void set(ImageStruct& image, int new_size, String new_label, Font& new_font);													// 画像から
	void set(IconFont::ID icon_id);																									// ロゴフォントから
	void set(IconFont::ID icon_id, int new_size, int new_icon_size, PositionStruct new_shift);										// ロゴフォントから
	void set(IconFont::ID icon_id, int new_size, int new_icon_size, PositionStruct new_shift, String new_label, Font& new_font);	// ロゴフォントから
	
	// 配置
	void put(PositionStruct new_position);
	
	// 押されたかどうか
	bool push();
	
	// アクティブかどうか
	bool isActive();
	
	// 選択解除
	void release();
	
private:
	Texture button_texture;
	
	bool isIcon;
	
	PositionStruct position;
	SizeStruct size;
	SizeStruct icon_size;
	
	PositionStruct shift;
	
	Font font;
	String label;
	PositionStruct label_position;
	
	bool active;
};

#endif /* Button_hpp */
