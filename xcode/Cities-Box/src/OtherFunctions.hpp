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

void saveTextFile(string file_path, string str);
void saveTextFile(string file_path, Array<string> str_v);
void saveTextFile(string file_path, Array<FileStruct> fs);
void saveTextFile(string file_path, FileStruct fs);

string replaceString(string before_str, string search_str, string after_str);

#endif /* OtherFunctions_hpp */
