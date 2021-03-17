//
//  Tile.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#include "Tile.hpp"

// originalName
bool Tile::setOriginalName(String new_original_name) {
	m_original_name = new_original_name;
	return true;
}

String Tile::getOriginalName() {
	return m_original_name;
}

// types
bool Tile::setTypes(Array<String> new_types) {
	m_types = new_types;
	return true;
}

bool Tile::addType(String add_type) {
	m_types << add_type;
	return true;
}

bool Tile::addTypes(Array<String> add_types) {
	for (int i=0; i<add_types.size(); i++) {
		m_types << add_types[i];
	}
	return true;
}

Array<String> Tile::getTypes() {
	return m_types;
}

String Tile::getType(int num) {
	return m_types[num];
}

// directions
bool Tile::setDirections(Array<String> new_directions) {
	m_directions = new_directions;
	return true;
}

bool Tile::addDirection(String add_direction) {
	m_directions << add_direction;
	return true;
}

bool Tile::addDirections(Array<String> add_directions) {
	for (int i=0; i<add_directions.size(); i++) {
		m_directions << add_directions[i];
	}
	return true;
}

Array<String> Tile::getDirections() {
	return m_directions;
}

String Tile::getDirection(int num) {
	return m_directions[num];
}

void Tile::clearAddons() {
	addons.clear();
	m_types.clear();
	m_directions.clear();
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
