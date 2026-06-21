use super::*;

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

fn empty_loaded_city_data() -> ffi::LoadedCityData {
    ffi::LoadedCityData {
        addon_set_name: String::new(),
        map_width: 0,
        map_height: 0,
        objects: Vec::new(),
        tiles: Vec::new(),
    }
}

fn loaded_city_data(data: &SaveDataJson) -> ffi::LoadedCityData {
    let mut objects: Vec<_> = data.objects.iter().collect();
    objects.sort_by_key(|object| object.object_id);

    ffi::LoadedCityData {
        addon_set_name: data.addon_set.clone(),
        map_width: data.map_size.width,
        map_height: data.map_size.height,
        objects: objects
            .into_iter()
            .map(|object| ffi::LoadedObjectData {
                id: object.object_id,
                addon_name: object.addon_name.clone(),
                original_name: object.original_name.clone(),
                type_name: object.type_id.clone(),
                direction_name: object.direction_id.clone(),
                origin_x: object.origin_coordinate.x,
                origin_y: object.origin_coordinate.y,
                visible: object.visible,
            })
            .collect(),
        tiles: data
            .map
            .iter()
            .flat_map(|row| row.iter())
            .map(|tile| {
                let mut rates: Vec<_> = tile.rate.iter().collect();
                rates.sort_by(|left, right| left.0.cmp(right.0));
                ffi::LoadedTileData {
                    residents: tile.residents,
                    workers_commercial: tile.workers.commercial,
                    workers_office: tile.workers.office,
                    workers_industrial: tile.workers.industrial,
                    workers_farm: tile.workers.farm,
                    workers_public: tile.workers.public,
                    students: tile.students,
                    reservation: tile.reservation,
                    original_name: tile.original_name.clone(),
                    ages: tile.age.clone(),
                    genders: tile.gender.clone(),
                    objects: tile
                        .objects
                        .iter()
                        .map(|object| ffi::LoadedTileObjectData {
                            object_id: object.object_id,
                            relative_x: object.relative_coordinate.x,
                            relative_y: object.relative_coordinate.y,
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
                            kind: workplace.work_kind,
                            serial_number: workplace.serial_number,
                        })
                        .collect(),
                    schools: tile
                        .school
                        .iter()
                        .map(|school| ffi::LoadedSchoolData {
                            kind: school.school_kind,
                            serial_number: school.serial_number,
                        })
                        .collect(),
                }
            })
            .collect(),
    }
}

impl RustCityMap {
    pub(super) fn load_city_map(&mut self, path: String) -> ffi::LoadCityResult {
        match read_and_normalize_map(Path::new(&path)) {
            Ok((data, _catalog)) => {
                let city = loaded_city_data(&data);
                self.pending_load = Some(data);
                ffi::LoadCityResult {
                    success: true,
                    error_message: String::new(),
                    city,
                }
            }
            Err(error) => {
                self.pending_load = None;
                ffi::LoadCityResult {
                    success: false,
                    error_message: error,
                    city: empty_loaded_city_data(),
                }
            }
        }
    }

    pub(super) fn commit_loaded_city_map(&mut self) -> bool {
        if let Some(data) = self.pending_load.take() {
            self.apply_loaded_data(data);
            true
        } else {
            false
        }
    }

    fn apply_loaded_data(&mut self, data: SaveDataJson) {
        self.version = data.version;
        self.addon_set_name = data.addon_set;
        self.city_name = data.city_name;
        self.mayor_name = data.mayor_name;
        self.simulation.population = data.total_population;
        self.change_weather = data.change_weather;
        self.simulation.temperature = data.temperature;
        self.dark_on_night = data.dark_on_night;
        self.map_size = [data.map_size.width, data.map_size.height];
        self.simulation.time = ffi::TimeStruct {
            year: data.time.year,
            month: data.time.month,
            date: data.time.date,
            hour: data.time.hour,
            minutes: data.time.minutes,
        };
        self.simulation.demand = ffi::RCOIFstruct {
            residential: data.demand.residential,
            commercial: data.demand.commercial,
            office: data.demand.office,
            industrial: data.demand.industrial,
            farm: data.demand.farm,
        };
        self.simulation.money = data.money;
        self.simulation.budget_police = data.budget.police;
        self.simulation.budget_fire = data.budget.fire_depertment;
        self.simulation.budget_post = data.budget.post_office;
        self.simulation.budget_education = data.budget.education;
        self.simulation.tax_residential = data.tax.residential;
        self.simulation.tax_commercial = data.tax.commercial;
        self.simulation.tax_office = data.tax.office;
        self.simulation.tax_industrial = data.tax.industrial;
        self.simulation.tax_farm = data.tax.farm;

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

    // スライス（配列の参照）として一括で受け取る
}
