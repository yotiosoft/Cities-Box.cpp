//
//  CityMap_SaveData.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "CityMap.hpp"

bool CityMap::save() {
	// JsonWriterを宣言
	JSON mapData;
	
	mapData[U"Version"] = RELEASE_NUMBER;
	mapData[U"Addon_Set"] = m_addon_set_name;
	mapData[U"City_Name"] = m_city_name;
	mapData[U"Mayor_Name"] = m_mayor_name;
	mapData[U"Total_Population"] = m_total_population;
	mapData[U"Change_Weather"] = m_change_weather;
	mapData[U"Temperature"] = m_temperature;
	mapData[U"Dark_on_Night"] = m_dark_on_night;
	
	mapData[U"Map_size"][U"width"] = m_map_size.x;
	mapData[U"Map_size"][U"height"] = m_map_size.y;
	
	mapData[U"Time"][U"year"] = m_time_now.year;
	mapData[U"Time"][U"month"] = m_time_now.month;
	mapData[U"Time"][U"date"] = m_time_now.date;
	mapData[U"Time"][U"hour"] = m_time_now.hour;
	mapData[U"Time"][U"minutes"] = m_time_now.minutes;
	
	mapData[U"Demand"][U"residential"] = m_demand.residential;
	mapData[U"Demand"][U"commercial"] = m_demand.commercial;
	mapData[U"Demand"][U"office"] = m_demand.office;
	mapData[U"Demand"][U"industrial"] = m_demand.industrial;
	mapData[U"Demand"][U"farm"] = m_demand.farm;
	
	mapData[U"Money"] = m_money;
	
	mapData[U"Budget"][U"police"] = m_budget.police;
	mapData[U"Budget"][U"fire_depertment"] = m_budget.fireDepertment;
	mapData[U"Budget"][U"post_office"] = m_budget.postOffice;
	mapData[U"Budget"][U"education"] = m_budget.education;
	
	mapData[U"Tax"][U"residential"] = m_tax.residential;
	mapData[U"Tax"][U"commercial"] = m_tax.commercial;
	mapData[U"Tax"][U"office"] = m_tax.office;
	mapData[U"Tax"][U"industrial"] = m_tax.industrial;
	mapData[U"Tax"][U"farm"] = m_tax.farm;
		
	/*
	 // オブジェクトID
	 int object_id = object[U"objectID"].get<int>();
	 
	 // アドオン名
	 String addon_name = object[U"addon_name"].getString();
	 
	 // 固有名称
	 String original_name = object[U"original_name"].getString();
	 
	 // TypeID
	 TypeID::Type type_id = typeNameToTypeID(object[U"typeID"].getString());
	 
	 // DirectionID
	 DirectionID::Type direction_id = directionNameToDirectionID(object[U"directionID"].getString());
	 
	 // 原点
	 CoordinateStruct origin_coordinate;
	 origin_coordinate.x = object[U"origin_coordinate.x"].get<int>();
	 origin_coordinate.y = object[U"origin_coordinate.y"].get<int>();
	 */
	
	Array<JSON> json_objects_array;
	for (auto object : m_objects) {
		JSON json_object;
		
		json_object[U"objectID"] = object.second->getObjectID();
		json_object[U"addon_name"] = object.second->getAddonName(NameMode::English);
		json_object[U"original_name"] = object.second->getOriginalName();
		json_object[U"typeID"] = UnitaryTools::typeIDToTypeName(object.second->getTypeID());
		json_object[U"directionID"] = UnitaryTools::directionIDToDirectionName(object.second->getDirectionID());
		
		json_object[U"origin_coordinate"][U"x"] = object.second->getOriginCoordinate().x;
		json_object[U"origin_coordinate"][U"y"] = object.second->getOriginCoordinate().y;
		
		json_objects_array << json_object;
	}
	mapData[U"Objects"] = json_objects_array;
	
	Array<JSON> json_map;
	for (int y=0; y<m_map_size.y; y++) {
		Array<JSON> json_map_y;
		for (int x=0; x<m_map_size.x; x++) {
			JSON json_map_x;
			
			Array<JSON> json_map_x_objects;
			for (ObjectStruct object_struct : m_tiles[y][x].getObjectStructs()) {
				JSON json_map_x_object;
				
				json_map_x_object[U"objectID"] = object_struct.object_p->getObjectID();
				
				json_map_x_object[U"relative_coordinate"][U"x"] = object_struct.relative_coordinate.relative.x;
				json_map_x_object[U"relative_coordinate"][U"y"] = object_struct.relative_coordinate.relative.y;
				
				json_map_x_objects << json_map_x_object;
			}
			json_map_x[U"objects"] = json_map_x_objects;
			
			json_map_x[U"residents"] = m_tiles[y][x].residents;
			
			json_map_x[U"workers"][U"commercial"] = m_tiles[y][x].workers.commercial;
			json_map_x[U"workers"][U"office"] = m_tiles[y][x].workers.office;
			json_map_x[U"workers"][U"industrial"] = m_tiles[y][x].workers.industrial;
			json_map_x[U"workers"][U"farm"] = m_tiles[y][x].workers.farm;
			json_map_x[U"workers"][U"public"] = m_tiles[y][x].workers.publicFacility;
			
			json_map_x[U"students"] = m_tiles[y][x].students;
			
			for (auto rate = m_tiles[y][x].rate.begin(); rate != m_tiles[y][x].rate.end() ; rate++) {
				if (rate->second != 0) {
					json_map_x[U"rate"][UnitaryTools::rateIDToRateName(rate->first)] = rate->second;
				}
			}
			
			/*
			map_file.key(U"crop").startObject();
			{
				map_file.key(U"name").write(tiles[y][x].crop.name);
				map_file.key(U"amount").write(tiles[y][x].crop.amount);
			}
			map_file.endObject();
			 */
			
			Array<JSON> json_map_x_ages_array;
			for (int i=0; i<m_tiles[y][x].age.size(); i++) {
				JSON json_map_x_age = m_tiles[y][x].age[i];
				json_map_x_ages_array << json_map_x_age;
			}
			json_map_x[U"age"] = json_map_x_ages_array;
			
			Array<JSON> json_map_x_genders_array;
			for (int i=0; i<m_tiles[y][x].gender.size(); i++) {
				JSON json_map_x_gender = m_tiles[y][x].gender[i];
				json_map_x_genders_array << json_map_x_gender;
			}
			json_map_x[U"gender"] = json_map_x_genders_array;
			
			Array<JSON> json_map_x_work_places_array;
			for (int i=0; i<m_tiles[y][x].workPlaces.size(); i++) {
				JSON json_map_x_work_places;
				json_map_x_work_places[U"work_kind"] = m_tiles[y][x].workPlaces[i].workPlace;
				json_map_x_work_places[U"serial_number"] = m_tiles[y][x].workPlaces[i].workPlacesSerialNumber;
				
				json_map_x_work_places_array << json_map_x_work_places;
			}
			json_map_x[U"work_places"] = json_map_x_work_places_array;
			
			Array<JSON> json_schools_array;
			for (int i=0; i<m_tiles[y][x].schools.size(); i++) {
				JSON json_school;
				json_school[U"school_kind"] = m_tiles[y][x].schools[i].school;
				json_school[U"serial_number"] = m_tiles[y][x].schools[i].schoolSerialNumber;
				json_schools_array << json_school;
			}
			json_map_x[U"school"] = json_schools_array;
			
			json_map_x[U"reservation"] = m_tiles[y][x].reservation;
			
			json_map_x[U"original_name"] = m_tiles[y][x].getOriginalName();
			
			json_map_y << json_map_x;
		}
		
		json_map << json_map_y;
	}
	mapData[U"Map"] = json_map;
	
	UnitaryTools::saveTextFile(m_map_file_path.toUTF8(), UnitaryTools::stringXOR(mapData.formatUTF8(), "citiesboxmapdatafilexor"));
	//saveTextFile(map_file_path.toUTF8()+".cbj", map_file.get().toUTF8());
	//map_file.save(map_file_path+U".cbj");
	
	return true;
}

void CityMap::freeMapAndAddons() {
	for (auto i = m_addons.begin(); i != m_addons.end() ; i++) {
		delete(i->second);
	}
	
	for (auto i = m_objects.begin(); i != m_objects.end() ; i++) {
		delete(i->second);
	}
	
	Array<Array<Tile>>().swap(m_tiles);
}
