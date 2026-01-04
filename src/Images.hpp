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
#include <citiesbox-rs/citiesbox-rs.h>

class Images {
public:
    Images();

    // 指定したパスから画像をロード
    void load(const String& filePath, const String& tag);
    void load(const String& filePath, const String& tag, const Color& transparent);

    // 外部からテクスチャを参照するためのマップ
    // images[tag][filename_without_ext]
    std::map<String, std::map<String, ImageStruct>> images;

private:
    // Rust側のカタログ管理オブジェクト
    rust::images::ImageCatalog *m_catalog;
};

#endif /* Images_hpp */
