use serde::{Deserialize, Serialize};
use serde_json::{Value, json};
use std::collections::{HashMap, HashSet};
use std::fs::{self, OpenOptions};
use std::io::{self, Write};
use std::path::{Path, PathBuf};

fn sidecar_path(path: &Path, suffix: &str) -> PathBuf {
    let mut value = path.as_os_str().to_os_string();
    value.push(suffix);
    PathBuf::from(value)
}

fn write_and_sync(path: &Path, contents: &[u8]) -> io::Result<()> {
    let mut file = OpenOptions::new().write(true).create_new(true).open(path)?;
    file.write_all(contents)?;
    file.sync_all()
}

fn io_context(context: &str, error: io::Error) -> io::Error {
    io::Error::new(error.kind(), format!("{context}: {error}"))
}

#[cfg(windows)]
fn replace_path_atomically(replacement: &Path, target: &Path) -> io::Result<()> {
    use std::iter;
    use std::os::windows::ffi::OsStrExt;

    #[link(name = "Kernel32")]
    unsafe extern "system" {
        fn MoveFileExW(
            existing_file_name: *const u16,
            new_file_name: *const u16,
            flags: u32,
        ) -> i32;
    }

    fn wide(path: &Path) -> Vec<u16> {
        path.as_os_str()
            .encode_wide()
            .chain(iter::once(0))
            .collect()
    }

    const MOVEFILE_REPLACE_EXISTING: u32 = 0x0000_0001;
    const MOVEFILE_WRITE_THROUGH: u32 = 0x0000_0008;

    let replacement_wide = wide(replacement);
    let target_wide = wide(target);
    let succeeded = unsafe {
        MoveFileExW(
            replacement_wide.as_ptr(),
            target_wide.as_ptr(),
            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH,
        )
    };

    if succeeded == 0 {
        Err(io::Error::last_os_error())
    } else {
        Ok(())
    }
}

#[cfg(not(windows))]
fn replace_path_atomically(replacement: &Path, target: &Path) -> io::Result<()> {
    fs::rename(replacement, target)
}

fn replace_file_atomically(target: &Path, replacement: &Path, backup: &Path) -> io::Result<()> {
    if target.exists() {
        let backup_temp = sidecar_path(backup, &format!(".tmp.{}", std::process::id()));
        let backup_result = (|| {
            fs::copy(target, &backup_temp).map_err(|error| io_context("copy backup", error))?;
            OpenOptions::new()
                .read(true)
                .write(true)
                .open(&backup_temp)
                .and_then(|file| file.sync_all())
                .map_err(|error| io_context("sync backup", error))?;
            replace_path_atomically(&backup_temp, backup)
                .map_err(|error| io_context("replace backup", error))
        })();
        if backup_result.is_err() {
            let _ = fs::remove_file(&backup_temp);
        }
        backup_result?;
    }
    replace_path_atomically(replacement, target).map_err(|error| io_context("replace save", error))
}

fn atomic_write_with_backup(path: &Path, contents: &[u8]) -> io::Result<()> {
    let temp = sidecar_path(path, &format!(".tmp.{}", std::process::id()));
    let backup = sidecar_path(path, ".bak");
    let result = (|| {
        write_and_sync(&temp, contents)?;
        replace_file_atomically(path, &temp, &backup)
    })();

    if result.is_err() {
        let _ = fs::remove_file(&temp);
    }
    result
}

#[derive(Default)]
struct AddonMetadata {
    categories: HashSet<String>,
    tile_sizes: HashMap<(String, String), (i32, i32)>,
}

fn normalized_token(value: &str) -> String {
    value
        .chars()
        .filter(|ch| ch.is_ascii_alphanumeric())
        .flat_map(char::to_lowercase)
        .collect()
}

fn canonical_type_name(value: &str) -> Option<String> {
    let name = match normalized_token(value).as_str() {
        "normal" => "Normal",
        "underconstruction" => "UnderConstruction",
        "default" | "riverdefault" => "Default",
        "turn" | "riverturn" => "Turn",
        "intersectiont" | "riverintersectiont" => "IntersectionT",
        "intersectioncross" => "IntersectionCross",
        "deadend" | "riverdeadend" => "DeadEnd",
        "traincrossing" => "TrainCrossing",
        "bridge" => "Bridge",
        "tileautumn" => "TileAutumn",
        "tilesnow" => "TileSnow",
        "waterestuary" => "WaterEstuary",
        "intersectioncrosswithoutonecorner" | "waterintersectioncrosswithoutonecorner" => {
            "WaterIntersectionCrossWithoutOneCorner"
        }
        "isolated" => "Isolated",
        "waterintersectiont" => "WaterIntersectionT",
        "wateroffshore" => "WaterOffshore",
        "waterturn" => "WaterTurn",
        _ => return None,
    };
    Some(name.to_string())
}

fn canonical_direction_name(value: &str) -> Option<String> {
    let name = match value {
        "Northeast" => "Northeast",
        "Northwest" => "Northwest",
        "Southeast" => "Southeast",
        "Southwest" => "Southwest",
        _ => match normalized_token(value).as_str() {
            "none" | "normal" => "None",
            "west" | "left" => "West",
            "north" | "top" => "North",
            "east" | "right" => "East",
            "south" | "bottom" => "South",
            "eastwest" | "depth" => "EastWest",
            "northsouth" | "width" => "NorthSouth",
            "southeastwest" | "leftrightbottom" => "SouthEastWest",
            "northeastwest" | "leftrighttop" => "NorthEastWest",
            "northsouthwest" | "lefttopbottom" => "NorthSouthWest",
            "northsoutheast" | "righttopbottom" => "NorthSouthEast",
            "southwest" | "leftbottom" => "SouthWest",
            "northwest" | "lefttop" => "NorthWest",
            "southeast" | "rightbottom" => "SouthEast",
            "northeast" | "righttop" => "NorthEast",
            "all" => "All",
            "offshore" => "Offshore",
            "withoutsouthwestnorthwest" => "WithoutSouthwestNorthwest",
            "withoutsoutheastsouthwest" => "WithoutSoutheastSouthwest",
            "withoutnortheastnorthwest" => "WithoutNortheastNorthwest",
            "withoutnortheastsoutheast" => "WithoutNortheastSoutheast",
            "withouteast" => "WithoutEast",
            "withoutsouth" => "WithoutSouth",
            "withoutnorth" => "WithoutNorth",
            "withoutwest" => "WithoutWest",
            "withoutnorthwestnorthwest" => "WithoutNorthWestNorthwest",
            "withoutnorthnortheasteast" => "WithoutNorthNortheastEast",
            "withoutsouthsouthwestwest" => "WithoutSouthSouthwestWest",
            "withouteastsoutheastsouth" => "WithoutEastSoutheastSouth",
            "withoutnorthwest" => "WithoutNorthwest",
            "withoutnortheast" => "WithoutNortheast",
            "withoutsouthwest" | "withoutsouthsouthwest" => "WithoutSouthwest",
            "withoutsoutheast" => "WithoutSoutheast",
            _ => return None,
        },
    };
    Some(name.to_string())
}

fn collect_addon_files(path: &Path, output: &mut Vec<PathBuf>) -> io::Result<()> {
    for entry in fs::read_dir(path)? {
        let entry = entry?;
        let path = entry.path();
        if path.is_dir() {
            collect_addon_files(&path, output)?;
        } else if path
            .extension()
            .and_then(|value| value.to_str())
            .is_some_and(|value| value.eq_ignore_ascii_case("adj"))
        {
            output.push(path);
        }
    }
    Ok(())
}

fn load_addon_catalog(
    addon_set: &str,
    addon_root: &Path,
) -> Result<HashMap<String, AddonMetadata>, String> {
    let mut files = Vec::new();
    collect_addon_files(addon_root, &mut files)
        .map_err(|error| format!("アドオンフォルダを読み込めませんでした: {error}"))?;
    let mut catalog = HashMap::new();

    for path in files {
        let text = fs::read_to_string(&path).map_err(|error| {
            format!(
                "アドオン定義を読み込めませんでした ({}): {error}",
                path.display()
            )
        })?;
        let value: Value =
            serde_json::from_str(text.trim_start_matches('\u{feff}')).map_err(|error| {
                format!("アドオン定義のJSONが不正です ({}): {error}", path.display())
            })?;
        let belongs = value
            .get("Belong_addon_set_name")
            .and_then(Value::as_array)
            .is_some_and(|sets| sets.iter().any(|set| set.as_str() == Some(addon_set)));
        if !belongs {
            continue;
        }
        let Some(name) = value.get("name").and_then(Value::as_str) else {
            continue;
        };
        let mut metadata = AddonMetadata::default();
        if let Some(categories) = value.get("Categories").and_then(Value::as_array) {
            for category in categories.iter().filter_map(Value::as_str) {
                metadata.categories.insert(normalized_token(category));
            }
        }
        if let Some(types) = value.get("Types").and_then(Value::as_array) {
            for addon_type in types {
                let Some(type_name) = addon_type
                    .get("type_name")
                    .and_then(Value::as_str)
                    .and_then(canonical_type_name)
                else {
                    continue;
                };
                if let Some(directions) = addon_type.get("Directions").and_then(Value::as_array) {
                    for direction in directions {
                        let Some(direction_name) = direction
                            .get("direction_name")
                            .and_then(Value::as_str)
                            .and_then(canonical_direction_name)
                        else {
                            continue;
                        };
                        let width = direction
                            .pointer("/squares/width")
                            .and_then(Value::as_i64)
                            .unwrap_or(1) as i32;
                        let height = direction
                            .pointer("/squares/height")
                            .and_then(Value::as_i64)
                            .unwrap_or(1) as i32;
                        metadata
                            .tile_sizes
                            .insert((type_name.clone(), direction_name), (width, height));
                    }
                }
            }
        }
        catalog.insert(name.to_string(), metadata);
    }
    Ok(catalog)
}

fn xor_bytes(input: &[u8], key: &[u8]) -> Vec<u8> {
    input
        .iter()
        .enumerate()
        .map(|(index, byte)| byte ^ key[index % key.len()])
        .collect()
}

fn required_i32(value: &Value, pointer: &str) -> Result<i32, String> {
    value
        .pointer(pointer)
        .and_then(Value::as_i64)
        .and_then(|number| i32::try_from(number).ok())
        .ok_or_else(|| format!("項目 {pointer} が整数ではありません。"))
}

fn required_string<'a>(value: &'a Value, pointer: &str) -> Result<&'a str, String> {
    value
        .pointer(pointer)
        .and_then(Value::as_str)
        .ok_or_else(|| format!("項目 {pointer} が文字列ではありません。"))
}

fn normalized_array(value: Option<&Value>) -> Value {
    match value {
        Some(Value::Array(values)) => Value::Array(values.clone()),
        _ => Value::Array(Vec::new()),
    }
}

fn normalized_assignment_array(value: Option<&Value>, kind_key: &str) -> Value {
    let mut values = match normalized_array(value) {
        Value::Array(values) => values,
        _ => unreachable!(),
    };
    for value in &mut values {
        if let Some(object) = value.as_object_mut() {
            if let Some(kind) = object
                .get(kind_key)
                .and_then(Value::as_str)
                .and_then(|value| value.parse::<i32>().ok())
            {
                object.insert(kind_key.to_string(), json!(kind));
            }
        }
    }
    Value::Array(values)
}

fn normalized_reservation(value: Option<&Value>) -> i32 {
    value
        .and_then(Value::as_i64)
        .map(|number| number as i32)
        .or_else(|| {
            value
                .and_then(Value::as_str)
                .and_then(|text| text.parse().ok())
        })
        .unwrap_or(0)
}

fn normalized_bool(value: Option<&Value>, default: bool) -> bool {
    match value {
        Some(Value::Bool(value)) => *value,
        Some(Value::String(value)) if value.eq_ignore_ascii_case("true") => true,
        Some(Value::String(value)) if value.eq_ignore_ascii_case("false") => false,
        Some(Value::Number(value)) => value.as_i64().is_some_and(|value| value != 0),
        _ => default,
    }
}

fn normalize_condition(
    metadata: &AddonMetadata,
    source_version: i32,
    type_name: &str,
    direction_name: &str,
) -> Result<(String, String), String> {
    let mut object_type =
        canonical_type_name(type_name).ok_or_else(|| format!("不明なTypeIDです: {type_name}"))?;
    let mut direction = canonical_direction_name(direction_name)
        .ok_or_else(|| format!("不明なDirectionIDです: {direction_name}"))?;

    if source_version == 142 && metadata.categories.contains("waterway") {
        if object_type == "IntersectionCross" {
            object_type = "WaterOffshore".to_string();
            direction = "Offshore".to_string();
        } else if object_type == "IntersectionT" {
            object_type = "WaterIntersectionT".to_string();
        } else if object_type == "Turn" {
            object_type = "WaterTurn".to_string();
        }

        direction = match (object_type.as_str(), direction.as_str()) {
            ("WaterTurn", "NorthEast") => "WithoutNorthNortheastEast",
            ("WaterTurn", "SouthEast") => "WithoutEastSoutheastSouth",
            ("WaterTurn", "NorthWest") => "WithoutSouthSouthwestWest",
            ("WaterTurn", "SouthWest") => "WithoutNorthWestNorthwest",
            ("WaterIntersectionT", "NorthSouthEast") => "WithoutWest",
            ("WaterIntersectionT", "NorthSouthWest") => "WithoutEast",
            ("WaterIntersectionT", "NorthEastWest") => "WithoutSouth",
            ("WaterIntersectionT", "SouthEastWest") => "WithoutNorth",
            ("WaterEstuary", "NorthSouthEast") => "WithoutNortheastSoutheast",
            ("WaterEstuary", "NorthSouthWest") => "WithoutSouthwestNorthwest",
            ("WaterEstuary", "NorthEastWest") => "WithoutNortheastNorthwest",
            ("WaterEstuary", "SouthEastWest") => "WithoutSoutheastSouthwest",
            ("WaterIntersectionCrossWithoutOneCorner", "NorthEast") => "WithoutNortheast",
            ("WaterIntersectionCrossWithoutOneCorner", "SouthEast") => "WithoutSoutheast",
            ("WaterIntersectionCrossWithoutOneCorner", "NorthWest") => "WithoutNorthwest",
            ("WaterIntersectionCrossWithoutOneCorner", "SouthWest") => "WithoutSouthwest",
            _ => direction.as_str(),
        }
        .to_string();
    }
    Ok((object_type, direction))
}

fn convert_legacy_map(
    mut root: Value,
    catalog: &HashMap<String, AddonMetadata>,
) -> Result<Value, String> {
    let width = required_i32(&root, "/Map_size/width")?;
    let height = required_i32(&root, "/Map_size/height")?;
    let rows = root
        .get("Map")
        .and_then(Value::as_array)
        .ok_or_else(|| "項目 /Map が配列ではありません。".to_string())?;
    if rows.len() != height as usize {
        return Err("Mapの行数がMap_size.heightと一致しません。".to_string());
    }

    let mut next_object_id = 0_i32;
    let mut used_ids = HashSet::new();
    let mut origin_ids: HashMap<(i32, i32, String), i32> = HashMap::new();
    let mut objects = Vec::new();
    let mut output_rows = Vec::with_capacity(height as usize);

    for (y, row) in rows.iter().enumerate() {
        let tiles = row
            .as_array()
            .ok_or_else(|| format!("Map[{y}] が配列ではありません。"))?;
        if tiles.len() != width as usize {
            return Err(format!("Map[{y}]の列数がMap_size.widthと一致しません。"));
        }
        let mut output_tiles = Vec::with_capacity(width as usize);
        for (x, tile) in tiles.iter().enumerate() {
            let tile_object = tile
                .as_object()
                .ok_or_else(|| format!("Map[{y}][{x}] がオブジェクトではありません。"))?;
            let stored_serial = tile_object
                .get("serial_number")
                .and_then(Value::as_i64)
                .unwrap_or(0) as i32;
            let stored_relative_x = tile
                .pointer("/tiles_count/x")
                .and_then(Value::as_i64)
                .unwrap_or(0) as i32;
            let stored_relative_y = tile
                .pointer("/tiles_count/y")
                .and_then(Value::as_i64)
                .unwrap_or(0) as i32;
            let original_name = tile_object
                .get("original_name")
                .and_then(Value::as_str)
                .unwrap_or("")
                .to_string();
            let mut tile_objects = Vec::new();

            if let Some(addons) = tile_object.get("addons").and_then(Value::as_array) {
                for addon in addons {
                    let addon_name =
                        addon.get("name").and_then(Value::as_str).ok_or_else(|| {
                            format!("Map[{y}][{x}].addons.name が文字列ではありません。")
                        })?;
                    let metadata = catalog.get(addon_name).ok_or_else(|| {
                        format!("必要なアドオン「{addon_name}」が見つかりません。")
                    })?;
                    let raw_type = addon
                        .get("type_number")
                        .and_then(Value::as_str)
                        .ok_or_else(|| {
                            format!("Map[{y}][{x}].addons.type_number が文字列ではありません。")
                        })?;
                    let raw_direction = addon
                        .get("direction_number")
                        .and_then(Value::as_str)
                        .ok_or_else(|| {
                            format!(
                                "Map[{y}][{x}].addons.direction_number が文字列ではありません。"
                            )
                        })?;
                    let (type_name, mut direction_name) =
                        normalize_condition(metadata, 141, raw_type, raw_direction)?;
                    if direction_name == "None" && type_name == "IntersectionCross" {
                        direction_name = "All".to_string();
                    }

                    let mut relative_y = stored_relative_y;
                    if direction_name != "West" {
                        let (_, use_height) = metadata.tile_sizes.get(&(type_name.clone(), direction_name.clone()))
                            .or_else(|| metadata.tile_sizes.get(&(type_name.clone(), "None".to_string())))
                            .copied().or_else(|| {
                                (stored_relative_x == 0 && stored_relative_y == 0).then_some((1, 1))
                            }).ok_or_else(|| format!("アドオン「{addon_name}」の {type_name}/{direction_name} 定義がありません。"))?;
                        if use_height > 1 {
                            relative_y = use_height - 1 - relative_y;
                        }
                    }
                    let relative_x = stored_relative_x;
                    let origin_x = x as i32 - relative_x;
                    let origin_y = y as i32 - relative_y;
                    if origin_x < 0 || origin_y < 0 || origin_x >= width || origin_y >= height {
                        return Err(format!(
                            "Map[{y}][{x}] の旧形式オブジェクト原点が範囲外です。"
                        ));
                    }

                    let origin_key = (origin_x, origin_y, addon_name.to_string());
                    let object_id = if relative_x == 0 && relative_y == 0 {
                        let id = if stored_serial <= 0 || used_ids.contains(&stored_serial) {
                            next_object_id += 1;
                            while used_ids.contains(&next_object_id) {
                                next_object_id += 1;
                            }
                            next_object_id
                        } else {
                            next_object_id = next_object_id.max(stored_serial);
                            stored_serial
                        };
                        used_ids.insert(id);
                        origin_ids.insert(origin_key, id);
                        objects.push(json!({
                            "objectID": id,
                            "addon_name": addon_name,
                            "original_name": original_name,
                            "typeID": type_name,
                            "directionID": direction_name,
                            "origin_coordinate": { "x": origin_x, "y": origin_y },
                            "visible": true
                        }));
                        id
                    } else {
                        *origin_ids.get(&origin_key).ok_or_else(|| {
                            format!("Map[{y}][{x}] の旧形式オブジェクト原点を解決できませんでした ({addon_name})。")
                        })?
                    };
                    tile_objects.push(json!({
                        "objectID": object_id,
                        "relative_coordinate": { "x": relative_x, "y": relative_y },
                        "visible": true
                    }));
                }
            }

            output_tiles.push(json!({
                "coordinate": { "x": x as i32, "y": y as i32 },
                "objects": tile_objects,
                "residents": tile_object.get("residents").cloned().unwrap_or(json!(0)),
                "workers": tile_object.get("workers").cloned().unwrap_or_else(|| json!({"commercial":0,"office":0,"industrial":0,"farm":0,"public":0})),
                "students": tile_object.get("students").cloned().unwrap_or(json!(0)),
                "rate": tile_object.get("rate").cloned().filter(|value| value.is_object()).unwrap_or_else(|| json!({})),
                "age": normalized_array(tile_object.get("age")),
                "gender": normalized_array(tile_object.get("gender")),
                "work_places": normalized_assignment_array(tile_object.get("work_places"), "work_kind"),
                "school": normalized_assignment_array(tile_object.get("school"), "school_kind"),
                "reservation": normalized_reservation(tile_object.get("reservation")),
                "original_name": original_name
            }));
        }
        output_rows.push(Value::Array(output_tiles));
    }

    let object = root
        .as_object_mut()
        .ok_or_else(|| "ルートJSONがオブジェクトではありません。".to_string())?;
    object.insert("Version".to_string(), json!(142));
    let change_weather = normalized_bool(object.get("Change_Weather"), true);
    let dark_on_night = normalized_bool(object.get("Dark_on_Night"), true);
    object.insert("Change_Weather".to_string(), Value::Bool(change_weather));
    object.insert("Dark_on_Night".to_string(), Value::Bool(dark_on_night));
    object.insert("Objects".to_string(), Value::Array(objects));
    object.insert("Map".to_string(), Value::Array(output_rows));
    Ok(root)
}

fn normalize_current_map(
    mut root: Value,
    catalog: &HashMap<String, AddonMetadata>,
) -> Result<Value, String> {
    let objects = root
        .get_mut("Objects")
        .and_then(Value::as_array_mut)
        .ok_or_else(|| "項目 /Objects が配列ではありません。".to_string())?;
    for (index, object) in objects.iter_mut().enumerate() {
        let addon_name = object
            .get("addon_name")
            .and_then(Value::as_str)
            .ok_or_else(|| format!("Objects[{index}].addon_name が文字列ではありません。"))?
            .to_string();
        let metadata = catalog
            .get(&addon_name)
            .ok_or_else(|| format!("必要なアドオン「{addon_name}」が見つかりません。"))?;
        let raw_type = object
            .get("typeID")
            .and_then(Value::as_str)
            .ok_or_else(|| format!("Objects[{index}].typeID が文字列ではありません。"))?
            .to_string();
        let raw_direction = object
            .get("directionID")
            .and_then(Value::as_str)
            .ok_or_else(|| format!("Objects[{index}].directionID が文字列ではありません。"))?
            .to_string();
        let (type_name, direction_name) =
            normalize_condition(metadata, 142, &raw_type, &raw_direction)?;
        let object_map = object
            .as_object_mut()
            .ok_or_else(|| format!("Objects[{index}] がオブジェクトではありません。"))?;
        object_map.insert("typeID".to_string(), Value::String(type_name));
        object_map.insert("directionID".to_string(), Value::String(direction_name));
        object_map
            .entry("visible".to_string())
            .or_insert(Value::Bool(true));
    }

    let rows = root
        .get_mut("Map")
        .and_then(Value::as_array_mut)
        .ok_or_else(|| "項目 /Map が配列ではありません。".to_string())?;
    for row in rows {
        let tiles = row
            .as_array_mut()
            .ok_or_else(|| "Mapの行が配列ではありません。".to_string())?;
        for tile in tiles {
            let tile = tile
                .as_object_mut()
                .ok_or_else(|| "Mapのタイルがオブジェクトではありません。".to_string())?;
            for key in ["objects", "age", "gender", "work_places", "school"] {
                if !tile.get(key).is_some_and(Value::is_array) {
                    tile.insert(key.to_string(), Value::Array(Vec::new()));
                }
            }
            if !tile.get("rate").is_some_and(Value::is_object) {
                tile.insert("rate".to_string(), json!({}));
            }
            let reservation = normalized_reservation(tile.get("reservation"));
            let work_places = normalized_assignment_array(tile.get("work_places"), "work_kind");
            let schools = normalized_assignment_array(tile.get("school"), "school_kind");
            tile.insert("reservation".to_string(), json!(reservation));
            tile.insert("work_places".to_string(), work_places);
            tile.insert("school".to_string(), schools);
            if let Some(object_refs) = tile.get_mut("objects").and_then(Value::as_array_mut) {
                for object_ref in object_refs {
                    if let Some(map) = object_ref.as_object_mut() {
                        map.entry("visible".to_string())
                            .or_insert(Value::Bool(true));
                    }
                }
            }
        }
    }
    Ok(root)
}

fn validate_save_data(
    data: &SaveDataJson,
    catalog: &HashMap<String, AddonMetadata>,
) -> Result<(), String> {
    const MAX_MAP_SIDE: i32 = 4096;
    const MAX_MAP_TILES: i64 = 4_000_000;
    let width = data.map_size.width;
    let height = data.map_size.height;
    if width <= 0
        || height <= 0
        || width > MAX_MAP_SIDE
        || height > MAX_MAP_SIDE
        || i64::from(width) * i64::from(height) > MAX_MAP_TILES
    {
        return Err(format!(
            "マップサイズが許容範囲外です ({width} x {height})。"
        ));
    }
    if data.map.len() != height as usize || data.map.iter().any(|row| row.len() != width as usize) {
        return Err("Mapの配列形状がMap_sizeと一致しません。".to_string());
    }

    let mut ids = HashSet::new();
    for object in &data.objects {
        if object.object_id < 0 || !ids.insert(object.object_id) {
            return Err(format!(
                "ObjectID {} が不正または重複しています。",
                object.object_id
            ));
        }
        if !catalog.contains_key(&object.addon_name) {
            return Err(format!(
                "必要なアドオン「{}」が見つかりません。",
                object.addon_name
            ));
        }
        if canonical_type_name(&object.type_id).is_none()
            || canonical_direction_name(&object.direction_id).is_none()
        {
            return Err(format!(
                "ObjectID {} のTypeIDまたはDirectionIDが不正です。",
                object.object_id
            ));
        }
        if object.origin_coordinate.x < 0
            || object.origin_coordinate.y < 0
            || object.origin_coordinate.x >= width
            || object.origin_coordinate.y >= height
        {
            return Err(format!(
                "ObjectID {} の原点座標が範囲外です。",
                object.object_id
            ));
        }
    }
    for (y, row) in data.map.iter().enumerate() {
        for (x, tile) in row.iter().enumerate() {
            for object in &tile.objects {
                if !ids.contains(&object.object_id) {
                    return Err(format!(
                        "Map[{y}][{x}] が存在しないObjectID {}を参照しています。",
                        object.object_id
                    ));
                }
            }
        }
    }
    Ok(())
}

fn read_and_normalize_map(
    path: &Path,
) -> Result<(SaveDataJson, HashMap<String, AddonMetadata>), String> {
    let encrypted =
        fs::read(path).map_err(|error| format!("セーブファイルを開けませんでした: {error}"))?;
    if encrypted.is_empty() {
        return Err("セーブファイルが空です。".to_string());
    }
    let decrypted = xor_bytes(&encrypted, b"citiesboxmapdatafilexor");
    let root: Value = serde_json::from_slice(&decrypted)
        .map_err(|error| format!("復号後のJSONを解析できませんでした: {error}"))?;
    let version = required_i32(&root, "/Version")?;
    if !(1..=142).contains(&version) {
        return Err(format!("未対応のセーブデータバージョンです ({version})。"));
    }
    let addon_set = required_string(&root, "/Addon_Set")?.to_string();
    let map_relative_addons = path
        .parent()
        .and_then(Path::parent)
        .and_then(Path::parent)
        .map(|app_root| app_root.join("addons"));
    let addon_root = if Path::new("./addons").is_dir() {
        PathBuf::from("./addons")
    } else {
        map_relative_addons
            .ok_or_else(|| "アドオンフォルダの場所を特定できませんでした。".to_string())?
    };
    let catalog = load_addon_catalog(&addon_set, &addon_root)?;
    if catalog.is_empty() {
        return Err(format!(
            "アドオンセット「{addon_set}」を読み込めませんでした。"
        ));
    }
    let normalized = if version <= 141 {
        convert_legacy_map(root, &catalog)?
    } else {
        normalize_current_map(root, &catalog)?
    };
    let data: SaveDataJson = serde_json::from_value(normalized)
        .map_err(|error| format!("セーブデータの構造が不正です: {error}"))?;
    validate_save_data(&data, &catalog)?;
    Ok((data, catalog))
}

#[repr(C)] // C++とメモリレイアウトを完全に一致させる
pub struct RawTileData {
    pub residents: i32,
    pub workers_commercial: i32,
    pub workers_office: i32,
    pub workers_industrial: i32,
    pub workers_farm: i32,
    pub workers_public: i32,
    pub students: i32,
    pub reservation: i32,
}

#[cxx::bridge(namespace = "rust::citymap")]
mod ffi {
    // Rust側の構造体をC++に見せる
    struct RawTileData {
        residents: i32,
        workers_commercial: i32,
        workers_office: i32,
        workers_industrial: i32,
        workers_farm: i32,
        workers_public: i32,
        students: i32,
        reservation: i32,
    }

    // C++ 側の構造体を定義（POD: Plain Old Data として）
    #[derive(Clone)]
    struct TimeStruct {
        year: i32,
        month: i32,
        date: i32,
        hour: i32,
        minutes: i32,
    }

    #[derive(Clone)]
    struct RCOIFstruct {
        residential: f64,
        commercial: f64,
        office: f64,
        industrial: f64,
        farm: f64,
    }

    struct LoadedObjectData {
        id: i32,
        addon_name: String,
        original_name: String,
        type_name: String,
        direction_name: String,
        origin_x: i32,
        origin_y: i32,
        visible: bool,
    }

    struct LoadedTileObjectData {
        object_id: i32,
        relative_x: i32,
        relative_y: i32,
    }

    struct LoadedRateData {
        name: String,
        value: i32,
    }

    struct LoadedWorkPlaceData {
        kind: i32,
        serial_number: i32,
    }

    struct LoadedSchoolData {
        kind: i32,
        serial_number: i32,
    }

    struct LoadedTileData {
        residents: i32,
        workers_commercial: i32,
        workers_office: i32,
        workers_industrial: i32,
        workers_farm: i32,
        workers_public: i32,
        students: i32,
        reservation: i32,
        original_name: String,
        ages: Vec<i32>,
        genders: Vec<String>,
        objects: Vec<LoadedTileObjectData>,
        rates: Vec<LoadedRateData>,
        work_places: Vec<LoadedWorkPlaceData>,
        schools: Vec<LoadedSchoolData>,
    }

    extern "Rust" {
        type RustCityMap;

        fn new_city_map() -> Box<RustCityMap>;

        // セーブデータの読み込み・検証・現行形式への変換
        fn load_from_file(&mut self, path: String) -> bool;
        fn get_last_load_error(&self) -> String;
        fn get_city_name(&self) -> String;
        fn get_mayor_name(&self) -> String;
        fn get_addon_set_name(&self) -> String;
        fn get_change_weather(&self) -> bool;
        fn get_dark_on_night(&self) -> bool;
        fn get_map_width(&self) -> i32;
        fn get_map_height(&self) -> i32;
        fn get_budget_police(&self) -> i32;
        fn get_budget_fire(&self) -> i32;
        fn get_budget_post(&self) -> i32;
        fn get_budget_education(&self) -> i32;
        fn get_tax_residential(&self) -> f64;
        fn get_tax_commercial(&self) -> f64;
        fn get_tax_office(&self) -> f64;
        fn get_tax_industrial(&self) -> f64;
        fn get_tax_farm(&self) -> f64;
        fn get_loaded_objects(&self) -> Vec<LoadedObjectData>;
        fn get_loaded_tiles(&self) -> Vec<LoadedTileData>;

        // 初期化
        fn set_status(
            &mut self,
            pop: i32,
            money: i32,
            temp: i32,
            time: TimeStruct,
            demand: RCOIFstruct,
        );

        // マップサイズの初期化（タイルのメモリ確保）
        fn init_map_size(&mut self, width: i32, height: i32);

        // オブジェクトの登録
        fn clear_objects(&mut self);
        fn add_object(
            &mut self,
            id: i32,
            addon_en: String,
            orig_name: String,
            type_id: String,
            dir_id: String,
            x: i32,
            y: i32,
            visible: bool,
        );

        // タイルデータの詳細設定
        fn set_tile_basic(
            &mut self,
            x: i32,
            y: i32,
            residents: i32,
            students: i32,
            reservation: i32,
            orig_name: String,
        );
        fn set_tile_workers(
            &mut self,
            x: i32,
            y: i32,
            comm: i32,
            offi: i32,
            indu: i32,
            farm: i32,
            publ: i32,
        );
        fn add_tile_object_ref(
            &mut self,
            x: i32,
            y: i32,
            obj_id: i32,
            rel_x: i32,
            rel_y: i32,
            visible: bool,
        );
        fn set_tile_stats(&mut self, x: i32, y: i32, ages: Vec<i32>, genders: Vec<String>);
        fn add_tile_rate(&mut self, x: i32, y: i32, key: String, value: i32);
        fn add_tile_work_place(&mut self, x: i32, y: i32, kind: i32, serial_number: i32);
        fn add_tile_school(&mut self, x: i32, y: i32, kind: i32, serial_number: i32);

        // ステータス取得
        fn get_population(&self) -> i32;
        fn get_money(&self) -> i32;
        fn get_temperature(&self) -> i32;
        fn get_demand(&self) -> RCOIFstruct;

        // 時間進行
        fn city_time(&mut self, minutes_delta: i32) -> TimeStruct;
        fn update_world(&mut self, minutes_delta: i32) -> TimeStruct;

        // 基本情報の同期用
        fn set_save_version(&mut self, version: i32);
        fn set_city_metadata(&mut self, city_name: String, mayor_name: String, addon_set: String);
        fn set_financial_data(&mut self, money: i32, population: i32);

        // 予算と税率 (一括で渡すと効率的)
        fn set_budget(&mut self, police: i32, fire: i32, post: i32, edu: i32);
        fn set_tax(&mut self, r: f64, c: f64, o: f64, i: f64, f: f64);

        // 時間と天候の設定
        fn set_environment(
            &mut self,
            year: i32,
            month: i32,
            date: i32,
            hour: i32,
            min: i32,
            weather: bool,
            night: bool,
        );

        // セーブデータ生成
        fn generate_save_json(&self) -> String;

        // スライス（配列の参照）として一括で受け取る
        fn bulk_set_tiles(&mut self, data: &[RawTileData], width: i32, height: i32);
        // 保存まで実行
        fn save_to_file(&self, path: String) -> bool;
    }
}

pub struct RustCityMap {
    // 需要度
    demand: ffi::RCOIFstruct,

    // 時刻
    time: ffi::TimeStruct,

    // 都市の基本情報
    pub version: i32,
    pub addon_set_name: String,
    pub city_name: String,
    pub mayor_name: String,
    pub population: i32,
    pub temperature: i32,
    pub money: i32,

    // 設定
    pub change_weather: bool,
    pub dark_on_night: bool,

    // マップ形状
    pub map_size: [i32; 2], // [width, height]

    // 予算
    pub budget_police: i32,
    pub budget_fire: i32,
    pub budget_post: i32,
    pub budget_education: i32,

    // 税率
    pub tax_residential: f64,
    pub tax_commercial: f64,
    pub tax_office: f64,
    pub tax_industrial: f64,
    pub tax_farm: f64,

    // オブジェクト管理 (m_objects)
    pub objects: HashMap<i32, RustObject>, // Key : ObjectID

    // タイルデータ
    pub tiles: Vec<Vec<RustTile>>,

    pub last_load_error: String,
}

impl RustCityMap {
    fn load_from_file(&mut self, path: String) -> bool {
        match read_and_normalize_map(Path::new(&path)) {
            Ok((data, _catalog)) => {
                self.apply_loaded_data(data);
                self.last_load_error.clear();
                true
            }
            Err(error) => {
                self.last_load_error = error;
                false
            }
        }
    }

    fn apply_loaded_data(&mut self, data: SaveDataJson) {
        self.version = data.version;
        self.addon_set_name = data.addon_set;
        self.city_name = data.city_name;
        self.mayor_name = data.mayor_name;
        self.population = data.total_population;
        self.change_weather = data.change_weather;
        self.temperature = data.temperature;
        self.dark_on_night = data.dark_on_night;
        self.map_size = [data.map_size.width, data.map_size.height];
        self.time = ffi::TimeStruct {
            year: data.time.year,
            month: data.time.month,
            date: data.time.date,
            hour: data.time.hour,
            minutes: data.time.minutes,
        };
        self.demand = ffi::RCOIFstruct {
            residential: data.demand.residential,
            commercial: data.demand.commercial,
            office: data.demand.office,
            industrial: data.demand.industrial,
            farm: data.demand.farm,
        };
        self.money = data.money;
        self.budget_police = data.budget.police;
        self.budget_fire = data.budget.fire_depertment;
        self.budget_post = data.budget.post_office;
        self.budget_education = data.budget.education;
        self.tax_residential = data.tax.residential;
        self.tax_commercial = data.tax.commercial;
        self.tax_office = data.tax.office;
        self.tax_industrial = data.tax.industrial;
        self.tax_farm = data.tax.farm;

        self.objects = data
            .objects
            .into_iter()
            .map(|object| {
                let id = object.object_id;
                (
                    id,
                    RustObject {
                        id,
                        addon_name_en: object.addon_name,
                        original_name: object.original_name,
                        type_id: object.type_id,
                        direction_id: object.direction_id,
                        origin_coordinate: object.origin_coordinate,
                        visible: object.visible,
                    },
                )
            })
            .collect();

        self.tiles = data
            .map
            .into_iter()
            .map(|row| {
                row.into_iter()
                    .map(|tile| RustTile {
                        residents: tile.residents,
                        workers_commercial: tile.workers.commercial,
                        workers_office: tile.workers.office,
                        workers_industrial: tile.workers.industrial,
                        workers_farm: tile.workers.farm,
                        workers_public: tile.workers.public,
                        students: tile.students,
                        reservation: tile.reservation,
                        original_name: tile.original_name,
                        rate: tile.rate,
                        age: tile.age,
                        gender: tile.gender,
                        work_places: tile
                            .work_places
                            .into_iter()
                            .map(|workplace| RustWorkPlace {
                                kind: workplace.work_kind,
                                serial_number: workplace.serial_number,
                            })
                            .collect(),
                        schools: tile
                            .school
                            .into_iter()
                            .map(|school| RustSchool {
                                kind: school.school_kind,
                                serial_number: school.serial_number,
                            })
                            .collect(),
                        object_structs: tile
                            .objects
                            .into_iter()
                            .map(|object| RustTileObjectReference {
                                object_id: object.object_id,
                                relative_x: object.relative_coordinate.x,
                                relative_y: object.relative_coordinate.y,
                                visible: object.visible,
                            })
                            .collect(),
                    })
                    .collect()
            })
            .collect();
    }

    fn get_last_load_error(&self) -> String {
        self.last_load_error.clone()
    }
    fn get_city_name(&self) -> String {
        self.city_name.clone()
    }
    fn get_mayor_name(&self) -> String {
        self.mayor_name.clone()
    }
    fn get_addon_set_name(&self) -> String {
        self.addon_set_name.clone()
    }
    fn get_change_weather(&self) -> bool {
        self.change_weather
    }
    fn get_dark_on_night(&self) -> bool {
        self.dark_on_night
    }
    fn get_map_width(&self) -> i32 {
        self.map_size[0]
    }
    fn get_map_height(&self) -> i32 {
        self.map_size[1]
    }
    fn get_budget_police(&self) -> i32 {
        self.budget_police
    }
    fn get_budget_fire(&self) -> i32 {
        self.budget_fire
    }
    fn get_budget_post(&self) -> i32 {
        self.budget_post
    }
    fn get_budget_education(&self) -> i32 {
        self.budget_education
    }
    fn get_tax_residential(&self) -> f64 {
        self.tax_residential
    }
    fn get_tax_commercial(&self) -> f64 {
        self.tax_commercial
    }
    fn get_tax_office(&self) -> f64 {
        self.tax_office
    }
    fn get_tax_industrial(&self) -> f64 {
        self.tax_industrial
    }
    fn get_tax_farm(&self) -> f64 {
        self.tax_farm
    }

    fn get_loaded_objects(&self) -> Vec<ffi::LoadedObjectData> {
        let mut objects: Vec<_> = self.objects.values().collect();
        objects.sort_by_key(|object| object.id);
        objects
            .into_iter()
            .map(|object| ffi::LoadedObjectData {
                id: object.id,
                addon_name: object.addon_name_en.clone(),
                original_name: object.original_name.clone(),
                type_name: object.type_id.clone(),
                direction_name: object.direction_id.clone(),
                origin_x: object.origin_coordinate.x,
                origin_y: object.origin_coordinate.y,
                visible: object.visible,
            })
            .collect()
    }

    fn get_loaded_tiles(&self) -> Vec<ffi::LoadedTileData> {
        self.tiles
            .iter()
            .flat_map(|row| row.iter())
            .map(|tile| {
                let mut rates: Vec<_> = tile.rate.iter().collect();
                rates.sort_by(|left, right| left.0.cmp(right.0));
                ffi::LoadedTileData {
                    residents: tile.residents,
                    workers_commercial: tile.workers_commercial,
                    workers_office: tile.workers_office,
                    workers_industrial: tile.workers_industrial,
                    workers_farm: tile.workers_farm,
                    workers_public: tile.workers_public,
                    students: tile.students,
                    reservation: tile.reservation,
                    original_name: tile.original_name.clone(),
                    ages: tile.age.clone(),
                    genders: tile.gender.clone(),
                    objects: tile
                        .object_structs
                        .iter()
                        .map(|object| ffi::LoadedTileObjectData {
                            object_id: object.object_id,
                            relative_x: object.relative_x,
                            relative_y: object.relative_y,
                        })
                        .collect(),
                    rates: rates
                        .into_iter()
                        .map(|(name, value)| ffi::LoadedRateData {
                            name: name.clone(),
                            value: *value,
                        })
                        .collect(),
                    work_places: tile
                        .work_places
                        .iter()
                        .map(|workplace| ffi::LoadedWorkPlaceData {
                            kind: workplace.kind,
                            serial_number: workplace.serial_number,
                        })
                        .collect(),
                    schools: tile
                        .schools
                        .iter()
                        .map(|school| ffi::LoadedSchoolData {
                            kind: school.kind,
                            serial_number: school.serial_number,
                        })
                        .collect(),
                }
            })
            .collect()
    }

    // スライス（配列の参照）として一括で受け取る
    fn bulk_set_tiles(&mut self, data: &[ffi::RawTileData], width: i32, height: i32) {
        self.map_size = [width, height];
        // データを流し込む（統計やレート等は別途Setterを呼ぶか、一旦クリア）
        self.tiles = data
            .iter()
            .enumerate()
            .map(|(i, d)| {
                let mut tile = RustTile::default((i as i32) % width, (i as i32) / width);
                tile.residents = d.residents;
                tile.workers_commercial = d.workers_commercial;
                tile.workers_office = d.workers_office;
                tile.workers_industrial = d.workers_industrial;
                tile.workers_farm = d.workers_farm;
                tile.workers_public = d.workers_public;
                tile.students = d.students;
                tile.reservation = d.reservation;
                tile
            })
            .collect::<Vec<_>>() // 一旦フラットなリストとして作り
            .chunks(width as usize) // widthごとに分割して Vec<Vec<RustTile>> に
            .map(|chunk| chunk.to_vec())
            .collect();
    }

    // 初期化
    fn set_status(
        &mut self,
        pop: i32,
        money: i32,
        temp: i32,
        time: ffi::TimeStruct,
        demand: ffi::RCOIFstruct,
    ) {
        self.demand = demand;
        self.population = pop;
        self.money = money;
        self.temperature = temp;
        self.time = time;
    }

    // マップがロードされた時にまず呼ぶ
    fn init_map_size(&mut self, width: i32, height: i32) {
        self.map_size = [width, height];
        self.tiles = (0..height)
            .map(|y| (0..width).map(|x| RustTile::default(x, y)).collect())
            .collect();
    }

    fn add_object(
        &mut self,
        id: i32,
        addon_en: String,
        orig_name: String,
        type_id: String,
        dir_id: String,
        x: i32,
        y: i32,
        visible: bool,
    ) {
        self.objects.insert(
            id,
            RustObject {
                id,
                addon_name_en: addon_en,
                original_name: orig_name,
                type_id,
                direction_id: dir_id,
                origin_coordinate: CoordinateJson { x, y },
                visible,
            },
        );
    }

    fn set_tile_basic(
        &mut self,
        x: i32,
        y: i32,
        residents: i32,
        students: i32,
        reservation: i32,
        orig_name: String,
    ) {
        if let Some(tile) = self
            .tiles
            .get_mut(y as usize)
            .and_then(|row| row.get_mut(x as usize))
        {
            tile.residents = residents;
            tile.students = students;
            tile.reservation = reservation;
            tile.original_name = orig_name;
        }
    }

    fn add_tile_object_ref(
        &mut self,
        x: i32,
        y: i32,
        obj_id: i32,
        rel_x: i32,
        rel_y: i32,
        visible: bool,
    ) {
        if let Some(tile) = self
            .tiles
            .get_mut(y as usize)
            .and_then(|row| row.get_mut(x as usize))
        {
            tile.object_structs.push(RustTileObjectReference {
                object_id: obj_id,
                relative_x: rel_x,
                relative_y: rel_y,
                visible,
            });
        }
    }

    fn add_tile_rate(&mut self, x: i32, y: i32, key: String, value: i32) {
        if let Some(tile) = self
            .tiles
            .get_mut(y as usize)
            .and_then(|row| row.get_mut(x as usize))
        {
            tile.rate.insert(key, value);
        }
    }

    // オブジェクトリストを空にする（同期の開始時に呼ぶ）
    fn clear_objects(&mut self) {
        self.objects.clear();
    }

    // タイルの労働者情報を一括設定
    fn set_tile_workers(
        &mut self,
        x: i32,
        y: i32,
        comm: i32,
        offi: i32,
        indu: i32,
        farm: i32,
        publ: i32,
    ) {
        if let Some(tile) = self
            .tiles
            .get_mut(y as usize)
            .and_then(|row| row.get_mut(x as usize))
        {
            tile.workers_commercial = comm;
            tile.workers_office = offi;
            tile.workers_industrial = indu;
            tile.workers_farm = farm;
            tile.workers_public = publ;
        }
    }

    // タイルの統計データ（年齢・性別）を設定
    // cxxbridge を介して Vec<i32> を直接受け取る
    fn set_tile_stats(&mut self, x: i32, y: i32, ages: Vec<i32>, genders: Vec<String>) {
        if let Some(tile) = self
            .tiles
            .get_mut(y as usize)
            .and_then(|row| row.get_mut(x as usize))
        {
            tile.age = ages;
            tile.gender = genders;
        }
    }

    fn add_tile_work_place(&mut self, x: i32, y: i32, kind: i32, serial_number: i32) {
        if let Some(tile) = self
            .tiles
            .get_mut(y as usize)
            .and_then(|row| row.get_mut(x as usize))
        {
            tile.work_places.push(RustWorkPlace {
                kind,
                serial_number,
            });
        }
    }

    fn add_tile_school(&mut self, x: i32, y: i32, kind: i32, serial_number: i32) {
        if let Some(tile) = self
            .tiles
            .get_mut(y as usize)
            .and_then(|row| row.get_mut(x as usize))
        {
            tile.schools.push(RustSchool {
                kind,
                serial_number,
            });
        }
    }

    // ステータス取得
    fn get_population(&self) -> i32 {
        self.population
    }
    fn get_money(&self) -> i32 {
        self.money
    }
    fn get_temperature(&self) -> i32 {
        self.temperature
    }
    fn get_demand(&self) -> ffi::RCOIFstruct {
        self.demand.clone()
    }

    // 時間進行
    fn city_time(&mut self, minutes_delta: i32) -> ffi::TimeStruct {
        self.time.city_time(minutes_delta)
    }
    // 時間進行 + 需要度更新
    fn update_world(&mut self, minutes_delta: i32) -> ffi::TimeStruct {
        let previous_date = self.time.date;
        let new_time = self.time.city_time(minutes_delta);
        self.time = new_time;

        // 日付が変わった場合、需要度を更新するロジックをここに追加
        if self.time.date != previous_date {
            // 需要度の更新ロジック（例としてランダムに変化させる）
            use rand::Rng;
            let mut rng = rand::thread_rng();
            self.demand.residential += rng.gen_range(-5.0..5.0);
            self.demand.commercial += rng.gen_range(-5.0..5.0);
            self.demand.office += rng.gen_range(-5.0..5.0);
            self.demand.industrial += rng.gen_range(-5.0..5.0);
            self.demand.farm += rng.gen_range(-5.0..5.0);

            // 需要度の範囲を制限（例: 0.0 から 100.0 の間）
            self.demand.residential = self.demand.residential.clamp(0.0, 100.0);
            self.demand.commercial = self.demand.commercial.clamp(0.0, 100.0);
            self.demand.office = self.demand.office.clamp(0.0, 100.0);
            self.demand.industrial = self.demand.industrial.clamp(0.0, 100.0);
            self.demand.farm = self.demand.farm.clamp(0.0, 100.0);
        }

        self.time.clone()
    }

    // マップデータの設定・取得
    fn set_save_version(&mut self, version: i32) {
        self.version = version;
    }

    fn set_city_metadata(&mut self, city_name: String, mayor_name: String, addon_set: String) {
        self.city_name = city_name;
        self.mayor_name = mayor_name;
        self.addon_set_name = addon_set;
    }

    fn set_financial_data(&mut self, money: i32, population: i32) {
        self.money = money;
        self.population = population;
    }

    fn set_budget(&mut self, police: i32, fire: i32, post: i32, edu: i32) {
        self.budget_police = police;
        self.budget_fire = fire;
        self.budget_post = post;
        self.budget_education = edu;
    }

    fn set_tax(&mut self, r: f64, c: f64, o: f64, i: f64, f: f64) {
        self.tax_residential = r;
        self.tax_commercial = c;
        self.tax_office = o;
        self.tax_industrial = i;
        self.tax_farm = f;
    }

    fn set_environment(
        &mut self,
        year: i32,
        month: i32,
        date: i32,
        hour: i32,
        min: i32,
        weather: bool,
        night: bool,
    ) {
        self.time.year = year;
        self.time.month = month;
        self.time.date = date;
        self.time.hour = hour;
        self.time.minutes = min;
        self.change_weather = weather;
        self.dark_on_night = night;
    }

    // セーブデータ JSON 文字列生成
    pub fn generate_save_json(&self) -> String {
        // 1. オブジェクトデータの集計 (m_objects 相当)
        let objects_json: Vec<ObjectEntryJson> = self
            .objects
            .iter()
            .map(|(id, obj)| ObjectEntryJson {
                object_id: *id,
                addon_name: obj.get_addon_name_en(),
                original_name: obj.get_original_name(),
                type_id: obj.type_id.clone(),
                direction_id: obj.direction_id.clone(),
                origin_coordinate: CoordinateJson {
                    x: obj.origin_coordinate.x,
                    y: obj.origin_coordinate.y,
                },
                visible: obj.visible,
            })
            .collect();

        // 2. マップタイルデータの集計 (m_tiles[y][x] 相当)
        let mut map_json = Vec::new();
        for y in 0..self.map_size[1] {
            let mut row_json = Vec::new();
            for x in 0..self.map_size[0] {
                let tile = &self.tiles[y as usize][x as usize];

                // タイル内のオブジェクトリスト
                let tile_objects = tile
                    .object_structs
                    .iter()
                    .map(|os| TileObjectJson {
                        object_id: os.object_id,
                        relative_coordinate: CoordinateJson {
                            x: os.relative_x,
                            y: os.relative_y,
                        },
                        visible: os.visible,
                    })
                    .collect();

                // 勤務地リスト
                let work_places = tile
                    .work_places
                    .iter()
                    .map(|wp| WorkPlaceJson {
                        work_kind: wp.kind,
                        serial_number: wp.serial_number,
                    })
                    .collect();

                // 学校リスト
                let school = tile
                    .schools
                    .iter()
                    .map(|s| SchoolJson {
                        school_kind: s.kind,
                        serial_number: s.serial_number,
                    })
                    .collect();

                row_json.push(TileJson {
                    coordinate: CoordinateJson { x, y },
                    objects: tile_objects,
                    residents: tile.residents,
                    workers: WorkersJson {
                        commercial: tile.workers_commercial,
                        office: tile.workers_office,
                        industrial: tile.workers_industrial,
                        farm: tile.workers_farm,
                        public: tile.workers_public,
                    },
                    students: tile.students,
                    rate: tile.rate.clone(), // HashMap<String, i32> 0以外の値のみ保持
                    age: tile.age.clone(),
                    gender: tile.gender.clone(),
                    work_places,
                    school,
                    reservation: tile.reservation,
                    original_name: tile.original_name.clone(),
                });
            }
            map_json.push(row_json);
        }

        // 3. すべてを SaveDataJson に統合
        let save_data = SaveDataJson {
            version: self.version,
            addon_set: self.addon_set_name.clone(),
            city_name: self.city_name.clone(),
            mayor_name: self.mayor_name.clone(),
            total_population: self.population,
            change_weather: self.change_weather,
            temperature: self.temperature,
            dark_on_night: self.dark_on_night,
            map_size: MapSizeJson {
                width: self.map_size[0],
                height: self.map_size[1],
            },
            time: TimeJson {
                year: self.time.year,
                month: self.time.month,
                date: self.time.date,
                hour: self.time.hour,
                minutes: self.time.minutes,
            },
            demand: DemandJson {
                residential: self.demand.residential,
                commercial: self.demand.commercial,
                office: self.demand.office,
                industrial: self.demand.industrial,
                farm: self.demand.farm,
            },
            money: self.money,
            budget: BudgetJson {
                police: self.budget_police,
                fire_depertment: self.budget_fire,
                post_office: self.budget_post,
                education: self.budget_education,
            },
            tax: TaxJson {
                residential: self.tax_residential,
                commercial: self.tax_commercial,
                office: self.tax_office,
                industrial: self.tax_industrial,
                farm: self.tax_farm,
            },
            objects: objects_json,
            map: map_json,
        };

        // 4. JSON文字列へ変換 (Pretty Print)
        serde_json::to_string_pretty(&save_data).unwrap_or_else(|_| "{}".to_string())
    }

    pub fn save_to_file(&self, path: String) -> bool {
        let json_str = self.generate_save_json();
        if json_str.is_empty() || json_str == "{}" {
            return false;
        }
        let encrypted = self.apply_xor_encryption(json_str, "citiesboxmapdatafilexor");
        match atomic_write_with_backup(Path::new(&path), &encrypted) {
            Ok(()) => true,
            Err(error) => {
                eprintln!("Failed to save map to {path}: {error}");
                false
            }
        }
    }

    fn apply_xor_encryption(&self, input: String, key: &str) -> Vec<u8> {
        let key_bytes = key.as_bytes();
        input
            .as_bytes()
            .iter()
            .enumerate()
            .map(|(i, &b)| b ^ key_bytes[i % key_bytes.len()])
            .collect()
    }
}

impl ffi::TimeStruct {
    // CityMap_GetData.cpp の cityTime ロジックを移植
    pub fn city_time(&mut self, minutes_delta: i32) -> ffi::TimeStruct {
        self.minutes += minutes_delta;

        if self.minutes >= 60 {
            self.hour += 1;
            self.minutes -= 60;

            if self.hour >= 24 {
                self.date += 1;
                self.hour -= 24;

                let month = self.month;
                let is_leap_year = self.year % 4 == 0;

                let days_in_month = match month {
                    4 | 6 | 9 | 11 => 30,
                    2 => {
                        if is_leap_year {
                            29
                        } else {
                            28
                        }
                    }
                    _ => 31,
                };

                if self.date > days_in_month {
                    self.month += 1;
                    self.date = 1;

                    if self.month > 12 {
                        self.year += 1;
                        self.month = 1;
                    }
                }
            }
        }
        ffi::TimeStruct {
            year: self.year,
            month: self.month,
            date: self.date,
            hour: self.hour,
            minutes: self.minutes,
        }
    }
}

// セーブデータ用の構造体（serde用）
#[derive(Serialize, Deserialize, Debug)]
pub struct SaveDataJson {
    #[serde(rename = "Version")]
    pub version: i32,
    #[serde(rename = "Addon_Set")]
    pub addon_set: String,
    #[serde(rename = "City_Name")]
    pub city_name: String,
    #[serde(rename = "Mayor_Name")]
    pub mayor_name: String,
    #[serde(rename = "Total_Population")]
    pub total_population: i32,
    #[serde(rename = "Change_Weather")]
    pub change_weather: bool,
    #[serde(rename = "Temperature")]
    pub temperature: i32,
    #[serde(rename = "Dark_on_Night")]
    pub dark_on_night: bool,
    #[serde(rename = "Map_size")]
    pub map_size: MapSizeJson,
    #[serde(rename = "Time")]
    pub time: TimeJson,
    #[serde(rename = "Demand")]
    pub demand: DemandJson,
    #[serde(rename = "Money")]
    pub money: i32,
    #[serde(rename = "Budget")]
    pub budget: BudgetJson,
    #[serde(rename = "Tax")]
    pub tax: TaxJson,
    #[serde(rename = "Objects")]
    pub objects: Vec<ObjectEntryJson>,
    #[serde(rename = "Map")]
    pub map: Vec<Vec<TileJson>>,
}

// --- 基本プロパティ用 ---
#[derive(Serialize, Deserialize, Debug)]
pub struct MapSizeJson {
    pub width: i32,
    pub height: i32,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct TimeJson {
    pub year: i32,
    pub month: i32,
    pub date: i32,
    pub hour: i32,
    pub minutes: i32,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct DemandJson {
    pub residential: f64,
    pub commercial: f64,
    pub office: f64,
    pub industrial: f64,
    pub farm: f64,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct BudgetJson {
    pub police: i32,
    pub fire_depertment: i32, // C++側のスペルミス（depertment）に合わせる
    pub post_office: i32,
    pub education: i32,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct TaxJson {
    pub residential: f64,
    pub commercial: f64,
    pub office: f64,
    pub industrial: f64,
    pub farm: f64,
}

// --- オブジェクト管理用 (m_objects) ---
#[derive(Serialize, Deserialize, Debug)]
pub struct ObjectEntryJson {
    #[serde(rename = "objectID")]
    pub object_id: i32,
    pub addon_name: String,
    pub original_name: String,
    #[serde(rename = "typeID")]
    pub type_id: String,
    #[serde(rename = "directionID")]
    pub direction_id: String,
    pub origin_coordinate: CoordinateJson,
    pub visible: bool,
}

// --- タイルデータ用 (m_tiles[y][x]) ---
#[derive(Serialize, Deserialize, Debug)]
pub struct TileJson {
    pub coordinate: CoordinateJson,
    pub objects: Vec<TileObjectJson>,
    pub residents: i32,
    pub workers: WorkersJson,
    pub students: i32,
    pub rate: HashMap<String, i32>, // 存在する値のみ保存
    pub age: Vec<i32>,
    pub gender: Vec<String>,
    pub work_places: Vec<WorkPlaceJson>,
    pub school: Vec<SchoolJson>,
    pub reservation: i32,
    pub original_name: String,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct TileObjectJson {
    #[serde(rename = "objectID")]
    pub object_id: i32,
    pub relative_coordinate: CoordinateJson,
    pub visible: bool,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct CoordinateJson {
    pub x: i32,
    pub y: i32,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct WorkersJson {
    pub commercial: i32,
    pub office: i32,
    pub industrial: i32,
    pub farm: i32,
    pub public: i32,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct WorkPlaceJson {
    pub work_kind: i32,
    pub serial_number: i32,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct SchoolJson {
    pub school_kind: i32,
    pub serial_number: i32,
}

// タイルデータ
#[derive(Clone)]
pub struct RustTile {
    pub residents: i32,
    pub workers_commercial: i32,
    pub workers_office: i32,
    pub workers_industrial: i32,
    pub workers_farm: i32,
    pub workers_public: i32,
    pub students: i32,
    pub reservation: i32,
    pub original_name: String,

    // 存在するレート名とその値 (UnitaryTools::rateIDToRateName 相当)
    pub rate: HashMap<String, i32>,

    // 配列データ
    pub age: Vec<i32>,
    pub gender: Vec<String>,
    pub work_places: Vec<RustWorkPlace>,
    pub schools: Vec<RustSchool>,

    // タイル上のオブジェクト情報 (ObjectStruct相当)
    pub object_structs: Vec<RustTileObjectReference>,
}

impl RustTile {
    fn default(x: i32, y: i32) -> Self {
        Self {
            residents: 0,
            workers_commercial: 0,
            workers_office: 0,
            workers_industrial: 0,
            workers_farm: 0,
            workers_public: 0,
            students: 0,
            reservation: 0,
            original_name: String::new(),
            rate: HashMap::new(),
            age: Vec::new(),
            gender: Vec::new(),
            work_places: Vec::new(),
            schools: Vec::new(),
            object_structs: Vec::new(),
        }
    }
}

pub struct RustObject {
    pub id: i32,
    pub addon_name_en: String,
    pub original_name: String,
    pub type_id: String,
    pub direction_id: String,
    pub origin_coordinate: CoordinateJson,
    pub visible: bool,
}

impl RustObject {
    // アドオンの英語名を取得
    pub fn get_addon_name_en(&self) -> String {
        self.addon_name_en.clone()
    }

    // オリジナル名を取得
    pub fn get_original_name(&self) -> String {
        self.original_name.clone()
    }
}

#[derive(Clone)]
pub struct RustTileObjectReference {
    pub object_id: i32,
    pub relative_x: i32,
    pub relative_y: i32,
    pub visible: bool,
}

#[derive(Clone)]
pub struct RustWorkPlace {
    pub kind: i32,
    pub serial_number: i32,
}

#[derive(Clone)]
pub struct RustSchool {
    pub kind: i32,
    pub serial_number: i32,
}

// 新規マップ作成
fn new_city_map() -> Box<RustCityMap> {
    Box::new(RustCityMap {
        // --- 基本メタデータ ---
        version: 142,
        addon_set_name: String::new(),
        city_name: String::from("New City"),
        mayor_name: String::from("Mayor"),
        population: 0,
        temperature: 20,
        money: 100000, // 初期資金

        // --- 設定フラグ ---
        change_weather: true,
        dark_on_night: true,

        // --- マップ形状 (初期値は 0, 0 または最小サイズ) ---
        map_size: [0, 0],

        // --- 時間管理 (TimeStruct) ---
        time: ffi::TimeStruct {
            year: 2024,
            month: 1,
            date: 1,
            hour: 0,
            minutes: 0,
        },

        // --- 需要管理 (RCOIFstruct) ---
        demand: ffi::RCOIFstruct {
            residential: 0.0,
            commercial: 0.0,
            office: 0.0,
            industrial: 0.0,
            farm: 0.0,
        },

        // --- 予算管理 (デフォルト値) ---
        budget_police: 100,
        budget_fire: 100,
        budget_post: 100,
        budget_education: 100,

        // --- 税率管理 (デフォルト 7.0% など) ---
        tax_residential: 7.0,
        tax_commercial: 7.0,
        tax_office: 7.0,
        tax_industrial: 7.0,
        tax_farm: 7.0,

        // --- オブジェクト・タイル管理 (最初は空) ---
        objects: HashMap::new(),
        tiles: Vec::new(),
        last_load_error: String::new(),
    })
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::time::{SystemTime, UNIX_EPOCH};

    #[test]
    fn save_json_uses_the_cpp_loader_compatible_schema() {
        let mut city = new_city_map();
        city.set_save_version(142);
        city.init_map_size(1, 1);
        city.add_object(
            7,
            "road".to_string(),
            "".to_string(),
            "IntersectionT".to_string(),
            "NorthSouthEast".to_string(),
            0,
            0,
            false,
        );
        city.set_tile_stats(
            0,
            0,
            vec![12, 34],
            vec!["male".to_string(), "female".to_string()],
        );
        city.add_tile_work_place(0, 0, 2, 101);
        city.add_tile_school(0, 0, 1, 202);

        let saved: SaveDataJson = serde_json::from_str(&city.generate_save_json()).unwrap();

        assert_eq!(saved.version, 142);
        assert_eq!(saved.objects[0].type_id, "IntersectionT");
        assert_eq!(saved.objects[0].direction_id, "NorthSouthEast");
        assert!(!saved.objects[0].visible);
        assert_eq!(saved.map[0][0].gender, ["male", "female"]);
        assert_eq!(saved.map[0][0].work_places.len(), 1);
        assert_eq!(saved.map[0][0].school.len(), 1);
    }

    #[test]
    fn atomic_save_keeps_the_previous_file_as_backup() {
        let unique = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_nanos();
        let path = std::env::temp_dir().join(format!(
            "citiesbox-save-test-{}-{unique}.cbj",
            std::process::id()
        ));
        let backup = sidecar_path(&path, ".bak");
        let temp = sidecar_path(&path, &format!(".tmp.{}", std::process::id()));
        let path_string = path.to_string_lossy().into_owned();

        let mut city = new_city_map();
        city.set_city_metadata(
            "First".to_string(),
            "Mayor".to_string(),
            "Normal".to_string(),
        );
        assert!(city.save_to_file(path_string.clone()));
        let first_save = fs::read(&path).unwrap();

        city.set_city_metadata(
            "Second".to_string(),
            "Mayor".to_string(),
            "Normal".to_string(),
        );
        assert!(city.save_to_file(path_string));

        assert_eq!(fs::read(&backup).unwrap(), first_save);
        assert_ne!(fs::read(&path).unwrap(), first_save);

        let second_save = fs::read(&path).unwrap();
        city.set_city_metadata(
            "Third".to_string(),
            "Mayor".to_string(),
            "Normal".to_string(),
        );
        assert!(city.save_to_file(path.to_string_lossy().into_owned()));
        assert_eq!(fs::read(&backup).unwrap(), second_save);
        assert_ne!(fs::read(&path).unwrap(), second_save);
        assert!(!temp.exists());

        let _ = fs::remove_file(path);
        let _ = fs::remove_file(backup);
    }

    fn windows_map_path(file_name: &str) -> PathBuf {
        Path::new(env!("CARGO_MANIFEST_DIR"))
            .join("../../Windows/Cities-Box.cpp/Cities-Box.cpp/App/data/maps")
            .join(file_name)
    }

    #[test]
    fn loads_current_map_into_rust_state() {
        let mut city = new_city_map();
        let path = windows_map_path("Sample_City.cbd.cbj");
        assert!(
            city.load_from_file(path.to_string_lossy().into_owned()),
            "{}",
            city.get_last_load_error()
        );
        assert_eq!(city.version, 142);
        assert_eq!(city.map_size, [100, 100]);
        assert_eq!(city.tiles.len(), 100);
        assert!(!city.objects.is_empty());
    }

    #[test]
    fn loads_original_r142_map_into_rust_state() {
        let mut city = new_city_map();
        let path = windows_map_path("backup.cbj");
        assert!(
            city.load_from_file(path.to_string_lossy().into_owned()),
            "{}",
            city.get_last_load_error()
        );
        assert_eq!(city.version, 142);
        assert_eq!(city.map_size, [100, 100]);
        assert_eq!(city.tiles.len(), 100);
        assert!(!city.objects.is_empty());
    }

    #[test]
    fn converts_legacy_map_to_current_rust_state() {
        let mut city = new_city_map();
        let path = windows_map_path("Sample_City.cbd.cbj.cbj");
        assert!(
            city.load_from_file(path.to_string_lossy().into_owned()),
            "{}",
            city.get_last_load_error()
        );
        assert_eq!(city.version, 142);
        assert_eq!(city.map_size, [100, 100]);
        assert_eq!(city.tiles.len(), 100);
        assert!(!city.objects.is_empty());
    }
}
