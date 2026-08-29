#ifndef ConnectableBehavior_hpp
#define ConnectableBehavior_hpp

#include <array>
#include <cstddef>

#include "Enums.hpp"

namespace ConnectableBehavior {
	template <class Categories>
	constexpr bool contains(const Categories& categories, CategoryID::Type category) {
		for (const auto value : categories) {
			if (value == category) {
				return true;
			}
		}
		return false;
	}

	template <class LeftCategories, class RightCategories, std::size_t Size>
	constexpr bool shareAnyCategory(
		const LeftCategories& left,
		const RightCategories& right,
		const std::array<CategoryID::Type, Size>& compatibleCategories
	) {
		for (const auto category : compatibleCategories) {
			if (contains(left, category) && contains(right, category)) {
				return true;
			}
		}
		return false;
	}

	template <class LeftCategories, class RightCategories>
	constexpr bool categoriesCanConnect(const LeftCategories& left, const RightCategories& right) {
		if (!contains(left, CategoryID::Connectable) || !contains(right, CategoryID::Connectable)) {
			return false;
		}

		constexpr std::array compatibleCategories{
			CategoryID::Road,
			CategoryID::Train,
			CategoryID::Waterway,
			CategoryID::Airport,
		};
		return shareAnyCategory(left, right, compatibleCategories);
	}

	template <class LeftCategories, class RightCategories>
	constexpr bool categoriesMatch(
		const LeftCategories& left,
		const RightCategories& right,
		CategoryID::Type hint
	) {
		if (hint == CategoryID::Connectable) {
			constexpr std::array compatibleCategories{
				CategoryID::Road,
				CategoryID::Railroad,
				CategoryID::Station,
				CategoryID::Waterway,
				CategoryID::Taxiway,
				CategoryID::Runway,
			};
			return shareAnyCategory(left, right, compatibleCategories);
		}

		if (hint == CategoryID::ObjectType) {
			constexpr std::array compatibleCategories{
				CategoryID::Residential,
				CategoryID::Commecial,
				CategoryID::Office,
				CategoryID::Industrial,
				CategoryID::Farm,
				CategoryID::Public,
				CategoryID::Park,
				CategoryID::Tile,
			};
			return shareAnyCategory(left, right, compatibleCategories);
		}

		return false;
	}

	constexpr TypeID::Type typeFromDirection(DirectionID::Type direction) {
		switch (direction) {
			case DirectionID::None:
				return TypeID::Normal;
			case DirectionID::North:
			case DirectionID::South:
			case DirectionID::East:
			case DirectionID::West:
				return TypeID::DeadEnd;
			case DirectionID::EastWest:
			case DirectionID::NorthSouth:
				return TypeID::Default;
			case DirectionID::SouthWest:
			case DirectionID::NorthWest:
			case DirectionID::SouthEast:
			case DirectionID::NorthEast:
				return TypeID::Turn;
			case DirectionID::SouthEastWest:
			case DirectionID::NorthEastWest:
			case DirectionID::NorthSouthWest:
			case DirectionID::NorthSouthEast:
				return TypeID::IntersectionT;
			case DirectionID::All:
				return TypeID::IntersectionCross;
			case DirectionID::Offshore:
				return TypeID::WaterOffshore;
			case DirectionID::WithoutSouthwestNorthwest:
			case DirectionID::WithoutNortheastNorthwest:
			case DirectionID::WithoutSoutheastSouthwest:
			case DirectionID::WithoutNortheastSoutheast:
				return TypeID::WaterEstuary;
			case DirectionID::WithoutEast:
			case DirectionID::WithoutSouth:
			case DirectionID::WithoutNorth:
			case DirectionID::WithoutWest:
				return TypeID::WaterIntersectionT;
			case DirectionID::WithoutNorthWestNorthwest:
			case DirectionID::WithoutNorthNortheastEast:
			case DirectionID::WithoutSouthSouthwestWest:
			case DirectionID::WithoutEastSoutheastSouth:
				return TypeID::WaterTurn;
			case DirectionID::WithoutNorthwest:
			case DirectionID::WithoutNortheast:
			case DirectionID::WithoutSouthwest:
			case DirectionID::WithoutSoutheast:
				return TypeID::WaterIntersectionCrossWithoutOneCorner;
			default:
				return TypeID::Disabled;
		}
	}

	constexpr TypeID::Type crossingType(CategoryID::Type first, CategoryID::Type second) {
		const bool roadAndRailroad =
			(first == CategoryID::Road && second == CategoryID::Railroad) ||
			(first == CategoryID::Railroad && second == CategoryID::Road);
		if (roadAndRailroad) {
			return TypeID::TrainCrossing;
		}

		const bool routeAndWaterway =
			((first == CategoryID::Road || first == CategoryID::Railroad) && second == CategoryID::Waterway) ||
			(first == CategoryID::Waterway && (second == CategoryID::Road || second == CategoryID::Railroad));
		if (routeAndWaterway) {
			return TypeID::Bridge;
		}

		return TypeID::Disabled;
	}
}

#endif
