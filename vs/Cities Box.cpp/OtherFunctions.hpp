//
//  OtherFunctions.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#ifndef OtherFunctions_hpp
#define OtherFunctions_hpp

#include "Header.hpp"

vector<string> split(string str, string separator);

void saveTextFile(string file_path, string str);
void saveTextFile(string file_path, vector<string> str_v);
void saveTextFile(string file_path, vector<FileStruct> fs);
void saveTextFile(string file_path, FileStruct fs);

string replaceString(string before_str, string search_str, string after_str);

#endif /* OtherFunctions_hpp */
