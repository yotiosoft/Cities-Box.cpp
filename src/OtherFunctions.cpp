//
//  OtherFunctions.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "OtherFunctions.hpp"

Array<string> splitUTF8(string str, string separator) {
	auto separatorLength = separator.length();
	
	Array<string> list = Array<string>();
	
	if (separatorLength == 0) {
		list.push_back(str);
	} else {
		auto offset = std::string::size_type(0);
		while (1) {
			auto pos = str.find(separator, offset);
			if (pos == std::string::npos) {
				list.push_back(str.substr(offset));
				break;
			}
			list.push_back(str.substr(offset, pos - offset));
			offset = pos + separatorLength;
		}
	}
	
	return list;
}
Array<String> split(String str, String separator) {
	string strUTF8 = str.toUTF8();
	string separatorUTF8 = separator.toUTF8();
	
	auto separatorLength = separator.length();
	
	Array<String> list = Array<String>();
	
	if (separatorLength == 0) {
		list.push_back(str);
	} else {
		auto offset = std::string::size_type(0);
		while (1) {
			auto pos = strUTF8.find(separatorUTF8, offset);
			if (pos == std::string::npos) {
				list.push_back(Unicode::Widen(strUTF8.substr(offset)));
				break;
			}
			list.push_back(Unicode::Widen(strUTF8.substr(offset, pos - offset)));
			offset = pos + separatorLength;
		}
	}
	
	return list;
}

void cMes(Font& font, String str, PositionStruct topLeft, PositionStruct bottomRight, Color& color) {
	PositionStruct str_long = getStringTopLeft(font, str, topLeft, bottomRight);
	font(str).draw(str_long.x, str_long.y, color);
}
void cMes(Font& font, String str, PositionStruct topLeft, Size size, Color& color) {
	PositionStruct str_long = getStringTopLeft(font, str, topLeft, size);
	font(str).draw(str_long.x, str_long.y, color);
}

PositionStruct getStringTopLeft(Font& font, String str, PositionStruct topLeft, PositionStruct bottomRight) {
	PositionStruct retSize;
	retSize.x = (bottomRight.x + topLeft.x) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).w / 2;
	retSize.y = (bottomRight.y + topLeft.y) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).h / 2;
	return retSize;
}
PositionStruct getStringTopLeft(Font& font, String str, PositionStruct topLeft, Size size) {
	PositionStruct retSize;
	retSize.x = (topLeft.x * 2 + size.x) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).w / 2;
	retSize.y = (topLeft.y * 2 + size.y) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).h / 2;
	return retSize;
}

void saveTextFile(string filePath, string str) {
	ofstream ofs;
	ofs.open(filePath, ios::out | ios::binary);
	
	ofs << str;
}

void saveTextFile(string filePath, Array<string> strV) {
	ofstream ofs;
	ofs.open(filePath, ios::out | ios::binary);
	
	for (int i = 0; i < strV.size(); i++) {
		ofs << strV[i] << endl;
	}
}

void saveTextFile(string filePath, Array<FileStruct> fs) {
	ofstream ofs;
	ofs.open(filePath, ios::out | ios::binary);
	
	for (int i = 0; i < fs.size(); i++) {
		ofs << fs[i].file_path << endl;
		ofs << "    file_name   : " << fs[i].file_name << endl;
		ofs << "    folder_path : " << fs[i].folder_path << endl;
		ofs << "    folder_name : " << fs[i].folder_name << endl;
	}
}

void saveTextFile(string filePath, FileStruct fs) {
	ofstream ofs;
	ofs.open(filePath, ios::out | ios::binary);
	
	ofs << fs.file_path << endl;
	ofs << "    file_name   : " << fs.file_name << endl;
	ofs << "    folder_path : " << fs.folder_path << endl;
	ofs << "    folder_name : " << fs.folder_name << endl;
}

string replaceString(string beforeStr, string searchStr, string afterStr) {
	string::size_type pos(beforeStr.find(searchStr));
	
	while (pos != string::npos) {
		beforeStr.replace(pos, searchStr.length(), afterStr);
		pos = beforeStr.find(searchStr, pos + afterStr.length());
	}
	
	return beforeStr;
}

RCOIFP::Type getRCOIFP(int number) {
	switch (number) {
		case 0:
			return RCOIFP::Residential;
		case 1:
			return RCOIFP::Commercial;
		case 2:
			return RCOIFP::Office;
		case 3:
			return RCOIFP::Industrial;
		case 4:
			return RCOIFP::Farm;
		case 5:
			return RCOIFP::Public;
		default:
			return RCOIFP::None;
	}
}

School::Type getSchool(int number) {
	switch (number) {
		case 0:
			return School::ElementarySchool;
		case 1:
			return School::JuniorHighSchool;
		case 2:
			return School::HighSchool;
		case 3:
			return School::University;
		default:
			return School::None;
	}
}

string stringXOR(const std::string &data, const std::string &key) {
	std::string result(data.size(), '\0');
	for (std::size_t i = 0; i < data.size(); i++) {
		result[i] = data[i] ^ key[i % key.size()];
	}
	return result;
}

TypeID::Type typeNameToTypeID(String type_name) {
	if (type_name == U"normal") {
		return TypeID::Normal;
	}
	if (type_name == U"under_construction") {
		return TypeID::UnderConstruction;
	}
	if (type_name == U"default") {
		return TypeID::Default;
	}
	if (type_name == U"turn") {
		return TypeID::Turn;
	}
	if (type_name == U"intersection_T") {
		return TypeID::IntersectionT;
	}
	if (type_name == U"intersection_cross") {
		return TypeID::IntersectionCross;
	}
	if (type_name == U"dead_end") {
		return TypeID::DeadEnd;
	}
	if (type_name == U"train_crossing") {
		return TypeID::TrainCrossing;
	}
	if (type_name == U"bridge") {
		return TypeID::Bridge;
	}
	if (type_name == U"autumn") {
		return TypeID::TileAutumn;
	}
	if (type_name == U"snow") {
		return TypeID::TileSnow;
	}
	if (type_name == U"estuary") {
		return TypeID::WaterEstuary;
	}
	if (type_name == U"intersection_cross_without_one_corner") {
		return TypeID::WaterIntersectionCrossWithoutOneCorner;
	}
	if (type_name == U"isolated") {
		return TypeID::WaterIsolated;
	}
	if (type_name == U"river_dead_end") {
		return TypeID::RiverDeadEnd;
	}
	if (type_name == U"river_default") {
		return TypeID::RiverDefault;
	}
	if (type_name == U"river_intersection_T") {
		return TypeID::RiverIntersectionT;
	}
	if (type_name == U"river_intersection_cross") {
		return TypeID::RiverIntersectionCross;
	}
	if (type_name == U"river_turn") {
		return TypeID::RiverTurn;
	}
	
	// 定義に該当しない場合は無効に
	return TypeID::Disabled;
}

DirectionID::Type directionNameToDirectionID(String direction_name) {
	/*
	 namespace DirectionID {
		 enum Type {
			 Normal			= 0,
			 Left			= 1,
			 Top				= 2,
			 Right			= 3,
			 Bottom			= 4,
			 Depth			= 5,
			 Width			= 6,
			 LeftRightBottom	= 7,
			 LeftRightTop	= 8,
			 LeftTopBottom	= 9,
			 RightTopBottom	= 10,
			 LeftBottom		= 11,
			 LeftTop			= 12,
			 RightBottom		= 13,
			 RightTop		= 14,
		 };
	 }
	 */
	
	if (direction_name == U"normal") {
		return DirectionID::Normal;
	}
	if (direction_name == U"left") {
		return DirectionID::Left;
	}
	if (direction_name == U"top") {
		return DirectionID::Top;
	}
	if (direction_name == U"right") {
		return DirectionID::Right;
	}
	if (direction_name == U"bottom") {
		return DirectionID::Bottom;
	}
	if (direction_name == U"depth") {
		return DirectionID::Depth;
	}
	if (direction_name == U"width") {
		return DirectionID::Width;
	}
	if (direction_name == U"left-right-bottom") {
		return DirectionID::LeftRightBottom;
	}
	if (direction_name == U"left-right-top") {
		return DirectionID::LeftRightTop;
	}
	if (direction_name == U"left-top-bottom") {
		return DirectionID::LeftTopBottom;
	}
	if (direction_name == U"right-top-bottom") {
		return DirectionID::RightTopBottom;
	}
	if (direction_name == U"left-bottom") {
		return DirectionID::LeftBottom;
	}
	if (direction_name == U"left-top") {
		return DirectionID::LeftTop;
	}
	if (direction_name == U"right-bottom") {
		return DirectionID::RightBottom;
	}
	if (direction_name == U"right-top") {
		return DirectionID::RightTop;
	}
	
	return DirectionID::Disabled;
}

Array<DirectionID::Type> directionNameToDirectionID(Array<String> direction_name) {
	Array<DirectionID::Type> ret_array;
	for (int i=0; i<direction_name.size(); i++) {
		ret_array << directionNameToDirectionID(direction_name[i]);
	}
	return ret_array;
}

LayerType::Type timeToLayerType(TimeStruct time) {
	int layer_type_int = (int)LayerType::Normal;
	
	if (time.month >= 3 && time.month <= 5) {
		layer_type_int = (int)LayerType::OnSpring;
	}
	if (time.month >= 6 && time.month <= 8) {
		layer_type_int = (int)LayerType::OnSummer;
	}
	if (time.month >= 9 && time.month <= 11) {
		layer_type_int = (int)LayerType::OnAutumn;
	}
	if (time.month == 12 || time.month == 1 || time.month == 2) {
		layer_type_int = (int)LayerType::OnWinter;
	}
	
	if ((time.hour >= 16 && time.hour <= 17) || time.hour == 6) {
		layer_type_int += (int)LayerType::Evening;
	} else if ((time.hour >= 18 && time.hour <= 23) || (time.hour >= 0 && time.hour <= 5)) {
		layer_type_int += (int)LayerType::Night;
	}
	
	return (LayerType::Type)layer_type_int;
}

void blendColorAndImage(Image& imageTemp, Color blendColor) {
	double outA, outR, outG, outB;
	for (int h=0; h<imageTemp.height(); h++) {
		for (int w=0; w<imageTemp.width(); w++) {
			double bcA = blendColor.a / 255.0;
			double itA = imageTemp[h][w].a / 255.0;
			
			outA = bcA+itA*(1.0-bcA);
			if (outA < 1.0) {
				imageTemp[h][w].a = 0;
				imageTemp[h][w].r = 0;
				imageTemp[h][w].g = 0;
				imageTemp[h][w].b = 0;
			}
			else {
				outR = (blendColor.r*bcA+imageTemp[h][w].r*itA*(1.0-bcA))/outA;
				outG = (blendColor.g*bcA+imageTemp[h][w].g*itA*(1.0-bcA))/outA;
				outB = (blendColor.b*bcA+imageTemp[h][w].b*itA*(1.0-bcA))/outA;
				
				imageTemp[h][w].a = outA*255;
				imageTemp[h][w].r = outR;
				imageTemp[h][w].g = outG;
				imageTemp[h][w].b = outB;
			}
		}
	}
}
