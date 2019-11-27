//
//  Images.cpp
//

#include "Images.hpp"

void Images::load(string file_path, string tag) {
	// ファイル一覧を取得
	vector<FileStruct> image_files = getAllFilesName(file_path, "png");
	
	// 各ファイルを読み込み
	for (int i=0; i<image_files.size(); i++) {
		ImageStruct new_image;
		new_image.texture = Texture(Unicode::Widen(image_files[i].file_path));
		new_image.file_path = file_path;
		new_image.file_name = image_files[i].file_path;
		new_image.tag = tag;
		
		images[tag][split(image_files[i].file_name, ".")[0]] = new_image;
		cout << "image: " << tag << " " << split(image_files[i].file_name, ".")[0] << endl;
	}
}
