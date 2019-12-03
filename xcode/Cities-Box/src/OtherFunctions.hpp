//
//  OtherFunctions.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#ifndef OtherFunctions_hpp
#define OtherFunctions_hpp

#include "Specific.hpp"

Array<string> split(string str, string separator);

void cMes(Font& font, String str, PositionStruct top_left, PositionStruct bottom_right, Color& color);
void cMes(Font& font, String str, PositionStruct top_left, SizeStruct size, Color& color);

#endif /* OtherFunctions_hpp */
