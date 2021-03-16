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
	Button(ImageStruct& image, int newSize);																						// 画像から
	Button(ImageStruct& image, int newSize, String newLabel, Font& newFont);														// 画像から
	Button(IconFont::ID iconID);																									// ロゴフォントから
	Button(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift);												// ロゴフォントから
	Button(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift, String newLabel, Font& newFont);				// ロゴフォントから
	
	// 設定
	void set(ImageStruct& image);																									// 画像から
	void set(ImageStruct& image, int newSize);																						// 画像から
	void set(ImageStruct& image, int newSize, String newLabel, Font& newFont);														// 画像から
	void set(IconFont::ID iconID);																									// ロゴフォントから
	void set(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift);											// ロゴフォントから
	void set(IconFont::ID iconID, int newSize, int newIconSize, PositionStruct newShift, String newLabel, Font& newFont);			// ロゴフォントから
	
	// 配置
	void put(PositionStruct newPosition);
	void putRelative(PositionStruct newPosition, PositionStruct leftTop);
	
	// 押されたかどうか
	bool push();
	bool pushRelative(PositionStruct leftTop);
	
	// アクティブかどうか
	bool isActive();
	
	// 選択解除
	void release();
	
private:
	Texture m_button_texture;
	
	bool m_is_icon;
	
	PositionStruct m_position;
	SizeStruct m_size;
	SizeStruct m_icon_size;
	
	PositionStruct m_shift;
	
	Font m_font;
	String m_label;
	PositionStruct m_label_position;
	
	bool m_active;
};

#endif /* Button_hpp */
