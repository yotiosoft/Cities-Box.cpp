use serde::{Deserialize, Serialize};
use serde_json::{Value, json};
use std::collections::{HashMap, HashSet};
use std::fs::{self, OpenOptions};
use std::io::{self, Write};
use std::path::{Path, PathBuf};

mod load;
mod models;
mod save;
mod state;

use crate::simulation::SimulationState;
use models::*;
#[cfg(test)]
use save::sidecar_path;

#[cxx::bridge(namespace = "rust::citymap")]
pub(crate) mod ffi {
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

    #[derive(Clone)]
    struct SimulationSnapshot {
        time: TimeStruct,
        population: i32,
        money: i32,
        temperature: i32,
        demand: RCOIFstruct,
        budget_police: i32,
        budget_fire: i32,
        budget_post: i32,
        budget_education: i32,
        tax_residential: f64,
        tax_commercial: f64,
        tax_office: f64,
        tax_industrial: f64,
        tax_farm: f64,
    }

    struct ResidentialTileState {
        x: i32,
        y: i32,
        residents: i32,
        maximum_capacity: i32,
        ages: Vec<i32>,
        genders: Vec<String>,
    }

    struct SimulationMapStats {
        residential_tiles: i32,
        commercial_tiles: i32,
        office_tiles: i32,
        industrial_tiles: i32,
        farm_tiles: i32,
        police_stations: i32,
        fire_departments: i32,
        post_offices: i32,
        education_facilities: i32,
    }

    struct SimulationUpdate {
        snapshot: SimulationSnapshot,
        residential_tiles: Vec<ResidentialTileState>,
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
        addon_set_name: String,
        map_width: i32,
        map_height: i32,
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

        // シミュレーション状態の取得と更新
        fn simulation_snapshot(&self) -> SimulationSnapshot;
        fn will_run_daily_update(&self, minutes_delta: i32) -> bool;
        fn update_world(
            &mut self,
            minutes_delta: i32,
            residential_tiles: Vec<ResidentialTileState>,
            map_stats: SimulationMapStats,
        ) -> SimulationUpdate;

        // 基本情報の同期用
        fn set_save_version(&mut self, version: i32);

        // スライス（配列の参照）として一括で受け取る
        fn bulk_set_tiles(&mut self, data: &[RawTileData], width: i32, height: i32);
        // 保存まで実行
        fn save_to_file(&self, path: String) -> bool;
    }
}

pub struct RustCityMap {
    // OpenSiv3Dに依存しない都市状態。simulation.rs はこの値だけを更新し、
    // 保存用のタイル・オブジェクト写像を参照しない。
    pub(crate) simulation: SimulationState,

    // 都市の基本情報
    pub version: i32,
    pub addon_set_name: String,
    pub city_name: String,
    pub mayor_name: String,
    // 設定
    pub change_weather: bool,
    pub dark_on_night: bool,

    // マップ形状
    pub map_size: [i32; 2], // [width, height]

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
        simulation: SimulationState::new(),

        // --- 設定フラグ ---
        change_weather: true,
        dark_on_night: true,

        // --- マップ形状 (初期値は 0, 0 または最小サイズ) ---
        map_size: [0, 0],

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
        city.map_size = [1, 1];
        city.tiles = vec![vec![RustTile::default()]];
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
        city.city_name = "First".to_string();
        city.mayor_name = "Mayor".to_string();
        city.addon_set_name = "Normal".to_string();
        assert!(city.save_to_file(path_string.clone()));
        let first_save = fs::read(&path).unwrap();

        city.city_name = "Second".to_string();
        assert!(city.save_to_file(path_string));

        assert_eq!(fs::read(&backup).unwrap(), first_save);
        assert_ne!(fs::read(&path).unwrap(), first_save);

        let second_save = fs::read(&path).unwrap();
        city.city_name = "Third".to_string();
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

    #[test]
    fn loaded_simulation_state_survives_update_save_and_reload() {
        let unique = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_nanos();
        let first_path = windows_map_path(&format!(
            "citiesbox-state-roundtrip-{}-{unique}-first.cbj",
            std::process::id()
        ));
        let second_path = windows_map_path(&format!(
            "citiesbox-state-roundtrip-{}-{unique}-second.cbj",
            std::process::id()
        ));

        let mut source = new_city_map();
        let source_load = source.load_city_map(
            windows_map_path("Sample_City.cbd.cbj")
                .to_string_lossy()
                .into_owned(),
        );
        assert!(source_load.success, "{}", source_load.error_message);
        assert!(source.commit_loaded_city_map());
        source.simulation.time = ffi::TimeStruct {
            year: 2032,
            month: 2,
            date: 29,
            hour: 12,
            minutes: 34,
        };
        source.simulation.population = 12_345;
        source.simulation.money = 678_901;
        source.simulation.temperature = -7;
        source.simulation.demand = ffi::RCOIFstruct {
            residential: 11.0,
            commercial: 22.0,
            office: 33.0,
            industrial: 44.0,
            farm: 55.0,
        };
        source.simulation.budget_police = 61;
        source.simulation.budget_fire = 62;
        source.simulation.budget_post = 63;
        source.simulation.budget_education = 64;
        source.simulation.tax_residential = 1.1;
        source.simulation.tax_commercial = 2.2;
        source.simulation.tax_office = 3.3;
        source.simulation.tax_industrial = 4.4;
        source.simulation.tax_farm = 5.5;
        assert!(source.save_to_file(first_path.to_string_lossy().into_owned()));

        let mut updated = new_city_map();
        let load = updated.load_city_map(first_path.to_string_lossy().into_owned());
        assert!(load.success, "{}", load.error_message);
        assert!(updated.commit_loaded_city_map());
        let update = updated.update_world(1, Vec::new(), crate::simulation::empty_map_stats());
        assert!(updated.save_to_file(second_path.to_string_lossy().into_owned()));

        let mut reloaded = new_city_map();
        let reload = reloaded.load_city_map(second_path.to_string_lossy().into_owned());
        assert!(reload.success, "{}", reload.error_message);
        assert!(reloaded.commit_loaded_city_map());
        let saved: SaveDataJson = serde_json::from_str(&reloaded.generate_save_json()).unwrap();

        assert_eq!(
            (
                saved.time.year,
                saved.time.month,
                saved.time.date,
                saved.time.hour,
                saved.time.minutes
            ),
            (2032, 2, 29, 12, 35)
        );
        assert_eq!(saved.total_population, 12_345);
        assert_eq!(saved.money, 678_901);
        assert_eq!(saved.temperature, update.snapshot.temperature);
        assert_eq!(
            (
                saved.demand.residential,
                saved.demand.commercial,
                saved.demand.office,
                saved.demand.industrial,
                saved.demand.farm
            ),
            (11.0, 22.0, 33.0, 44.0, 55.0)
        );
        assert_eq!(
            (
                saved.budget.police,
                saved.budget.fire_depertment,
                saved.budget.post_office,
                saved.budget.education
            ),
            (61, 62, 63, 64)
        );
        assert_eq!(
            (
                saved.tax.residential,
                saved.tax.commercial,
                saved.tax.office,
                saved.tax.industrial,
                saved.tax.farm
            ),
            (1.1, 2.2, 3.3, 4.4, 5.5)
        );

        let _ = fs::remove_file(first_path);
        let _ = fs::remove_file(second_path);
    }
}
