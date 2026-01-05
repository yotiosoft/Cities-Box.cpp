//
//  Images.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//
#pragma once

#ifndef Images_hpp
#define Images_hpp

#include <Specific.hpp>
#include "UnitaryTools.hpp"
#include <rs-images.h>

#endif /* Images_hpp */

class ImagesStruct {
public:
    ImagesStruct();

    // 指定したパスから画像をロード
    void load(const string filePath, const string tag);
    void load(const string filePath, const string tag, const Color transparent);

    // 外部からテクスチャを参照するためのマップ
    // images[tag][filename_without_ext]
    std::map<string, std::map<string, ImageStruct>> images;

private:
    // Rust側のカタログ管理オブジェクト
    rust::Box<rust::images::ImageCatalog> m_catalog;
};
