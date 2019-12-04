//
//  SubWindow.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2019/12/04.
//

#ifndef SubWindow_hpp
#define SubWindow_hpp

#include "Specific.hpp"

class SubWindow {
public:
	SubWindow(String new_title, Font* new_font, SizeStruct new_size, Color background_color);
	
	RenderTexture* getRenderTexture();
	
	void move(PositionStruct new_position);
	
	void update();
	void draw();
	
private:
	RenderTexture render;
	RenderTexture render_window;
	
	SizeStruct size;
	PositionStruct position;
	
	String title;
	Font* font;
	
	Color background_color;
	
	bool moving;
};

#endif /* SubWindow_hpp */
