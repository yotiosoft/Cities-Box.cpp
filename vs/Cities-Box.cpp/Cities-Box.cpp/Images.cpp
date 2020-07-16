//
//  Images.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "Images.hpp"

void Images::load(string filePath, string tag, Color transparent) {
	// ファイル一覧を取得
	Array<FileStruct> imageFiles = specific::getAllFilesName(filePath, "png");
	
	// 各ファイルを読み込み
	for (int i=0; i<imageFiles.size(); i++) {
		Image imageTemp(Unicode::Widen(imageFiles[i].file_path));
		
		if (transparent.r >= 0 && transparent.g >= 0 && transparent.b >= 0) {
			for (int h=0; h<imageTemp.height(); h++) {
				for (int w=0; w<imageTemp.width(); w++) {
					if (imageTemp[h][w].r == transparent.r && imageTemp[h][w].g == transparent.g && imageTemp[h][w].b == transparent.b) {
						imageTemp[h][w].setA(0);		// アルファ値を0に設定
					}
				}
			}
		}
		
		ImageStruct newImage;
		newImage.texture = Texture(imageTemp);
		newImage.file_path = filePath;
		newImage.file_name = imageFiles[i].file_path;
		newImage.tag = tag;
		
		images[tag][splitUTF8(imageFiles[i].file_name, ".")[0]] = newImage;
	}
}

void Images::load(string filePath, string tag) {
	Color noneClear;
	noneClear.r = -1;
	noneClear.g = -1;
	noneClear.b = -1;
	
	load(filePath, tag, noneClear);
}
