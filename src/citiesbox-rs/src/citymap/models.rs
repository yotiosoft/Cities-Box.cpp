use super::*;

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
#[derive(Clone, Default)]
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
