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

void addTextFile(string filePath, string str) {
	ofstream ofs;
	ofs.open(filePath, ios::out | ios::binary | ios::app);
	
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

void debugInit() {
	saveTextFile("log.txt", "");;
}

void debugLog(String str) {
	addTextFile("log.txt", str.toUTF8()+"\n");;
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
	if (type_name == U"Normal" || type_name == U"normal") {
		return TypeID::Normal;
	}
	if (type_name == U"UnderConstruction" || type_name == U"under_construction") {
		return TypeID::UnderConstruction;
	}
	if (type_name == U"Default" || type_name == U"default") {
		return TypeID::Default;
	}
	if (type_name == U"Turn" || type_name == U"turn") {
		return TypeID::Turn;
	}
	if (type_name == U"IntersectionT" || type_name == U"intersection_T") {
		return TypeID::IntersectionT;
	}
	if (type_name == U"IntersectionCross" || type_name == U"intersection_cross") {
		return TypeID::IntersectionCross;
	}
	if (type_name == U"DeadEnd" || type_name == U"dead_end") {
		return TypeID::DeadEnd;
	}
	if (type_name == U"TrainCrossing" || type_name == U"train_crossing") {
		return TypeID::TrainCrossing;
	}
	if (type_name == U"Bridge" || type_name == U"bridge") {
		return TypeID::Bridge;
	}
	if (type_name == U"TileAutumn" || type_name == U"autumn") {
		return TypeID::TileAutumn;
	}
	if (type_name == U"TileSnow" || type_name == U"snow") {
		return TypeID::TileSnow;
	}
	if (type_name == U"WaterEstuary" || type_name == U"estuary") {
		return TypeID::WaterEstuary;
	}
	if (type_name == U"WaterIntersectionCrossWithoutOneCorner" || type_name == U"intersection_cross_without_one_corner") {
		return TypeID::WaterIntersectionCrossWithoutOneCorner;
	}
	if (type_name == U"WaterIsolated" || type_name == U"isolated") {
		return TypeID::WaterIsolated;
	}
	if (type_name == U"RiverDeadEnd" || type_name == U"river_dead_end") {
		return TypeID::RiverDeadEnd;
	}
	if (type_name == U"RiverDefault" || type_name == U"river_default") {
		return TypeID::RiverDefault;
	}
	if (type_name == U"RiverIntersectionT" || type_name == U"river_intersection_T") {
		return TypeID::RiverIntersectionT;
	}
	if (type_name == U"RiverIntersectionCross" || type_name == U"river_intersection_cross") {
		return TypeID::RiverIntersectionCross;
	}
	if (type_name == U"RiverTurn" || type_name == U"river_turn") {
		return TypeID::RiverTurn;
	}
	
	// 定義に該当しない場合は無効に
	return TypeID::Disabled;
}

String typeIDToTypeName(TypeID::Type type_id) {
	switch (type_id) {
		case TypeID::Normal:
			return U"Normal";
		case TypeID::UnderConstruction:
			return U"UnderConstruction";
		case TypeID::Default:
			return U"Default";
		case TypeID::Turn:
			return U"Turn";
		case TypeID::IntersectionT:
			return U"IntersectionT";
		case TypeID::IntersectionCross:
			return U"IntersectionCross";
		case TypeID::DeadEnd:
			return U"DeadEnd";
		case TypeID::TrainCrossing:
			return U"TrainCrossing";
		case TypeID::Bridge:
			return U"Bridge";
		case TypeID::TileAutumn:
			return U"TileAutumn";
		case TypeID::TileSnow:
			return U"TileSnow";
		case TypeID::WaterEstuary:
			return U"WaterEstuary";
		case TypeID::WaterIntersectionCrossWithoutOneCorner:
			return U"WaterIntersectionCrossWithoutOneCorner";
		case TypeID::WaterIsolated:
			return U"WaterIsolated";
		case TypeID::RiverDeadEnd:
			return U"RiverDeadEnd";
		case TypeID::RiverDefault:
			return U"RiverDefault";
		case TypeID::RiverIntersectionT:
			return U"RiverIntersectionT";
		case TypeID::RiverIntersectionCross:
			return U"RiverIntersectionCross";
		case TypeID::RiverTurn:
			return U"RiverTurn";
		case TypeID::Disabled:
			break;
	}
	
	return U"Disabled";
}

DirectionID::Type directionNameToDirectionID(String direction_name) {
	if (direction_name == U"None" || direction_name == U"normal") {
		return DirectionID::None;
	}
	if (direction_name == U"West" || direction_name == U"left") {
		return DirectionID::West;
	}
	if (direction_name == U"North" || direction_name == U"top") {
		return DirectionID::North;
	}
	if (direction_name == U"East" || direction_name == U"right") {
		return DirectionID::East;
	}
	if (direction_name == U"South" || direction_name == U"bottom") {
		return DirectionID::South;
	}
	if (direction_name == U"EastWest" || direction_name == U"depth") {
		return DirectionID::EastWest;
	}
	if (direction_name == U"NorthSouth" || direction_name == U"width") {
		return DirectionID::NorthSouth;
	}
	if (direction_name == U"SouthEastWest" || direction_name == U"left-right-bottom") {
		return DirectionID::SouthEastWest;
	}
	if (direction_name == U"NorthEastWest" || direction_name == U"left-right-top") {
		return DirectionID::NorthEastWest;
	}
	if (direction_name == U"NorthSouthWest" || direction_name == U"left-top-bottom") {
		return DirectionID::NorthSouthWest;
	}
	if (direction_name == U"NorthSouthEast" || direction_name == U"right-top-bottom") {
		return DirectionID::NorthSouthEast;
	}
	if (direction_name == U"SouthWest" || direction_name == U"left-bottom") {
		return DirectionID::SouthWest;
	}
	if (direction_name == U"NorthWest" || direction_name == U"left-top") {
		return DirectionID::NorthWest;
	}
	if (direction_name == U"SouthEast" || direction_name == U"right-bottom") {
		return DirectionID::SouthEast;
	}
	if (direction_name == U"NorthEast" || direction_name == U"right-top") {
		return DirectionID::NorthEast;
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

String directionIDToDirectionName(DirectionID::Type direction_id) {
	switch (direction_id) {
		case DirectionID::None:
			return U"None";
		case DirectionID::West:
			return U"West";
		case DirectionID::North:
			return U"North";
		case DirectionID::East:
			return U"East";
		case DirectionID::South:
			return U"South";
		case DirectionID::EastWest:
			return U"EastWest";
		case DirectionID::NorthSouth:
			return U"NorthSouth";
		case DirectionID::SouthEastWest:
			return U"SouthEastWest";
		case DirectionID::NorthEastWest:
			return U"NorthEastWest";
		case DirectionID::NorthSouthWest:
			return U"NorthSouthWest";
		case DirectionID::NorthSouthEast:
			return U"NorthSouthEast";
		case DirectionID::SouthWest:
			return U"SouthWest";
		case DirectionID::NorthWest:
			return U"NorthWest";
		case DirectionID::SouthEast:
			return U"SouthEast";
		case DirectionID::NorthEast:
			return U"NorthEast";
		case DirectionID::Disabled:
			break;
	}
	
	return U"disabled";
}

DirectionID::Type getDirectionIDfromDifference(CoordinateStruct arg_before, CoordinateStruct arg_after) {
	int dx = arg_after.x - arg_before.x;
	int dy = arg_after.y - arg_before.y;
	
	if (dx == 0 && dy == 0) {
		return DirectionID::None;
	}
	if (dx == -1 && dy == 0) {
		return DirectionID::West;
	}
	if (dx == 1 && dy == 0) {
		return DirectionID::East;
	}
	if (dx == 0 && dy == -1) {
		return DirectionID::North;
	}
	if (dx == 0 && dy == 1) {
		return DirectionID::South;
	}
	if (dx == -1 && dy == -1) {
		return DirectionID::NorthWest;
	}
	if (dx == 1 && dy == -1) {
		return DirectionID::NorthEast;
	}
	if (dx == 1 && dy == 1) {
		return DirectionID::SouthEast;
	}
	if (dx == -1 && dy == 1) {
		return DirectionID::SouthWest;
	}
	
	return DirectionID::Disabled;
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

LayerType::Type layerNameToLayerType(String layer_name) {
	if (layer_name == U"Ground") {
		return LayerType::Ground;
	}
	if (layer_name == U"Normal") {
		return LayerType::Normal;
	}
	if (layer_name == U"OnSpring") {
		return LayerType::OnSpring;
	}
	if (layer_name == U"OnSummer") {
		return LayerType::OnSummer;
	}
	if (layer_name == U"OnAutumn") {
		return LayerType::OnAutumn;
	}
	if (layer_name == U"OnWinter") {
		return LayerType::OnWinter;
	}
	if (layer_name == U"Evening") {
		return LayerType::Evening;
	}
	if (layer_name == U"OnSpringEvening") {
		return LayerType::OnSpringEvening;
	}
	if (layer_name == U"OnSummerEvening") {
		return LayerType::OnSummerEvening;
	}
	if (layer_name == U"OnAutumnEvening") {
		return LayerType::OnAutumnEvening;
	}
	if (layer_name == U"OnWinterEvening") {
		return LayerType::OnWinterEvening;
	}
	if (layer_name == U"Night") {
		return LayerType::Night;
	}
	if (layer_name == U"OnSpringNight") {
		return LayerType::OnSpringNight;
	}
	if (layer_name == U"OnSummerNight") {
		return LayerType::OnSummerNight;
	}
	if (layer_name == U"OnAutumnNight") {
		return LayerType::OnAutumnNight;
	}
	if (layer_name == U"OnWinterNight") {
		return LayerType::OnWinterNight;
	}
	
	return LayerType::Disabled;
}

String layerTypeToLayerName(LayerType::Type layer_type) {
	switch (layer_type) {
		case LayerType::Ground:
			return U"Ground";
		case LayerType::Normal:
			return U"Normal";
		case LayerType::OnSpring:
			return U"OnSpring";
		case LayerType::OnSummer:
			return U"OnSummer";
		case LayerType::OnAutumn:
			return U"OnAutumn";
		case LayerType::OnWinter:
			return U"OnWinter";
		case LayerType::Evening:
			return U"Evening";
		case LayerType::OnSpringEvening:
			return U"OnSpringEvening";
		case LayerType::OnSummerEvening:
			return U"OnSummerEvening";
		case LayerType::OnAutumnEvening:
			return U"OnAutumnEvening";
		case LayerType::OnWinterEvening:
			return U"OnWinterEvening";
		case LayerType::Night:
			return U"Night";
		case LayerType::OnSpringNight:
			return U"OnSpringNight";
		case LayerType::OnSummerNight:
			return U"OnSummerNight";
		case LayerType::OnAutumnNight:
			return U"OnAutumnNight";
		case LayerType::OnWinterNight:
			return U"OnWinterNight";
		case LayerType::Disabled:
			break;
	}
	
	return U"disabled";
}

RateID::Type rateNameToRateID(String rate_name) {
	if (rate_name == U"LandPrice" || rate_name == U"land_price") {
		return RateID::LandPrice;
	}
	if (rate_name == U"CrimeRate" || rate_name == U"crime_rate") {
		return RateID::CrimeRate;
	}
	if (rate_name == U"EducationRate" || rate_name == U"education_rate") {
		return RateID::EducationRate;
	}
	if (rate_name == U"HappinessRate" || rate_name == U"happiness_rate") {
		return RateID::HappinessRate;
	}
	
	return RateID::None;
}

String rateIDToRateName(RateID::Type rate_id) {
	switch (rate_id) {
		case RateID::LandPrice:
			return U"LandPrice";
		case RateID::CrimeRate:
			return U"CrimeRate";
		case RateID::EducationRate:
			return U"EducationRate";
		case RateID::HappinessRate:
			return U"HappinessRate";
		default:
			break;
	}
	
	return U"None";
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

void setAlphaColor(Image& imageTemp, Color transparentRGB) {
	for (int h=0; h<imageTemp.height(); h++) {
		for (int w=0; w<imageTemp.width(); w++) {
			if (imageTemp[h][w].r == transparentRGB.r && imageTemp[h][w].g == transparentRGB.g && imageTemp[h][w].b == transparentRGB.b) {
				imageTemp[h][w].a = 0;
			}
		}
	}
}
