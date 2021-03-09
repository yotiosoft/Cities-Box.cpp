//
//  Tile.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#include "Tile.hpp"

// originalName
bool Tile::setOriginalName(String new_original_name) {
	originalName = new_original_name;
	return true;
}

String Tile::getOriginalName() {
	return originalName;
}

// types
bool Tile::setTypes(Array<String> new_types) {
	types = new_types;
	return true;
}

bool Tile::addType(String add_type) {
	types << add_type;
	return true;
}

bool Tile::addTypes(Array<String> add_types) {
	for (int i=0; i<add_types.size(); i++) {
		types << add_types[i];
	}
	return true;
}

Array<String> Tile::getTypes() {
	return types;
}

String Tile::getType(int num) {
	return types[num];
}

// directions
bool Tile::setDirections(Array<String> new_directions) {
	directions = new_directions;
	return true;
}

bool Tile::addDirection(String add_direction) {
	directions << add_direction;
	return true;
}

bool Tile::addDirections(Array<String> add_directions) {
	for (int i=0; i<add_directions.size(); i++) {
		directions << add_directions[i];
	}
	return true;
}

Array<String> Tile::getDirections() {
	return directions;
}

String Tile::getDirection(int num) {
	return directions[num];
}

void Tile::clearAddons() {
	addons.clear();
	types.clear();
	directions.clear();
}

void Tile::clearAll() {
	clearAddons();
	
	serialNumber = 0;
	tilesCount = {0, 0};
	residents = 0;
	workers = {0, 0, 0, 0, 0};
	students = 0;
	reservation = RCOIFP::None;
}
