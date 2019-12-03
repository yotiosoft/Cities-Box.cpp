//
//  Specific.cpp
//  Cities Box
//
//  注 : これはmacOS向けです
//
//  Created by YotioSoft on 2019/12/01.
//

#include "Specific.hpp"

void specific::sleep(int ms) {
	usleep(ms*1000);		// ミリ秒単位
}
