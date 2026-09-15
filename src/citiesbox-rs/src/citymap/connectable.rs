//! Value-only connection decisions mirrored from the current C++ behavior.
//!
//! This module intentionally has no `cxx` bridge or map ownership. It is the
//! Rust-side destination for connection rules while C++ continues to apply
//! decisions to `Object*`, `Tile`, and addon instances.

pub(crate) mod type_id {
    pub const DISABLED: i32 = 0;
    pub const NORMAL: i32 = 1;
    pub const DEFAULT: i32 = 3;
    pub const TURN: i32 = 4;
    pub const INTERSECTION_T: i32 = 5;
    pub const INTERSECTION_CROSS: i32 = 6;
    pub const DEAD_END: i32 = 7;
    pub const TRAIN_CROSSING: i32 = 8;
    pub const BRIDGE: i32 = 9;
    pub const WATER_ESTUARY: i32 = 12;
    pub const WATER_INTERSECTION_CROSS_WITHOUT_ONE_CORNER: i32 = 13;
    pub const WATER_INTERSECTION_T: i32 = 16;
    pub const WATER_OFFSHORE: i32 = 17;
    pub const WATER_TURN: i32 = 18;
}

pub(crate) mod direction_id {
    pub const DISABLED: i32 = -1;
    pub const NONE: i32 = 0;
    pub const NORTH: i32 = 1;
    pub const SOUTH: i32 = 2;
    pub const EAST: i32 = 5;
    pub const WEST: i32 = 9;
    pub const EAST_WEST: i32 = EAST + WEST;
    pub const NORTH_SOUTH: i32 = NORTH + SOUTH;
    pub const SOUTH_WEST: i32 = SOUTH + WEST;
    pub const NORTH_WEST: i32 = NORTH + WEST;
    pub const SOUTH_EAST: i32 = SOUTH + EAST;
    pub const NORTH_EAST: i32 = NORTH + EAST;
    pub const SOUTH_EAST_WEST: i32 = SOUTH + EAST + WEST;
    pub const NORTH_EAST_WEST: i32 = NORTH + EAST + WEST;
    pub const NORTH_SOUTH_WEST: i32 = NORTH + SOUTH + WEST;
    pub const NORTH_SOUTH_EAST: i32 = NORTH + SOUTH + EAST;
    pub const ALL: i32 = NORTH + EAST + SOUTH + WEST;
    pub const NORTHEAST: i32 = 21;
    pub const NORTHWEST: i32 = 22;
    pub const SOUTHEAST: i32 = 26;
    pub const SOUTHWEST: i32 = 29;
    pub const OFFSHORE: i32 =
        NORTH + NORTHEAST + EAST + SOUTHEAST + SOUTH + SOUTHWEST + WEST + NORTHWEST;
    pub const WITHOUT_SOUTHWEST_NORTHWEST: i32 = OFFSHORE - SOUTHWEST - NORTHWEST;
    pub const WITHOUT_NORTHEAST_NORTHWEST: i32 = OFFSHORE - NORTHEAST - NORTHWEST;
    pub const WITHOUT_SOUTHEAST_SOUTHWEST: i32 = OFFSHORE - SOUTHEAST - SOUTHWEST;
    pub const WITHOUT_NORTHEAST_SOUTHEAST: i32 = OFFSHORE - NORTHEAST - SOUTHEAST;
    pub const WITHOUT_EAST: i32 = OFFSHORE - NORTHEAST - EAST - SOUTHEAST;
    pub const WITHOUT_SOUTH: i32 = OFFSHORE - SOUTHEAST - SOUTH - SOUTHWEST;
    pub const WITHOUT_NORTH: i32 = OFFSHORE - NORTH - NORTHEAST - NORTHWEST;
    pub const WITHOUT_WEST: i32 = OFFSHORE - SOUTHWEST - WEST - NORTHWEST;
    pub const WITHOUT_NORTH_WEST_NORTHWEST: i32 = OFFSHORE - NORTH - WEST - NORTHWEST;
    pub const WITHOUT_NORTH_NORTHEAST_EAST: i32 = OFFSHORE - NORTH - NORTHEAST - EAST;
    pub const WITHOUT_SOUTH_SOUTHWEST_WEST: i32 = OFFSHORE - SOUTH - SOUTHWEST - WEST;
    pub const WITHOUT_EAST_SOUTHEAST_SOUTH: i32 = OFFSHORE - EAST - SOUTHEAST - SOUTH;
    pub const WITHOUT_NORTHWEST: i32 = OFFSHORE - NORTHWEST;
    pub const WITHOUT_NORTHEAST: i32 = OFFSHORE - NORTHEAST;
    pub const WITHOUT_SOUTHWEST: i32 = OFFSHORE - SOUTHWEST;
    pub const WITHOUT_SOUTHEAST: i32 = OFFSHORE - SOUTHEAST;
}

pub(crate) mod category_id {
    pub const CONNECTABLE: i32 = 1;
    pub const ROAD: i32 = 2;
    pub const RAILROAD: i32 = 6;
    pub const TRAIN: i32 = 7;
    pub const STATION: i32 = 8;
    pub const WATERWAY: i32 = 9;
    pub const AIRPORT: i32 = 12;
    pub const RUNWAY: i32 = 13;
    pub const TAXIWAY: i32 = 14;
    pub const OBJECT_TYPE: i32 = 16;
    pub const RESIDENTIAL: i32 = 19;
    pub const COMMERCIAL: i32 = 20;
    pub const OFFICE: i32 = 21;
    pub const INDUSTRIAL: i32 = 22;
    pub const FARM: i32 = 23;
    pub const PUBLIC: i32 = 24;
    pub const PARK: i32 = 35;
    pub const TILE: i32 = 37;
}

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub(crate) struct Coordinate {
    pub x: i32,
    pub y: i32,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) enum ConnectionStatus {
    Apply,
    AlreadyConnected,
    InvalidDirection,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) struct ConnectionRequest {
    pub from: Coordinate,
    pub to: Coordinate,
    pub current_direction: i32,
    pub current_type: i32,
    pub allow_diagonal: bool,
    pub from_here: bool,
    pub connection_slot_occupied: bool,
    pub forced_type: Option<i32>,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) struct ConnectionDecision {
    pub status: ConnectionStatus,
    pub relative_direction: i32,
    pub updated_direction: i32,
    pub updated_type: i32,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) struct RemovalDecision {
    pub updated_direction: i32,
    pub updated_type: i32,
    pub isolated: bool,
}

pub(crate) fn categories_can_connect(left: &[i32], right: &[i32]) -> bool {
    if !left.contains(&category_id::CONNECTABLE) || !right.contains(&category_id::CONNECTABLE) {
        return false;
    }
    [
        category_id::ROAD,
        category_id::TRAIN,
        category_id::WATERWAY,
        category_id::AIRPORT,
    ]
    .iter()
    .any(|category| left.contains(category) && right.contains(category))
}

pub(crate) fn categories_match(left: &[i32], right: &[i32], hint: i32) -> bool {
    let candidates: &[i32] = match hint {
        category_id::CONNECTABLE => &[
            category_id::ROAD,
            category_id::RAILROAD,
            category_id::STATION,
            category_id::WATERWAY,
            category_id::TAXIWAY,
            category_id::RUNWAY,
        ],
        category_id::OBJECT_TYPE => &[
            category_id::RESIDENTIAL,
            category_id::COMMERCIAL,
            category_id::OFFICE,
            category_id::INDUSTRIAL,
            category_id::FARM,
            category_id::PUBLIC,
            category_id::PARK,
            category_id::TILE,
        ],
        _ => return false,
    };
    candidates
        .iter()
        .any(|category| left.contains(category) && right.contains(category))
}

pub(crate) fn crossing_type(first: i32, second: i32) -> i32 {
    let road_and_railroad = (first == category_id::ROAD && second == category_id::RAILROAD)
        || (first == category_id::RAILROAD && second == category_id::ROAD);
    if road_and_railroad {
        return type_id::TRAIN_CROSSING;
    }
    let route_and_waterway = ((first == category_id::ROAD || first == category_id::RAILROAD)
        && second == category_id::WATERWAY)
        || (first == category_id::WATERWAY
            && (second == category_id::ROAD || second == category_id::RAILROAD));
    if route_and_waterway {
        return type_id::BRIDGE;
    }
    type_id::DISABLED
}

pub(crate) fn type_from_direction(direction: i32) -> i32 {
    use direction_id as direction;
    match direction {
        direction::NONE => type_id::NORMAL,
        direction::NORTH | direction::SOUTH | direction::EAST | direction::WEST => {
            type_id::DEAD_END
        }
        direction::EAST_WEST | direction::NORTH_SOUTH => type_id::DEFAULT,
        direction::SOUTH_WEST
        | direction::NORTH_WEST
        | direction::SOUTH_EAST
        | direction::NORTH_EAST => type_id::TURN,
        direction::SOUTH_EAST_WEST
        | direction::NORTH_EAST_WEST
        | direction::NORTH_SOUTH_WEST
        | direction::NORTH_SOUTH_EAST => type_id::INTERSECTION_T,
        direction::ALL => type_id::INTERSECTION_CROSS,
        direction::OFFSHORE => type_id::WATER_OFFSHORE,
        direction::WITHOUT_SOUTHWEST_NORTHWEST
        | direction::WITHOUT_NORTHEAST_NORTHWEST
        | direction::WITHOUT_SOUTHEAST_SOUTHWEST
        | direction::WITHOUT_NORTHEAST_SOUTHEAST => type_id::WATER_ESTUARY,
        direction::WITHOUT_EAST
        | direction::WITHOUT_SOUTH
        | direction::WITHOUT_NORTH
        | direction::WITHOUT_WEST => type_id::WATER_INTERSECTION_T,
        direction::WITHOUT_NORTH_WEST_NORTHWEST
        | direction::WITHOUT_NORTH_NORTHEAST_EAST
        | direction::WITHOUT_SOUTH_SOUTHWEST_WEST
        | direction::WITHOUT_EAST_SOUTHEAST_SOUTH => type_id::WATER_TURN,
        direction::WITHOUT_NORTHWEST
        | direction::WITHOUT_NORTHEAST
        | direction::WITHOUT_SOUTHWEST
        | direction::WITHOUT_SOUTHEAST => type_id::WATER_INTERSECTION_CROSS_WITHOUT_ONE_CORNER,
        _ => type_id::DISABLED,
    }
}

pub(crate) fn direction_from_difference(
    from: Coordinate,
    to: Coordinate,
    allow_diagonal: bool,
) -> i32 {
    match (to.x - from.x, to.y - from.y, allow_diagonal) {
        (0, 0, _) => direction_id::NONE,
        (-1, 0, _) => direction_id::WEST,
        (1, 0, _) => direction_id::EAST,
        (0, -1, _) => direction_id::NORTH,
        (0, 1, _) => direction_id::SOUTH,
        (-1, -1, true) => direction_id::NORTHWEST,
        (1, -1, true) => direction_id::NORTHEAST,
        (1, 1, true) => direction_id::SOUTHEAST,
        (-1, 1, true) => direction_id::SOUTHWEST,
        _ => direction_id::DISABLED,
    }
}

fn split_directions(direction: i32) -> Vec<i32> {
    use direction_id as d;
    let all = vec![
        d::NORTH,
        d::NORTHEAST,
        d::EAST,
        d::SOUTHEAST,
        d::SOUTH,
        d::SOUTHWEST,
        d::WEST,
        d::NORTHWEST,
    ];
    match direction {
        d::EAST_WEST => vec![d::EAST, d::WEST],
        d::NORTH_SOUTH => vec![d::NORTH, d::SOUTH],
        d::SOUTH_EAST_WEST => vec![d::SOUTH, d::EAST, d::WEST],
        d::NORTH_EAST_WEST => vec![d::NORTH, d::EAST, d::WEST],
        d::NORTH_SOUTH_WEST => vec![d::NORTH, d::SOUTH, d::WEST],
        d::NORTH_SOUTH_EAST => vec![d::NORTH, d::SOUTH, d::EAST],
        d::SOUTH_WEST => vec![d::SOUTH, d::WEST],
        d::NORTH_WEST => vec![d::NORTH, d::WEST],
        d::SOUTH_EAST => vec![d::SOUTH, d::EAST],
        d::NORTH_EAST => vec![d::NORTH, d::EAST],
        d::ALL => vec![d::WEST, d::NORTH, d::EAST, d::SOUTH],
        d::OFFSHORE => all,
        d::WITHOUT_SOUTHWEST_NORTHWEST => without(all, &[d::SOUTHWEST, d::NORTHWEST]),
        d::WITHOUT_NORTHEAST_NORTHWEST => without(all, &[d::NORTHEAST, d::NORTHWEST]),
        d::WITHOUT_SOUTHEAST_SOUTHWEST => without(all, &[d::SOUTHEAST, d::SOUTHWEST]),
        d::WITHOUT_NORTHEAST_SOUTHEAST => without(all, &[d::NORTHEAST, d::SOUTHEAST]),
        d::WITHOUT_EAST => without(all, &[d::NORTHEAST, d::EAST, d::SOUTHEAST]),
        d::WITHOUT_SOUTH => without(all, &[d::SOUTHEAST, d::SOUTH, d::SOUTHWEST]),
        d::WITHOUT_NORTH => without(all, &[d::NORTH, d::NORTHEAST, d::NORTHWEST]),
        d::WITHOUT_WEST => without(all, &[d::SOUTHWEST, d::WEST, d::NORTHWEST]),
        d::WITHOUT_NORTH_WEST_NORTHWEST => without(all, &[d::NORTH, d::WEST, d::NORTHWEST]),
        d::WITHOUT_NORTH_NORTHEAST_EAST => without(all, &[d::NORTH, d::NORTHEAST, d::EAST]),
        d::WITHOUT_SOUTH_SOUTHWEST_WEST => without(all, &[d::SOUTH, d::SOUTHWEST, d::WEST]),
        d::WITHOUT_EAST_SOUTHEAST_SOUTH => without(all, &[d::EAST, d::SOUTHEAST, d::SOUTH]),
        d::WITHOUT_NORTHWEST => without(all, &[d::NORTHWEST]),
        d::WITHOUT_NORTHEAST => without(all, &[d::NORTHEAST]),
        d::WITHOUT_SOUTHWEST => without(all, &[d::SOUTHWEST]),
        d::WITHOUT_SOUTHEAST => without(all, &[d::SOUTHEAST]),
        _ => vec![direction],
    }
}

fn without(mut directions: Vec<i32>, removed: &[i32]) -> Vec<i32> {
    directions.retain(|direction| !removed.contains(direction));
    directions
}

fn contains_direction(directions: i32, direction: i32) -> bool {
    split_directions(directions).contains(&direction)
}

fn add_direction(current: i32, added: i32) -> i32 {
    current + split_directions(added).iter().sum::<i32>()
}

fn remove_direction(current: i32, removed: i32) -> i32 {
    current - split_directions(removed).iter().sum::<i32>()
}

pub(crate) fn plan_connection(request: ConnectionRequest) -> ConnectionDecision {
    let relative = direction_from_difference(request.from, request.to, request.allow_diagonal);
    if relative == direction_id::DISABLED {
        return ConnectionDecision {
            status: ConnectionStatus::InvalidDirection,
            relative_direction: relative,
            updated_direction: request.current_direction,
            updated_type: request.current_type,
        };
    }
    // 保存済みDirectionIDは接続方向を保持しているが、ロード直後のC++側には
    // 接続ポインタがまだない。ポインタ一覧ではなく方向状態を正として、
    // 同じ方向を二重加算しない。
    if contains_direction(request.current_direction, relative) {
        return ConnectionDecision {
            status: ConnectionStatus::AlreadyConnected,
            relative_direction: relative,
            updated_direction: request.current_direction,
            updated_type: request.current_type,
        };
    }

    let updated_direction = add_direction(request.current_direction, relative);
    let preserve_crossing =
        request.current_type == type_id::TRAIN_CROSSING || request.current_type == type_id::BRIDGE;
    let updated_type = request.forced_type.unwrap_or_else(|| {
        if preserve_crossing {
            request.current_type
        } else {
            type_from_direction(updated_direction)
        }
    });
    ConnectionDecision {
        status: ConnectionStatus::Apply,
        relative_direction: relative,
        updated_direction,
        updated_type,
    }
}

pub(crate) fn plan_removal(current_direction: i32, removed_direction: i32) -> RemovalDecision {
    let updated_direction = remove_direction(current_direction, removed_direction);
    RemovalDecision {
        updated_direction,
        updated_type: type_from_direction(updated_direction),
        isolated: updated_direction == direction_id::DISABLED
            || updated_direction == direction_id::NONE,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn persisted_ids_match_cpp_values() {
        assert_eq!(type_id::TRAIN_CROSSING, 8);
        assert_eq!(type_id::BRIDGE, 9);
        assert_eq!(direction_id::NORTH, 1);
        assert_eq!(direction_id::SOUTH, 2);
        assert_eq!(direction_id::EAST, 5);
        assert_eq!(direction_id::WEST, 9);
        assert_eq!(direction_id::ALL, 17);
        assert_eq!(category_id::CONNECTABLE, 1);
        assert_eq!(category_id::RUNWAY, 13);
        assert_eq!(category_id::TAXIWAY, 14);
        assert_eq!(category_id::PARK, 35);
        assert_eq!(category_id::TILE, 37);
    }

    #[test]
    fn shapes_match_cpp_characterization() {
        assert_eq!(type_from_direction(direction_id::NONE), type_id::NORMAL);
        assert_eq!(type_from_direction(direction_id::NORTH), type_id::DEAD_END);
        assert_eq!(
            type_from_direction(direction_id::NORTH_SOUTH),
            type_id::DEFAULT
        );
        assert_eq!(type_from_direction(direction_id::NORTH_EAST), type_id::TURN);
        assert_eq!(
            type_from_direction(direction_id::NORTH_EAST_WEST),
            type_id::INTERSECTION_T
        );
        assert_eq!(
            type_from_direction(direction_id::ALL),
            type_id::INTERSECTION_CROSS
        );
        assert_eq!(
            type_from_direction(direction_id::OFFSHORE),
            type_id::WATER_OFFSHORE
        );
        assert_eq!(
            type_from_direction(direction_id::WITHOUT_NORTHEAST),
            type_id::WATER_INTERSECTION_CROSS_WITHOUT_ONE_CORNER
        );
    }

    #[test]
    fn category_and_crossing_rules_match_cpp_characterization() {
        let road = [category_id::CONNECTABLE, category_id::ROAD];
        let railroad = [
            category_id::CONNECTABLE,
            category_id::RAILROAD,
            category_id::TRAIN,
        ];
        let station = [
            category_id::CONNECTABLE,
            category_id::STATION,
            category_id::TRAIN,
        ];
        assert!(categories_can_connect(&railroad, &station));
        assert!(!categories_can_connect(&road, &railroad));
        assert!(!categories_match(
            &railroad,
            &station,
            category_id::CONNECTABLE
        ));
        assert!(categories_match(
            &[category_id::OBJECT_TYPE, category_id::PARK],
            &[category_id::OBJECT_TYPE, category_id::PARK],
            category_id::OBJECT_TYPE
        ));
        assert_eq!(
            crossing_type(category_id::ROAD, category_id::RAILROAD),
            type_id::TRAIN_CROSSING
        );
        assert_eq!(
            crossing_type(category_id::WATERWAY, category_id::ROAD),
            type_id::BRIDGE
        );
    }

    #[test]
    fn direction_and_connection_decisions_match_cpp_characterization() {
        assert_eq!(
            direction_from_difference(Coordinate { x: 4, y: 4 }, Coordinate { x: 5, y: 5 }, false),
            direction_id::DISABLED
        );
        assert_eq!(
            direction_from_difference(Coordinate { x: 4, y: 4 }, Coordinate { x: 5, y: 5 }, true),
            direction_id::SOUTHEAST
        );

        let mut request = ConnectionRequest {
            from: Coordinate { x: 10, y: 10 },
            to: Coordinate { x: 11, y: 10 },
            current_direction: direction_id::NORTH,
            current_type: type_id::DEAD_END,
            allow_diagonal: false,
            from_here: false,
            connection_slot_occupied: false,
            forced_type: None,
        };
        let decision = plan_connection(request);
        assert_eq!(decision.status, ConnectionStatus::Apply);
        assert_eq!(decision.updated_direction, direction_id::NORTH_EAST);
        assert_eq!(decision.updated_type, type_id::TURN);

        request.current_direction = direction_id::NORTH_EAST;
        request.current_type = type_id::TURN;
        request.from_here = true;
        request.connection_slot_occupied = true;
        assert_eq!(
            plan_connection(request).status,
            ConnectionStatus::AlreadyConnected
        );

        request.from_here = false;
        request.forced_type = Some(type_id::TRAIN_CROSSING);
        assert_eq!(
            plan_connection(request).status,
            ConnectionStatus::AlreadyConnected
        );
    }

    #[test]
    fn persisted_direction_prevents_duplicate_connection_without_cpp_pointer() {
        let request = ConnectionRequest {
            from: Coordinate { x: 10, y: 10 },
            to: Coordinate { x: 11, y: 10 },
            current_direction: direction_id::EAST_WEST,
            current_type: type_id::DEFAULT,
            allow_diagonal: false,
            from_here: false,
            connection_slot_occupied: false,
            forced_type: None,
        };

        let decision = plan_connection(request);
        assert_eq!(decision.status, ConnectionStatus::AlreadyConnected);
        assert_eq!(decision.updated_direction, direction_id::EAST_WEST);
        assert_eq!(decision.updated_type, type_id::DEFAULT);
    }

    #[test]
    fn water_diagonal_uses_the_distinct_water_direction_id() {
        assert_eq!(
            direction_from_difference(Coordinate { x: 4, y: 4 }, Coordinate { x: 5, y: 5 }, true),
            direction_id::SOUTHEAST
        );
        assert_ne!(direction_id::SOUTHEAST, direction_id::SOUTH_EAST);
    }

    #[test]
    fn water_composite_detects_an_existing_atomic_direction() {
        assert!(contains_direction(
            direction_id::OFFSHORE,
            direction_id::NORTHWEST
        ));
        assert!(!contains_direction(
            direction_id::WITHOUT_NORTHWEST,
            direction_id::NORTHWEST
        ));
    }

    #[test]
    fn removal_decision_matches_cpp_characterization() {
        let remaining = plan_removal(direction_id::NORTH_SOUTH, direction_id::SOUTH);
        assert_eq!(remaining.updated_direction, direction_id::NORTH);
        assert_eq!(remaining.updated_type, type_id::DEAD_END);
        assert!(!remaining.isolated);

        let isolated = plan_removal(direction_id::NORTH, direction_id::NORTH);
        assert_eq!(isolated.updated_direction, direction_id::NONE);
        assert_eq!(isolated.updated_type, type_id::NORMAL);
        assert!(isolated.isolated);
    }
}
