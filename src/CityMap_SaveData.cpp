//
//  CityMap_SaveData.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "CityMap.hpp"

bool CityMap::save() {
	// JsonWriterを宣言
	JSONWriter mapData;
	
	mapData.startObject();
	{
		mapData.key(U"Version").write(RELEASE_NUMBER);
		mapData.key(U"Addon_Set").write(m_addon_set_name);
		mapData.key(U"City_Name").write(m_city_name);
		mapData.key(U"Mayor_Name").write(m_mayor_name);
		mapData.key(U"Total_Population").write(m_total_population);
		mapData.key(U"Change_Weather").write(m_change_weather);
		mapData.key(U"Temperature").write(m_temperature);
		mapData.key(U"Dark_on_Night").write(m_dark_on_night);
		
		mapData.key(U"Map_size").startObject();
		{
			mapData.key(U"width").write(m_map_size.x);
			mapData.key(U"height").write(m_map_size.y);
		}
		mapData.endObject();
		
		mapData.key(U"Time").startObject();
		{
			mapData.key(U"year").write(m_time_now.year);
			mapData.key(U"month").write(m_time_now.month);
			mapData.key(U"date").write(m_time_now.date);
			mapData.key(U"hour").write(m_time_now.hour);
			mapData.key(U"minutes").write(m_time_now.minutes);
		}
		mapData.endObject();
		
		mapData.key(U"Demand").startObject();
		{
			mapData.key(U"residential").write(m_demand.residential);
			mapData.key(U"commercial").write(m_demand.commercial);
			mapData.key(U"office").write(m_demand.office);
			mapData.key(U"industrial").write(m_demand.industrial);
			mapData.key(U"farm").write(m_demand.farm);
		}
		mapData.endObject();
		
		mapData.key(U"Money").write(m_money);
		
		mapData.key(U"Budget").startObject();
		{
			mapData.key(U"police").write(m_budget.police);
			mapData.key(U"fire_depertment").write(m_budget.fireDepertment);
			mapData.key(U"post_office").write(m_budget.postOffice);
			mapData.key(U"education").write(m_budget.education);
		}
		mapData.endObject();
		
		mapData.key(U"Tax").startObject();
		{
			mapData.key(U"residential").write(m_tax.residential);
			mapData.key(U"commercial").write(m_tax.commercial);
			mapData.key(U"office").write(m_tax.office);
			mapData.key(U"industrial").write(m_tax.industrial);
			mapData.key(U"farm").write(m_tax.farm);
		}
		mapData.endObject();
		
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
		
		mapData.key(U"Objects").startArray();
		{
			for (auto object : m_objects) {
				mapData.startObject();
				{
					mapData.key(U"objectID").write(object.second->getObjectID());
					mapData.key(U"addon_name").write(object.second->getAddonName(NameMode::English));
					mapData.key(U"original_name").write(object.second->getOriginalName());
					mapData.key(U"typeID").write(UnitaryTools::typeIDToTypeName(object.second->getTypeID()));
					mapData.key(U"directionID").write(UnitaryTools::directionIDToDirectionName(object.second->getDirectionID()));
					mapData.key(U"origin_coordinate").startObject();
					{
						mapData.key(U"x").write(object.second->getOriginCoordinate().x);
						mapData.key(U"y").write(object.second->getOriginCoordinate().y);
					}
					mapData.endObject();
				}
				mapData.endObject();
			}
		}
		mapData.endArray();
		
		mapData.key(U"Map").startArray();
		{
			for (int y=0; y<m_map_size.y; y++) {
				mapData.startArray();
				{
					for (int x=0; x<m_map_size.x; x++) {
						mapData.startObject();
						{
							mapData.key(U"objects").startArray();
							{
								for (ObjectStruct object_struct : m_tiles[y][x].getObjectStructs()) {
									mapData.startObject();
									{
										mapData.key(U"objectID").write(object_struct.object_p->getObjectID());
										
										mapData.key(U"relative_coordinate").startObject();
										{
											mapData.key(U"x").write(object_struct.relative_coordinate.relative.x);
											mapData.key(U"y").write(object_struct.relative_coordinate.relative.y);
										}
										mapData.endObject();
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							
							mapData.key(U"residents").write(m_tiles[y][x].residents);
							
							mapData.key(U"workers").startObject();
							{
								mapData.key(U"commercial").write(m_tiles[y][x].workers.commercial);
								mapData.key(U"office").write(m_tiles[y][x].workers.office);
								mapData.key(U"industrial").write(m_tiles[y][x].workers.industrial);
								mapData.key(U"farm").write(m_tiles[y][x].workers.farm);
								mapData.key(U"public").write(m_tiles[y][x].workers.publicFacility);
							}
							mapData.endObject();
							
							mapData.key(U"students").write(m_tiles[y][x].students);
							
							mapData.key(U"rate").startObject();
							{
								for (auto rate = m_tiles[y][x].rate.begin(); rate != m_tiles[y][x].rate.end() ; rate++) {
									if (rate->second != 0) {
										mapData.key(UnitaryTools::rateIDToRateName(rate->first)).write(rate->second);
									}
								}
							}
							mapData.endObject();
							
							/*
							map_file.key(U"crop").startObject();
							{
								map_file.key(U"name").write(tiles[y][x].crop.name);
								map_file.key(U"amount").write(tiles[y][x].crop.amount);
							}
							map_file.endObject();
							 */
							
							mapData.key(U"age").startArray();
							{
								for (int i=0; i<m_tiles[y][x].age.size(); i++) {
									mapData.write(m_tiles[y][x].age[i]);
								}
							}
							mapData.endArray();
							
							mapData.key(U"gender").startArray();
							{
								for (int i=0; i<m_tiles[y][x].gender.size(); i++) {
									mapData.write(m_tiles[y][x].gender[i]);
								}
							}
							mapData.endArray();
							
							mapData.key(U"work_places").startArray();
							{
								for (int i=0; i<m_tiles[y][x].workPlaces.size(); i++) {
									mapData.startObject();
									{
										mapData.key(U"work_kind").write(m_tiles[y][x].workPlaces[i].workPlace);
										mapData.key(U"serial_number").write(m_tiles[y][x].workPlaces[i].workPlacesSerialNumber);
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							
							mapData.key(U"school").startArray();
							{
								for (int i=0; i<m_tiles[y][x].schools.size(); i++) {
									mapData.startObject();
									{
										mapData.key(U"school_kind").write(m_tiles[y][x].schools[i].school);
										mapData.key(U"serial_number").write(m_tiles[y][x].schools[i].schoolSerialNumber);
									}
									mapData.endObject();
								}
							}
							mapData.endArray();
							
							mapData.key(U"reservation").write(m_tiles[y][x].reservation);
							
							mapData.key(U"original_name").write(m_tiles[y][x].getOriginalName());
						}
						mapData.endObject();
					}
				}
				mapData.endArray();
			}
		}
		mapData.endArray();
	}
	mapData.endObject();
	
	UnitaryTools::saveTextFile(m_map_file_path.toUTF8(), UnitaryTools::stringXOR(mapData.get().toUTF8(), "citiesboxmapdatafilexor"));
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
