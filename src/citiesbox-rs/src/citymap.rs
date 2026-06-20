use serde::{Deserialize, Serialize};
use serde_json::{json, Value};
use std::collections::{HashMap, HashSet};
use std::fs::{self, OpenOptions};
use std::io::{self, Write};
use std::path::{Path, PathBuf};

mod load;
mod models;
mod save;
mod simulation;
mod state;

use models::*;
#[cfg(test)]
use save::sidecar_path;

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

    struct LoadedCityData {
        version: i32,
        addon_set_name: String,
        city_name: String,
        mayor_name: String,
        total_population: i32,
        change_weather: bool,
        temperature: i32,
        dark_on_night: bool,
        map_width: i32,
        map_height: i32,
        time: TimeStruct,
        demand: RCOIFstruct,
        money: i32,
        budget_police: i32,
        budget_fire: i32,
        budget_post: i32,
        budget_education: i32,
        tax_residential: f64,
        tax_commercial: f64,
        tax_office: f64,
        tax_industrial: f64,
        tax_farm: f64,
        objects: Vec<LoadedObjectData>,
        tiles: Vec<LoadedTileData>,
    }

    struct LoadCityResult {
        success: bool,
        error_message: String,
        city: LoadedCityData,
    }

    extern "Rust" {
        type RustCityMap;

        fn new_city_map() -> Box<RustCityMap>;

        // セーブデータを一括で読み込み、C++側の構築成功後にRust状態へ反映する
        fn load_city_map(&mut self, path: String) -> LoadCityResult;
        fn commit_loaded_city_map(&mut self) -> bool;

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

    pending_load: Option<SaveDataJson>,
}

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
        pending_load: None,
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

    fn assert_batch_load(file_name: &str) {
        let mut city = new_city_map();
        let path = windows_map_path(file_name);
        let result = city.load_city_map(path.to_string_lossy().into_owned());
        assert!(result.success, "{}", result.error_message);
        assert_eq!(result.city.version, 142);
        assert_eq!((result.city.map_width, result.city.map_height), (100, 100));
        assert_eq!(result.city.tiles.len(), 10_000);
        assert!(!result.city.objects.is_empty());

        assert!(city.commit_loaded_city_map());
        assert_eq!(city.version, 142);
        assert_eq!(city.map_size, [100, 100]);
        assert_eq!(city.tiles.len(), 100);
        assert!(!city.objects.is_empty());
        assert!(!city.commit_loaded_city_map());
    }

    #[test]
    fn loads_current_map_into_rust_state() {
        assert_batch_load("Sample_City.cbd.cbj");
    }

    #[test]
    fn loads_original_r142_map_into_rust_state() {
        assert_batch_load("backup.cbj");
    }

    #[test]
    fn converts_legacy_map_to_current_rust_state() {
        assert_batch_load("Sample_City.cbd.cbj.cbj");
    }

    #[test]
    fn failed_batch_load_returns_only_an_error() {
        let mut city = new_city_map();
        let result = city.load_city_map(
            windows_map_path("missing.cbj")
                .to_string_lossy()
                .into_owned(),
        );
        assert!(!result.success);
        assert!(!result.error_message.is_empty());
        assert!(result.city.objects.is_empty());
        assert!(result.city.tiles.is_empty());
        assert!(!city.commit_loaded_city_map());
    }
}
