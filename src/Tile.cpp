//
//  Tile.cpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/02/28.
//

#include "Tile.hpp"

// オブジェクトの登録
void Tile::addObject(Object *arg_object_p, RelativeCoordinateStruct arg_relative_coordinate) {
	ObjectStruct new_object_struct;
	new_object_struct.object_p = arg_object_p;
	new_object_struct.relative_coordinate = arg_relative_coordinate;
	
	m_objects << new_object_struct;
}

// オブジェクトの建設
void Tile::buildObject(Object* arg_object_p, RelativeCoordinateStruct arg_relative_coordinate) {
	clearAll();
	addObject(arg_object_p, arg_relative_coordinate);
}

// originalName
bool Tile::setOriginalName(String new_original_name) {
	m_original_name = new_original_name;
	return true;
}

String Tile::getOriginalName() {
	return m_original_name;
}

// types
bool Tile::setTypes(Array<TypeID::Type> new_types) {
	m_types = new_types;
	return true;
}

bool Tile::addType(TypeID::Type add_type) {
	m_types << add_type;
	return true;
}

bool Tile::addTypes(Array<TypeID::Type> add_types) {
	for (int i=0; i<(int)add_types.size(); i++) {
		m_types << add_types[i];
	}
	return true;
}

Array<TypeID::Type> Tile::getTypes() {
	return m_types;
}

TypeID::Type Tile::getType(int num) {
	return m_types[num];
}

// directions
bool Tile::setDirections(Array<DirectionID::Type> new_directions) {
	m_directions = new_directions;
	return true;
}

bool Tile::addDirection(DirectionID::Type add_direction) {
	m_directions << add_direction;
	return true;
}

bool Tile::addDirections(Array<DirectionID::Type> add_directions) {
	for (int i=0; i<(int)add_directions.size(); i++) {
		m_directions << add_directions[i];
	}
	return true;
}

Array<DirectionID::Type> Tile::getDirections() {
	return m_directions;
}

DirectionID::Type Tile::getDirection(int num) {
	return m_directions[num];
}

// レートの設定
void Tile::setTileRate(RateID::Type arg_rate_type, int arg_rate) {
	rate[arg_rate_type] += arg_rate;
}

// レートの取得
map<RateID::Type, int> Tile::getRate() {
	return rate;
}

int Tile::getRate(RateID::Type rate_id) {
	if (rate.find(rate_id) != rate.end()) {
		return rate[rate_id];
	}
	else {
		return 0;
	}
}
// 引数のアドオンがタイル上に存在するか？
bool Tile::hasAddon(CBAddon* addon_p) {
	for (auto object : m_objects) {
		if (object.object_p->getAddonP() == addon_p) {
			return true;
		}
	}
	
	return false;
}

// 引数のカテゴリに該当するアドオンがタイル上に存在するか？
Object* Tile::hasCategory(CategoryID::Type category) {
    for (auto object : m_objects) {
        if (object.object_p->getAddonP()->isInCategories(category)) {
            return object.object_p;
        }
    }
    return nullptr;
}

// Object情報を取得
bool Tile::isObjectExists(int arg_object_id) {
	bool ret = false;
	
	for (int i=0; i<(int)m_objects.size(); i++) {
		if (m_objects[i].object_p->getObjectID() == arg_object_id) {
			return true;
		}
	}
	
	return ret;
}

Object* Tile::getObjectP(String arg_addon_name, NameMode::Type arg_name_mode) {
	for (int i=0; i<(int)m_objects.size(); i++) {
		if (m_objects[i].object_p->getAddonName(arg_name_mode) == arg_addon_name) {
			return m_objects[i].object_p;
		}
	}
	
	return nullptr;
}

Array<Object*> Tile::getObjectsP(CategoryID::Type category_id) {
	Array<Object*> ret_objects;
	
	for (auto object : m_objects) {
		if (object.object_p->getAddonP()->isInCategories(category_id)) {
			ret_objects << object.object_p;
		}
	}
	
	return ret_objects;
}

Array<ObjectStruct> Tile::getObjectStructs() {
	return m_objects;
}

// 描画
void Tile::draw(RateID::Type arg_show_rate_id, PositionStruct arg_draw_position, TimeStruct arg_time) {
	for (int i=0; i<(int)m_objects.size(); i++) {
		int rate;
		Color rateColor = Color(0, 0, 0, 0);
		if (arg_show_rate_id != RateID::None) {
			rate = getRate(arg_show_rate_id);
			if (arg_show_rate_id == RateID::CrimeRate) {
				rateColor = m_get_rate_color(rate, false, 0);
			}
			else {
				rateColor = m_get_rate_color(rate, true, 50);
			}
		}
		
		// オブジェクトが削除された場合
		if (m_objects[i].object_p == nullptr) {
			m_objects.erase(m_objects.begin() + i);
			continue;
		}
		
		m_objects[i].object_p->draw(m_objects[i].relative_coordinate, arg_draw_position, arg_time, rateColor);
	}
}

// 特定のオブジェクトを削除
bool Tile::deleteObject(int arg_object_id) {
	if (m_objects.size() == 0) {
		return false;
	}

	for (auto it = m_objects.begin(); it != m_objects.end();) {
		if (it->object_p->getObjectID() == arg_object_id) {
			UnitaryTools::debugLog(U"delete object at {}, {} of {}, {}"_fmt(it->relative_coordinate.origin.x, it->relative_coordinate.origin.y, it->relative_coordinate.relative.x, it->relative_coordinate.relative.y));
			cout << U"delete object at {}, {} of {}, {}"_fmt(it->relative_coordinate.origin.x, it->relative_coordinate.origin.y, it->relative_coordinate.relative.x, it->relative_coordinate.relative.y) << endl;
            
			m_objects.erase(it);
			
			return true;
		}
        it++;
	}
	
	return false;
}

// クリア
void Tile::clearAddons() {
	addons.clear();
	m_types.clear();
	m_directions.clear();
	
	m_objects.clear();
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

Color Tile::m_get_rate_color(int rate, bool upper, int standard) {
	Color ret(50, 50, 50);
	
	if (upper) {
		if (rate > standard) {
			ret.b += (rate-standard) * 1.27;
		}
		else if (rate < standard) {
			ret.r += (standard-rate) * 1.27;
		}
		return ret;
	}
	
	if (rate < standard) {
		ret.b += (standard-rate) * 1.27;
	}
	else if (rate > standard) {
		ret.r += (rate-standard) * 1.27;
	}
	
	return ret;
}

bool Tile::m_is_objects_category_match(Object* obj1, Object* obj2) {
	if (obj1 == nullptr || obj2 == nullptr) {
		return false;
	}
	
	if (obj1->getAddonP() == nullptr || obj2->getAddonP() == nullptr) {
		return false;
	}
	
	// ToDo: road以外も追加
	if (obj1->getAddonP()->isInCategories(CategoryID::Road) && obj2->getAddonP()->isInCategories(CategoryID::Road)) {
		return true;
	}
	
	return false;
}
