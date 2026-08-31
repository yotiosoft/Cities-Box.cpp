use serde::{Deserialize, Serialize};
use serde_json::{Value, json};
use std::collections::{HashMap, HashSet};
use std::fs::{self, OpenOptions};
use std::io::{self, Write};
use std::path::{Path, PathBuf};

mod connectable;
mod load;
mod models;
mod network;
mod rates;
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

    #[derive(Debug)]
    enum ConnectableConnectionStatus {
        Apply = 0,
        AlreadyConnected = 1,
        InvalidDirection = 2,
    }

    struct ConnectableConnectionRequest {
        from_x: i32,
        from_y: i32,
        to_x: i32,
        to_y: i32,
        current_direction: i32,
        current_type: i32,
        allow_diagonal: bool,
        from_here: bool,
        connection_slot_occupied: bool,
        force_type: bool,
        forced_type: i32,
    }

    struct ConnectableConnectionDecision {
        status: ConnectableConnectionStatus,
        relative_direction: i32,
        updated_direction: i32,
        updated_type: i32,
    }

    struct ConnectableRemovalDecision {
        updated_direction: i32,
        updated_type: i32,
        isolated: bool,
    }

    struct ConnectableNetworkNode {
        object_id: i32,
        x: i32,
        y: i32,
        connectable_kind: i32,
        under_construction: bool,
    }

    struct ConnectableNetworkEdge {
        from_object_id: i32,
        to_object_id: i32,
        from_direction: i32,
        to_direction: i32,
    }

    struct ConnectableNetworkAnalysis {
        component_count: i32,
        component_ids: Vec<i32>,
        isolated_object_ids: Vec<i32>,
        unfinished_isolated_object_ids: Vec<i32>,
        duplicate_object_ids: Vec<i32>,
        invalid_edge_indices: Vec<i32>,
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

    struct BudgetSettings {
        police: i32,
        fire: i32,
        post: i32,
        education: i32,
    }

    struct TaxSettings {
        residential: f64,
        commercial: f64,
        office: f64,
        industrial: f64,
        farm: f64,
    }

    struct ResidentialTileState {
        x: i32,
        y: i32,
        residents: i32,
        maximum_capacity: i32,
        ages: Vec<i32>,
        genders: Vec<String>,
        work_place_kinds: Vec<i32>,
        work_place_serial_numbers: Vec<i32>,
        school_kinds: Vec<i32>,
        school_serial_numbers: Vec<i32>,
    }

    struct WorkPlaceTileState {
        x: i32,
        y: i32,
        kind: i32,
        serial_number: i32,
        maximum_capacity: i32,
        workers: i32,
    }

    struct SchoolTileState {
        x: i32,
        y: i32,
        kind: i32,
        serial_number: i32,
        maximum_capacity: i32,
        students: i32,
    }

    struct DemandTileState {
        land_price: i32,
        crime_rate: i32,
        education_rate: i32,
        noise_rate: i32,
    }

    #[derive(Clone)]
    struct TileRateState {
        x: i32,
        y: i32,
        land_price: i32,
        crime_rate: i32,
        education_rate: i32,
        noise_rate: i32,
        happiness_rate: i32,
    }

    struct RateEffect {
        rate_kind: i32,
        influence: i32,
        range: i32,
        origin_x: i32,
        origin_y: i32,
        will_be_deleted: bool,
    }

    struct SpecialDemandState {
        residential: i32,
        commercial: i32,
        office: i32,
        industrial: i32,
        farm: i32,
    }

    struct SimulationObjectState {
        object_id: i32,
        category_ids: Vec<i32>,
        monthly_maintenance_police: i64,
        monthly_maintenance_fire: i64,
        monthly_maintenance_post: i64,
        monthly_maintenance_education: i64,
        has_monthly_maintenance_police: bool,
        has_monthly_maintenance_fire: bool,
        has_monthly_maintenance_post: bool,
        has_monthly_maintenance_education: bool,
    }

    struct SimulationUpdate {
        snapshot: SimulationSnapshot,
        residential_tiles: Vec<ResidentialTileState>,
        work_place_tiles: Vec<WorkPlaceTileState>,
        school_tiles: Vec<SchoolTileState>,
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

        // 生ポインタやOpenSiv3D型を含まない接続状態の純粋計算
        fn plan_connectable_connection(
            request: ConnectableConnectionRequest,
        ) -> ConnectableConnectionDecision;
        fn plan_connectable_removal(
            current_direction: i32,
            removed_direction: i32,
        ) -> ConnectableRemovalDecision;
        fn connectable_categories_can_connect(left: &[i32], right: &[i32]) -> bool;
        fn connectable_categories_match(left: &[i32], right: &[i32], hint: i32) -> bool;
        fn connectable_crossing_type(first: i32, second: i32) -> i32;
        fn analyze_connectable_network(
            nodes: Vec<ConnectableNetworkNode>,
            edges: Vec<ConnectableNetworkEdge>,
        ) -> ConnectableNetworkAnalysis;

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
        fn update_rates(
            &mut self,
            width: i32,
            height: i32,
            tiles: Vec<TileRateState>,
            effects: Vec<RateEffect>,
        ) -> Vec<TileRateState>;
        fn add_tile_work_place(&mut self, x: i32, y: i32, kind: i32, serial_number: i32);
        fn add_tile_school(&mut self, x: i32, y: i32, kind: i32, serial_number: i32);

        // シミュレーション状態の取得と更新
        fn simulation_snapshot(&self) -> SimulationSnapshot;
        fn set_finance_settings(
            &mut self,
            budget: BudgetSettings,
            tax: TaxSettings,
        ) -> SimulationSnapshot;
        fn charge_construction_cost(&mut self);
        fn will_run_daily_update(&self, minutes_delta: i32) -> bool;
        #[allow(clippy::too_many_arguments)]
        fn update_world(
            &mut self,
            minutes_delta: i32,
            residential_tiles: Vec<ResidentialTileState>,
            work_place_tiles: Vec<WorkPlaceTileState>,
            school_tiles: Vec<SchoolTileState>,
            demand_tiles: Vec<DemandTileState>,
            special_demand: SpecialDemandState,
            simulation_objects: Vec<SimulationObjectState>,
        ) -> SimulationUpdate;

        // 基本情報の同期用
        fn set_save_version(&mut self, version: i32);

        // スライス（配列の参照）として一括で受け取る
        fn bulk_set_tiles(&mut self, data: &[RawTileData], width: i32, height: i32);
        // 保存まで実行
        fn save_to_file(&self, path: String) -> bool;
    }
}

fn analyze_connectable_network(
    nodes: Vec<ffi::ConnectableNetworkNode>,
    edges: Vec<ffi::ConnectableNetworkEdge>,
) -> ffi::ConnectableNetworkAnalysis {
    network::analyze(nodes, edges)
}

fn plan_connectable_connection(
    request: ffi::ConnectableConnectionRequest,
) -> ffi::ConnectableConnectionDecision {
    let decision = connectable::plan_connection(connectable::ConnectionRequest {
        from: connectable::Coordinate {
            x: request.from_x,
            y: request.from_y,
        },
        to: connectable::Coordinate {
            x: request.to_x,
            y: request.to_y,
        },
        current_direction: request.current_direction,
        current_type: request.current_type,
        allow_diagonal: request.allow_diagonal,
        from_here: request.from_here,
        connection_slot_occupied: request.connection_slot_occupied,
        forced_type: request.force_type.then_some(request.forced_type),
    });
    ffi::ConnectableConnectionDecision {
        status: match decision.status {
            connectable::ConnectionStatus::Apply => ffi::ConnectableConnectionStatus::Apply,
            connectable::ConnectionStatus::AlreadyConnected => {
                ffi::ConnectableConnectionStatus::AlreadyConnected
            }
            connectable::ConnectionStatus::InvalidDirection => {
                ffi::ConnectableConnectionStatus::InvalidDirection
            }
        },
        relative_direction: decision.relative_direction,
        updated_direction: decision.updated_direction,
        updated_type: decision.updated_type,
    }
}

fn plan_connectable_removal(
    current_direction: i32,
    removed_direction: i32,
) -> ffi::ConnectableRemovalDecision {
    let decision = connectable::plan_removal(current_direction, removed_direction);
    ffi::ConnectableRemovalDecision {
        updated_direction: decision.updated_direction,
        updated_type: decision.updated_type,
        isolated: decision.isolated,
    }
}

fn connectable_categories_can_connect(left: &[i32], right: &[i32]) -> bool {
    connectable::categories_can_connect(left, right)
}

fn connectable_categories_match(left: &[i32], right: &[i32], hint: i32) -> bool {
    connectable::categories_match(left, right, hint)
}

fn connectable_crossing_type(first: i32, second: i32) -> i32 {
    connectable::crossing_type(first, second)
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
    fn connectable_bridge_uses_value_only_decisions() {
        let connection = plan_connectable_connection(ffi::ConnectableConnectionRequest {
            from_x: 3,
            from_y: 4,
            to_x: 4,
            to_y: 4,
            current_direction: connectable::direction_id::NORTH,
            current_type: connectable::type_id::DEAD_END,
            allow_diagonal: false,
            from_here: false,
            connection_slot_occupied: false,
            force_type: false,
            forced_type: connectable::type_id::DISABLED,
        });
        assert_eq!(connection.status, ffi::ConnectableConnectionStatus::Apply);
        assert_eq!(
            connection.relative_direction,
            connectable::direction_id::EAST
        );
        assert_eq!(
            connection.updated_direction,
            connectable::direction_id::NORTH_EAST
        );
        assert_eq!(connection.updated_type, connectable::type_id::TURN);

        let removal = plan_connectable_removal(
            connectable::direction_id::NORTH_SOUTH,
            connectable::direction_id::SOUTH,
        );
        assert_eq!(removal.updated_direction, connectable::direction_id::NORTH);
        assert_eq!(removal.updated_type, connectable::type_id::DEAD_END);
        assert!(!removal.isolated);

        let road = [
            connectable::category_id::CONNECTABLE,
            connectable::category_id::ROAD,
        ];
        let other_road = [
            connectable::category_id::CONNECTABLE,
            connectable::category_id::ROAD,
        ];
        assert!(connectable_categories_can_connect(&road, &other_road));
        assert!(connectable_categories_match(
            &road,
            &other_road,
            connectable::category_id::CONNECTABLE
        ));
        assert_eq!(
            connectable_crossing_type(
                connectable::category_id::ROAD,
                connectable::category_id::RAILROAD
            ),
            connectable::type_id::TRAIN_CROSSING
        );
    }

    #[test]
    fn connectable_network_bridge_uses_value_only_snapshots() {
        let analysis = analyze_connectable_network(
            vec![
                ffi::ConnectableNetworkNode {
                    object_id: 11,
                    x: 2,
                    y: 3,
                    connectable_kind: connectable::category_id::ROAD,
                    under_construction: false,
                },
                ffi::ConnectableNetworkNode {
                    object_id: 12,
                    x: 3,
                    y: 3,
                    connectable_kind: connectable::category_id::ROAD,
                    under_construction: false,
                },
            ],
            vec![ffi::ConnectableNetworkEdge {
                from_object_id: 11,
                to_object_id: 12,
                from_direction: connectable::direction_id::EAST,
                to_direction: connectable::direction_id::WEST,
            }],
        );

        assert_eq!(analysis.component_count, 1);
        assert_eq!(analysis.component_ids, vec![0, 0]);
        assert!(analysis.isolated_object_ids.is_empty());
        assert!(analysis.invalid_edge_indices.is_empty());
    }

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
    fn connectable_shape_and_tile_reference_survive_save_and_reload() {
        let unique = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_nanos();
        let path = windows_map_path(&format!(
            "citiesbox-connectable-roundtrip-{}-{unique}.cbj",
            std::process::id()
        ));

        let mut source = new_city_map();
        source.set_save_version(142);
        source.addon_set_name = "Normal".to_string();
        source.map_size = [1, 1];
        source.tiles = vec![vec![RustTile::default()]];
        source.add_object(
            42,
            "Two_lane_normal_road(white_line)".to_string(),
            "".to_string(),
            "IntersectionT".to_string(),
            "NorthSouthEast".to_string(),
            0,
            0,
            true,
        );
        source.add_tile_object_ref(0, 0, 42, 0, 0, true);
        assert!(source.save_to_file(path.to_string_lossy().into_owned()));

        let mut loaded = new_city_map();
        let result = loaded.load_city_map(path.to_string_lossy().into_owned());
        assert!(result.success, "{}", result.error_message);
        assert_eq!(result.city.objects.len(), 1);
        assert_eq!(result.city.objects[0].id, 42);
        assert_eq!(result.city.objects[0].type_name, "IntersectionT");
        assert_eq!(result.city.objects[0].direction_name, "NorthSouthEast");
        assert_eq!(result.city.tiles[0].objects.len(), 1);
        assert_eq!(result.city.tiles[0].objects[0].object_id, 42);

        assert!(loaded.commit_loaded_city_map());
        let object = loaded.objects.get(&42).unwrap();
        assert_eq!(object.type_id, "IntersectionT");
        assert_eq!(object.direction_id, "NorthSouthEast");
        assert_eq!(loaded.tiles[0][0].object_structs[0].object_id, 42);

        let _ = std::fs::remove_file(path);
    }

    #[test]
    fn construction_cost_is_visible_in_the_cpp_snapshot() {
        let mut city = new_city_map();

        city.charge_construction_cost();

        assert_eq!(city.simulation_snapshot().money, 99_995);
    }

    fn budget_settings(police: i32, fire: i32, post: i32, education: i32) -> ffi::BudgetSettings {
        ffi::BudgetSettings {
            police,
            fire,
            post,
            education,
        }
    }

    fn tax_settings(
        residential: f64,
        commercial: f64,
        office: f64,
        industrial: f64,
        farm: f64,
    ) -> ffi::TaxSettings {
        ffi::TaxSettings {
            residential,
            commercial,
            office,
            industrial,
            farm,
        }
    }

    fn no_special_demand() -> ffi::SpecialDemandState {
        ffi::SpecialDemandState {
            residential: 0,
            commercial: 0,
            office: 0,
            industrial: 0,
            farm: 0,
        }
    }

    #[test]
    fn set_finance_settings_normalizes_and_returns_the_snapshot() {
        let mut city = new_city_map();

        let snapshot = city.set_finance_settings(
            budget_settings(-1, 201, -30, i32::MAX),
            tax_settings(-1.0, 200.0, f64::NAN, f64::INFINITY, f64::NEG_INFINITY),
        );

        assert_eq!(
            (
                snapshot.budget_police,
                snapshot.budget_fire,
                snapshot.budget_post,
                snapshot.budget_education
            ),
            (0, 200, 0, 200)
        );
        assert_eq!(
            (
                snapshot.tax_residential,
                snapshot.tax_commercial,
                snapshot.tax_office,
                snapshot.tax_industrial,
                snapshot.tax_farm
            ),
            (0.0, 100.0, 0.0, 0.0, 0.0)
        );
    }

    #[test]
    fn finance_settings_survive_update_world() {
        let mut city = new_city_map();
        city.set_finance_settings(
            budget_settings(11, 22, 33, 44),
            tax_settings(1.0, 2.0, 3.0, 4.0, 5.0),
        );

        let update = city.update_world(
            1,
            Vec::new(),
            Vec::new(),
            Vec::new(),
            Vec::new(),
            no_special_demand(),
            Vec::new(),
        );

        assert_eq!(
            (
                update.snapshot.budget_police,
                update.snapshot.budget_fire,
                update.snapshot.budget_post,
                update.snapshot.budget_education,
                update.snapshot.tax_residential,
                update.snapshot.tax_commercial,
                update.snapshot.tax_office,
                update.snapshot.tax_industrial,
                update.snapshot.tax_farm,
            ),
            (11, 22, 33, 44, 1.0, 2.0, 3.0, 4.0, 5.0)
        );
    }

    #[test]
    fn finance_settings_survive_save_and_reload() {
        let unique = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_nanos();
        let path = windows_map_path(&format!(
            "citiesbox-finance-roundtrip-{}-{unique}.cbj",
            std::process::id()
        ));
        let backup = sidecar_path(&path, ".bak");
        let mut city = new_city_map();
        city.addon_set_name = "Normal".to_string();
        city.map_size = [1, 1];
        city.tiles = vec![vec![RustTile::default()]];
        city.set_finance_settings(
            budget_settings(11, 22, 33, 44),
            tax_settings(1.0, 2.0, 3.0, 4.0, 5.0),
        );
        assert!(city.save_to_file(path.to_string_lossy().into_owned()));

        let mut reloaded = new_city_map();
        let load = reloaded.load_city_map(path.to_string_lossy().into_owned());
        assert!(load.success, "{}", load.error_message);
        assert!(reloaded.commit_loaded_city_map());
        let snapshot = reloaded.simulation_snapshot();

        assert_eq!(
            (
                snapshot.budget_police,
                snapshot.budget_fire,
                snapshot.budget_post,
                snapshot.budget_education,
                snapshot.tax_residential,
                snapshot.tax_commercial,
                snapshot.tax_office,
                snapshot.tax_industrial,
                snapshot.tax_farm,
            ),
            (11, 22, 33, 44, 1.0, 2.0, 3.0, 4.0, 5.0)
        );

        let _ = fs::remove_file(path);
        let _ = fs::remove_file(backup);
    }

    #[test]
    fn monthly_finances_use_the_updated_settings() {
        let mut city = new_city_map();
        city.simulation.time = ffi::TimeStruct {
            year: 2024,
            month: 1,
            date: 31,
            hour: 23,
            minutes: 59,
        };
        city.simulation.money = 100_000;
        city.set_finance_settings(
            budget_settings(10, 20, 30, 40),
            tax_settings(1.0, 2.0, 3.0, 4.0, 5.0),
        );
        let simulation_objects = [19, 20, 21, 22, 23, 26, 25, 27, 28]
            .into_iter()
            .enumerate()
            .map(|(index, category_id)| ffi::SimulationObjectState {
                object_id: index as i32,
                category_ids: vec![category_id],
                monthly_maintenance_police: 1_000,
                monthly_maintenance_fire: 1_000,
                monthly_maintenance_post: 1_000,
                monthly_maintenance_education: 1_000,
                has_monthly_maintenance_police: true,
                has_monthly_maintenance_fire: true,
                has_monthly_maintenance_post: true,
                has_monthly_maintenance_education: true,
            })
            .collect();

        let update = city.update_world(
            1,
            Vec::new(),
            Vec::new(),
            Vec::new(),
            Vec::new(),
            no_special_demand(),
            simulation_objects,
        );

        assert_eq!(update.snapshot.money, 99_000);
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
        let update = updated.update_world(
            1,
            Vec::new(),
            Vec::new(),
            Vec::new(),
            Vec::new(),
            no_special_demand(),
            Vec::new(),
        );
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
