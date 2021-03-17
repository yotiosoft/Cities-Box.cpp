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
	SubWindow(String newTitle, Font* newFont, SizeStruct newSize, Color backgroundColor);
	
	RenderTexture* getRenderTexture();
	
	void move(PositionStruct newPosition);
	
	void update();
	void draw();
	
private:
	RenderTexture m_render;
	RenderTexture m_render_window;
	
	SizeStruct m_size;
	PositionStruct m_position;
	
	String m_title;
	Font* m_font;
	
	Color m_background_color;
	
	bool m_moving;
};

#endif /* SubWindow_hpp */
