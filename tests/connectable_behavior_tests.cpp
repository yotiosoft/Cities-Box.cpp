#include <array>
#include <cstdlib>
#include <iostream>

#include "../src/ConnectableBehavior.hpp"

namespace {
	void expect(bool condition, const char* message) {
		if (!condition) {
			std::cerr << "FAILED: " << message << '\n';
			std::exit(EXIT_FAILURE);
		}
	}

	template <std::size_t Size>
	using Categories = std::array<CategoryID::Type, Size>;

	void testStableEnumValues() {
		static_assert(TypeID::Normal == 1);
		static_assert(TypeID::UnderConstruction == 2);
		static_assert(TypeID::IntersectionT == 5);
		static_assert(TypeID::TrainCrossing == 8);
		static_assert(TypeID::Bridge == 9);
		static_assert(DirectionID::None == 0);
		static_assert(DirectionID::North == 1);
		static_assert(DirectionID::South == 2);
		static_assert(DirectionID::East == 5);
		static_assert(DirectionID::West == 9);
		static_assert(DirectionID::All == 17);
		static_assert(CategoryID::Connectable == 1);
		static_assert(CategoryID::Road == 2);
		static_assert(CategoryID::Railroad == 6);
		static_assert(CategoryID::Station == 8);
		static_assert(CategoryID::Waterway == 9);
		static_assert(CategoryID::Runway == 13);
		static_assert(CategoryID::Taxiway == 14);
	}

	void testRoadShapes() {
		using ConnectableBehavior::typeFromDirection;
		expect(typeFromDirection(DirectionID::None) == TypeID::Normal, "isolated point remains Normal");
		for (const auto direction : {DirectionID::North, DirectionID::South, DirectionID::East, DirectionID::West}) {
			expect(typeFromDirection(direction) == TypeID::DeadEnd, "single connection is a dead end");
		}
		for (const auto direction : {DirectionID::EastWest, DirectionID::NorthSouth}) {
			expect(typeFromDirection(direction) == TypeID::Default, "opposite connections form a straight segment");
		}
		for (const auto direction : {DirectionID::SouthWest, DirectionID::NorthWest, DirectionID::SouthEast, DirectionID::NorthEast}) {
			expect(typeFromDirection(direction) == TypeID::Turn, "adjacent connections form a turn");
		}
		for (const auto direction : {DirectionID::SouthEastWest, DirectionID::NorthEastWest, DirectionID::NorthSouthWest, DirectionID::NorthSouthEast}) {
			expect(typeFromDirection(direction) == TypeID::IntersectionT, "three connections form a T intersection");
		}
		expect(typeFromDirection(DirectionID::All) == TypeID::IntersectionCross, "four connections form a cross intersection");
	}

	void testWaterwayShapes() {
		using ConnectableBehavior::typeFromDirection;
		expect(typeFromDirection(DirectionID::Offshore) == TypeID::WaterOffshore, "all water directions form offshore water");
		for (const auto direction : {DirectionID::WithoutSouthwestNorthwest, DirectionID::WithoutNortheastNorthwest, DirectionID::WithoutSoutheastSouthwest, DirectionID::WithoutNortheastSoutheast}) {
			expect(typeFromDirection(direction) == TypeID::WaterEstuary, "estuary direction maps to WaterEstuary");
		}
		for (const auto direction : {DirectionID::WithoutEast, DirectionID::WithoutSouth, DirectionID::WithoutNorth, DirectionID::WithoutWest}) {
			expect(typeFromDirection(direction) == TypeID::WaterIntersectionT, "three-way water direction maps to WaterIntersectionT");
		}
		for (const auto direction : {DirectionID::WithoutNorthWestNorthwest, DirectionID::WithoutNorthNortheastEast, DirectionID::WithoutSouthSouthwestWest, DirectionID::WithoutEastSoutheastSouth}) {
			expect(typeFromDirection(direction) == TypeID::WaterTurn, "water corner maps to WaterTurn");
		}
		for (const auto direction : {DirectionID::WithoutNorthwest, DirectionID::WithoutNortheast, DirectionID::WithoutSouthwest, DirectionID::WithoutSoutheast}) {
			expect(typeFromDirection(direction) == TypeID::WaterIntersectionCrossWithoutOneCorner, "missing diagonal maps to the cornerless water cross");
		}
	}

	void testBuildAndRemovalShapeTransitions() {
		using ConnectableBehavior::typeFromDirection;
		expect(typeFromDirection(DirectionID::None) == TypeID::Normal, "a recalculated segment with no connections uses the current Normal shape");
		expect(typeFromDirection(DirectionID::North) == TypeID::DeadEnd, "the first connection changes an isolated segment into a dead end");
		expect(typeFromDirection(DirectionID::NorthSouth) == TypeID::Default, "extending in the opposite direction creates a straight segment");
		expect(typeFromDirection(DirectionID::North) == TypeID::DeadEnd, "removing one side of a straight segment restores a dead end");
		expect(typeFromDirection(DirectionID::None) == TypeID::Normal, "removing the last connection restores the current no-connection shape");
	}

	void testConnectionCompatibility() {
		const Categories<2> road{CategoryID::Connectable, CategoryID::Road};
		const Categories<2> otherRoad{CategoryID::Connectable, CategoryID::Road};
		const Categories<3> railroad{CategoryID::Connectable, CategoryID::Railroad, CategoryID::Train};
		const Categories<3> station{CategoryID::Connectable, CategoryID::Station, CategoryID::Train};
		const Categories<2> waterway{CategoryID::Connectable, CategoryID::Waterway};
		const Categories<3> runway{CategoryID::Connectable, CategoryID::Runway, CategoryID::Airport};
		const Categories<3> taxiway{CategoryID::Connectable, CategoryID::Taxiway, CategoryID::Airport};
		const Categories<1> nonConnectableRoad{CategoryID::Road};

		expect(ConnectableBehavior::categoriesCanConnect(road, otherRoad), "roads connect through Road");
		expect(ConnectableBehavior::categoriesCanConnect(railroad, station), "railroads and stations connect through Train");
		expect(ConnectableBehavior::categoriesCanConnect(runway, taxiway), "runways and taxiways connect through Airport");
		expect(!ConnectableBehavior::categoriesCanConnect(road, railroad), "roads do not directly connect to railroads");
		expect(!ConnectableBehavior::categoriesCanConnect(road, nonConnectableRoad), "both addons must be Connectable");
		expect(!ConnectableBehavior::categoriesCanConnect(road, waterway), "roads do not directly connect to waterways");

		expect(ConnectableBehavior::categoriesMatch(railroad, railroad, CategoryID::Connectable), "Railroad is an explicit match category");
		expect(!ConnectableBehavior::categoriesMatch(railroad, station, CategoryID::Connectable), "isMatch keeps Railroad and Station distinct");
		expect(ConnectableBehavior::categoriesMatch(runway, runway, CategoryID::Connectable), "Runway is an explicit match category");
		expect(!ConnectableBehavior::categoriesMatch(runway, taxiway, CategoryID::Connectable), "isMatch keeps Runway and Taxiway distinct");
	}

	void testCrossingTypes() {
		using ConnectableBehavior::crossingType;
		expect(crossingType(CategoryID::Road, CategoryID::Railroad) == TypeID::TrainCrossing, "road over railroad creates a train crossing");
		expect(crossingType(CategoryID::Railroad, CategoryID::Road) == TypeID::TrainCrossing, "railroad over road creates a train crossing");
		expect(crossingType(CategoryID::Road, CategoryID::Waterway) == TypeID::Bridge, "road over waterway creates a bridge");
		expect(crossingType(CategoryID::Railroad, CategoryID::Waterway) == TypeID::Bridge, "railroad over waterway creates a bridge");
		expect(crossingType(CategoryID::Waterway, CategoryID::Road) == TypeID::Bridge, "waterway crossing a road creates a bridge");
		expect(crossingType(CategoryID::Waterway, CategoryID::Railroad) == TypeID::Bridge, "waterway crossing a railroad creates a bridge");
		expect(crossingType(CategoryID::Road, CategoryID::Road) == TypeID::Disabled, "same-kind routes do not create a crossing object");
	}
}

int main() {
	testStableEnumValues();
	testRoadShapes();
	testWaterwayShapes();
	testBuildAndRemovalShapeTransitions();
	testConnectionCompatibility();
	testCrossingTypes();
	std::cout << "connectable behavior characterization tests passed\n";
	return EXIT_SUCCESS;
}
