//
//  Images.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "Images.hpp"

void Images::load(string file_path, string tag, Color transparent) {
	// ファイル一覧を取得
	Array<FileStruct> image_files = specific::getAllFilesName(file_path, "png");
	
	// 各ファイルを読み込み
	for (int i=0; i<image_files.size(); i++) {
		Image image_temp(Unicode::Widen(image_files[i].file_path));
		
		if (transparent.r >= 0 && transparent.g >= 0 && transparent.b >= 0) {
			for (int h=0; h<image_temp.height(); h++) {
				for (int w=0; w<image_temp.width(); w++) {
					if (image_temp[h][w].r == transparent.r && image_temp[h][w].g == transparent.g && image_temp[h][w].b == transparent.b) {
						image_temp[h][w].setA(0);		// アルファ値を0に設定
					}
				}
			}
		}
		
		ImageStruct new_image;
		new_image.texture = Texture(image_temp);
		new_image.file_path = file_path;
		new_image.file_name = image_files[i].file_path;
		new_image.tag = tag;
		
		images[tag][split(image_files[i].file_name, ".")[0]] = new_image;
	}
}

void Images::load(string file_path, string tag) {
	Color none_clear;
	none_clear.r = -1;
	none_clear.g = -1;
	none_clear.b = -1;
	
	load(file_path, tag, none_clear);
}

