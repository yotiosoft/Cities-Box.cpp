//
//  Images.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "Images.hpp"

void Images::load(string file_path, string tag, int transparent_r, int transparent_g, int transparent_b) {
	// ファイル一覧を取得
	vector<FileStruct> image_files = getAllFilesName(file_path, "png");
	
	// 各ファイルを読み込み
	for (int i=0; i<image_files.size(); i++) {
		Image image_temp(Unicode::Widen(image_files[i].file_path));
		
		if (transparent_r >= 0 && transparent_g >= 0 && transparent_b >= 0) {
			for (int h=0; h<image_temp.height(); h++) {
				for (int w=0; w<image_temp.width(); w++) {
					if (image_temp[w][h].r == transparent_r && image_temp[w][h].g == transparent_g && image_temp[w][h].b == transparent_b) {
						image_temp[w][h].setA(0);		// アルファ値を0に設定
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
	load(file_path, tag, -1, -1, -1);
}

