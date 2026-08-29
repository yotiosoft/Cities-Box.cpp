#ifndef ConnectableBehavior_hpp
#define ConnectableBehavior_hpp

#include <array>
#include <cstddef>

#include "Enums.hpp"

namespace ConnectableBehavior {
	struct Coordinate {
		int x = 0;
		int y = 0;
	};

	struct DirectionParts {
		std::array<DirectionID::Type, 4> values{};
		std::size_t size = 0;
	};

	enum class ConnectionStatus {
		Apply,
		AlreadyConnected,
		InvalidDirection,
	};

	struct ConnectionRequest {
		Coordinate from;
		Coordinate to;
		DirectionID::Type currentDirection = DirectionID::None;
		TypeID::Type currentType = TypeID::Normal;
		bool allowDiagonal = false;
		bool fromHere = false;
		bool connectionSlotOccupied = false;
		bool forceType = false;
		TypeID::Type forcedType = TypeID::Disabled;
	};

	struct ConnectionDecision {
		ConnectionStatus status = ConnectionStatus::InvalidDirection;
		DirectionID::Type relativeDirection = DirectionID::Disabled;
		DirectionID::Type updatedDirection = DirectionID::None;
		TypeID::Type updatedType = TypeID::Normal;
	};

	struct RemovalRequest {
		DirectionID::Type currentDirection = DirectionID::None;
		DirectionID::Type removedDirection = DirectionID::Disabled;
	};

	struct RemovalDecision {
		DirectionID::Type updatedDirection = DirectionID::None;
		TypeID::Type updatedType = TypeID::Normal;
		bool isolated = true;
	};

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

	constexpr DirectionID::Type directionFromDifference(Coordinate from, Coordinate to, bool allowDiagonal) {
		const int dx = to.x - from.x;
		const int dy = to.y - from.y;
		if (dx == 0 && dy == 0) return DirectionID::None;
		if (dx == -1 && dy == 0) return DirectionID::West;
		if (dx == 1 && dy == 0) return DirectionID::East;
		if (dx == 0 && dy == -1) return DirectionID::North;
		if (dx == 0 && dy == 1) return DirectionID::South;
		if (allowDiagonal) {
			if (dx == -1 && dy == -1) return DirectionID::NorthWest;
			if (dx == 1 && dy == -1) return DirectionID::NorthEast;
			if (dx == 1 && dy == 1) return DirectionID::SouthEast;
			if (dx == -1 && dy == 1) return DirectionID::SouthWest;
		}
		return DirectionID::Disabled;
	}

	constexpr DirectionParts splitDirections(DirectionID::Type direction) {
		switch (direction) {
			case DirectionID::EastWest: return {{{DirectionID::East, DirectionID::West}}, 2};
			case DirectionID::NorthSouth: return {{{DirectionID::North, DirectionID::South}}, 2};
			case DirectionID::SouthEastWest: return {{{DirectionID::South, DirectionID::East, DirectionID::West}}, 3};
			case DirectionID::NorthEastWest: return {{{DirectionID::North, DirectionID::East, DirectionID::West}}, 3};
			case DirectionID::NorthSouthWest: return {{{DirectionID::North, DirectionID::South, DirectionID::West}}, 3};
			case DirectionID::NorthSouthEast: return {{{DirectionID::North, DirectionID::South, DirectionID::East}}, 3};
			case DirectionID::SouthWest: return {{{DirectionID::South, DirectionID::West}}, 2};
			case DirectionID::NorthWest: return {{{DirectionID::North, DirectionID::West}}, 2};
			case DirectionID::SouthEast: return {{{DirectionID::South, DirectionID::East}}, 2};
			case DirectionID::NorthEast: return {{{DirectionID::North, DirectionID::East}}, 2};
			case DirectionID::All: return {{{DirectionID::West, DirectionID::North, DirectionID::East, DirectionID::South}}, 4};
			default: return {{{direction}}, 1};
		}
	}

	constexpr bool containsDirection(DirectionID::Type directions, DirectionID::Type direction) {
		const auto parts = splitDirections(directions);
		for (std::size_t i = 0; i < parts.size; ++i) {
			if (parts.values[i] == direction) return true;
		}
		return false;
	}

	constexpr DirectionID::Type addDirection(DirectionID::Type current, DirectionID::Type added) {
		int result = static_cast<int>(current);
		const auto parts = splitDirections(added);
		for (std::size_t i = 0; i < parts.size; ++i) result += static_cast<int>(parts.values[i]);
		return static_cast<DirectionID::Type>(result);
	}

	constexpr DirectionID::Type removeDirection(DirectionID::Type current, DirectionID::Type removed) {
		int result = static_cast<int>(current);
		const auto parts = splitDirections(removed);
		for (std::size_t i = 0; i < parts.size; ++i) result -= static_cast<int>(parts.values[i]);
		return static_cast<DirectionID::Type>(result);
	}

	constexpr ConnectionDecision planConnection(const ConnectionRequest& request) {
		const auto relative = directionFromDifference(request.from, request.to, request.allowDiagonal);
		if (request.fromHere && request.connectionSlotOccupied && containsDirection(request.currentDirection, relative)) {
			return {ConnectionStatus::AlreadyConnected, relative, request.currentDirection, request.currentType};
		}
		if (relative == DirectionID::Disabled) {
			return {ConnectionStatus::InvalidDirection, relative, request.currentDirection, request.currentType};
		}

		const auto direction = addDirection(request.currentDirection, relative);
		const bool preserveCrossing = request.currentType == TypeID::TrainCrossing || request.currentType == TypeID::Bridge;
		const auto type = request.forceType ? request.forcedType : (preserveCrossing ? request.currentType : typeFromDirection(direction));
		return {ConnectionStatus::Apply, relative, direction, type};
	}

	constexpr RemovalDecision planRemoval(const RemovalRequest& request) {
		const auto direction = removeDirection(request.currentDirection, request.removedDirection);
		return {
			direction,
			typeFromDirection(direction),
			direction == DirectionID::Disabled || direction == DirectionID::None,
		};
	}
}

#endif
