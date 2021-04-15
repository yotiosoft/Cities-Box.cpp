//
//  OtherFunctions.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#ifndef OtherFunctions_hpp
#define OtherFunctions_hpp

#pragma once

#include "Header.hpp"

Array<string> splitUTF8(string str, string separator);
Array<String> split(String str, String separator);

void cMes(Font& font, String str, PositionStruct topLeft, PositionStruct bottomRight, Color& color);
void cMes(Font& font, String str, PositionStruct topLeft, Size size, Color& color);

PositionStruct getStringTopLeft(Font& font, String str, PositionStruct topLeft, PositionStruct bottomRight);
PositionStruct getStringTopLeft(Font& font, String str, PositionStruct topLeft, Size size);

void saveTextFile(string filePath, string str);
void addTextFile(string filePath, string str);
void saveTextFile(string filePath, Array<string> strV);
void saveTextFile(string filePath, Array<FileStruct> fs);
void saveTextFile(string filePath, FileStruct fs);

void debugInit();
void debugLog(String str);

string replaceString(string beforeStr, string searchStr, string afterStr);

RCOIFP::Type getRCOIFP(int number);
School::Type getSchool(int number);

string stringXOR(const std::string &data, const std::string &key);

TypeID::Type typeNameToTypeID(String type_name);
String typeIDToTypeName(TypeID::Type type_id);
DirectionID::Type directionNameToDirectionID(String direction_name);
String directionIDToDirectionName(DirectionID::Type direction_id);
Array<DirectionID::Type> directionNameToDirectionID(Array<String> direction_name);

DirectionID::Type getDirectionIDfromDifference(CoordinateStruct arg_before, CoordinateStruct arg_after);

LayerType::Type timeToLayerType(TimeStruct time);

LayerType::Type layerNameToLayerType(String layer_name);
String layerTypeToLayerName(LayerType::Type layer_type);

RateID::Type rateNameToRateID(String rate_name);
String rateIDToRateName(RateID::Type rate_id);

void blendColorAndImage(Image& imageTemp, Color blendColor);

void setAlphaColor(Image& imageTemp, Color transparentRGB);

#endif /* OtherFunctions_hpp */
