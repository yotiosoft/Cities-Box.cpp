//
//  Images.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "Images.hpp"

Images::Images() 
    : m_catalog(new_catalog()) // Rust側のオブジェクトを初期化
{}

void Images::load(const String& filePath, const String& tag) {
    // 透明色指定なしの場合は、無効な色を渡して呼び出し
    load(filePath, tag, Color(-1, -1, -1));
}

void Images::load(const String& filePath, const String& tag, const Color& transparent) {
    // 1. Rustにディレクトリをスキャンさせる
    // String(Siv3D) -> std::string -> char* 経由でRustへ
    m_catalog->scan_directory(filePath.toUTF8());

    // 2. Rustからスキャン結果の一覧を取得
    auto entries = m_catalog->get_entries();

    // 3. 各ファイルを読み込み
    for (const auto& entry : entries) {
        // Rustから届いたUTF-8パスをSiv3DのUnicode(FilePath)に変換
        FilePath s3dPath = Unicode::FromUTF8(entry.full_path);
        Image imageTemp(s3dPath);

        if (!imageTemp) continue;

        // 4. 透明色処理 (元のロジックを継承)
        if (transparent.r >= 0 && transparent.g >= 0 && transparent.b >= 0) {
            for (auto& pixel : imageTemp) {
                if (pixel.r == transparent.r && pixel.g == transparent.g && pixel.b == transparent.b) {
                    pixel.setA(0);
                }
            }
        }

        // 5. テクスチャ化してマップに格納
        ImageStruct newImage;
        newImage.texture = Texture(imageTemp);
        newImage.file_path = filePath;
        newImage.file_name = Unicode::FromUTF8(entry.file_name);
        newImage.tag = tag;

        // entry.stem はファイル名（拡張子なし）
        images[tag][Unicode::FromUTF8(entry.stem)] = newImage;
    }
}
