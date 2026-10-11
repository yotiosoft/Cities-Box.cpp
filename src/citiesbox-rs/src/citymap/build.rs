use super::ffi::{BuildDecision, BuildRequest};

const NORTH: i32 = 1;
const SOUTH: i32 = 2;
const EAST: i32 = 5;
const WEST: i32 = 9;

fn denied(code: i32, message: &str) -> BuildDecision {
    BuildDecision {
        allowed: false,
        direction: 0,
        error_code: code,
        error_message: message.to_string(),
    }
}

pub(super) fn plan_build(request: BuildRequest) -> BuildDecision {
    if request.x < 0 || request.y < 0 || request.x >= request.map_width || request.y >= request.map_height {
        return denied(4, "マップの範囲外には建設できません。");
    }
    if request.put_type {
        return BuildDecision { allowed: true, direction: 0, error_code: 0, error_message: String::new() };
    }

    let target_occupied = request.tiles.iter().any(|tile| tile.x == request.x && tile.y == request.y && tile.has_structure);
    if target_occupied && (!request.crossable || request.tiles.iter().any(|tile| tile.x == request.x && tile.y == request.y && tile.has_nonconnectable_structure)) {
        return denied(3, "既存の道路または建物があるため、建設できません。");
    }

    let mut has_road = false;
    let mut has_obstacle = false;
    for option in &request.options {
        let (dx, dy) = match option.direction {
            NORTH => (0, -1),
            SOUTH => (0, 1),
            EAST => (1, 0),
            WEST => (-1, 0),
            _ => (0, 0),
        };
        if !request.connectable {
            let nx = request.x + dx;
            let ny = request.y + dy;
            if !request.tiles.iter().any(|tile| tile.x == nx && tile.y == ny && tile.has_road) {
                continue;
            }
            has_road = true;
        }
        if !option.available || option.width <= 0 || option.height <= 0 {
            continue;
        }
        let origin_x = request.x - if option.direction == EAST { option.width - 1 } else { 0 };
        let origin_y = request.y - if matches!(option.direction, EAST | SOUTH | WEST) { option.height - 1 } else { 0 };
        if origin_x < 0 || origin_y < 0 || origin_x + option.width > request.map_width || origin_y + option.height > request.map_height {
            continue;
        }
        let mut blocked = false;
        for y in origin_y..origin_y + option.height {
            for x in origin_x..origin_x + option.width {
                let Some(tile) = request.tiles.iter().find(|tile| tile.x == x && tile.y == y) else {
                    blocked = true;
                    break;
                };
                if tile.has_structure && !(request.crossable && request.connectable && x == request.x && y == request.y && tile.has_connectable && !tile.has_nonconnectable_structure) {
                    has_obstacle = true;
                    blocked = true;
                    break;
                }
            }
            if blocked { break; }
        }
        if !blocked {
            return BuildDecision { allowed: true, direction: option.direction, error_code: 0, error_message: String::new() };
        }
    }
    if !request.connectable && !has_road {
        denied(1, "道路に隣接していないため、建設できません。")
    } else if has_obstacle {
        denied(3, "既存の道路または建物があるため、建設できません。")
    } else {
        denied(2, "周囲に建設するための土地がないため、建設できません。")
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::citymap::ffi::{BuildOption, BuildTile};

    fn tile(x: i32, y: i32, road: bool, structure: bool) -> BuildTile {
        BuildTile { x, y, has_road: road, has_structure: structure, has_connectable: road, has_nonconnectable_structure: structure && !road }
    }

    fn request() -> BuildRequest {
        BuildRequest {
            x: 1, y: 1, map_width: 4, map_height: 4,
            connectable: false, put_type: false, crossable: false,
            tiles: vec![tile(1, 1, false, false), tile(1, 0, false, false)],
            options: vec![BuildOption { direction: NORTH, width: 1, height: 1, available: true }],
        }
    }

    #[test]
    fn reports_each_build_rejection() {
        let mut input = request();
        assert_eq!(plan_build(input.clone()).error_code, 1);
        input.tiles[1].has_road = true;
        input.options[0].width = 5;
        assert_eq!(plan_build(input.clone()).error_code, 2);
        input.options[0].width = 1;
        input.tiles[0].has_structure = true;
        assert_eq!(plan_build(input).error_code, 3);
    }

    #[test]
    fn chooses_an_available_adjacent_road() {
        let mut input = request();
        input.tiles[1].has_road = true;
        let result = plan_build(input);
        assert!(result.allowed);
        assert_eq!(result.direction, NORTH);
    }
}
