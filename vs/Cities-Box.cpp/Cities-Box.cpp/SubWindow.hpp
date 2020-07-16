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
	RenderTexture render;
	RenderTexture renderWindow;
	
	SizeStruct size;
	PositionStruct position;
	
	String title;
	Font* font;
	
	Color backgroundColor;
	
	bool moving;
};

#endif /* SubWindow_hpp */
