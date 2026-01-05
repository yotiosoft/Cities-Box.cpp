use serde::{ Serialize, Deserialize };
use std::collections::HashMap;

#[cxx::bridge(namespace = "rust::citymap")]
mod ffi {
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

    extern "Rust" {
        type RustCityMap;

        fn new_city_map() -> Box<RustCityMap>;

        // 初期化
        fn set_status(&mut self, pop: i32, money: i32, temp: i32, time: TimeStruct, demand: RCOIFstruct);

        // ステータス取得
        fn get_population(&self) -> i32;
        fn get_money(&self) -> i32;
        fn get_temperature(&self) -> i32;
        fn get_demand(&self) -> RCOIFstruct;

        // 時間進行
        fn city_time(&mut self, minutes_delta: i32) -> TimeStruct;
        fn update_world(&mut self, minutes_delta: i32) -> TimeStruct;

        // 基本情報の同期用
        fn set_city_metadata(&mut self, city_name: String, mayor_name: String, addon_set: String);
        fn set_financial_data(&mut self, money: i32, population: i32);
        
        // 予算と税率 (一括で渡すと効率的)
        fn set_budget(&mut self, police: i32, fire: i32, post: i32, edu: i32);
        fn set_tax(&mut self, r: f64, c: f64, o: f64, i: f64, f: f64);

        // 時間と天候の設定
        fn set_environment(&mut self, year: i32, month: i32, date: i32, hour: i32, min: i32, weather: bool, night: bool);

        // セーブデータ生成（前回定義したもの）
        fn generate_save_json(&self) -> String;
    }
}

pub struct RustCityMap {
    // 需要度
    demand: ffi::RCOIFstruct,

    // 時刻
    time: ffi::TimeStruct,

    // 都市の基本情報
    pub version: String,
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
    pub objects: HashMap<i32, RustObject>,  // Key : ObjectID

    // タイルデータ
    pub tiles: Vec<Vec<RustTile>>,
}

impl RustCityMap {
    // 初期化
    fn set_status(&mut self, pop: i32, money: i32, temp: i32, time: ffi::TimeStruct, demand: ffi::RCOIFstruct) {
        self.demand = demand;
        self.population = pop;
        self.money = money;
        self.temperature = temp;
        self.time = time;
    }

    // ステータス取得
    fn get_population(&self) -> i32 { self.population }
    fn get_money(&self) -> i32 { self.money }
    fn get_temperature(&self) -> i32 { self.temperature }
    fn get_demand(&self) -> ffi::RCOIFstruct { self.demand.clone() }

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

    fn set_environment(&mut self, year: i32, month: i32, date: i32, hour: i32, min: i32, weather: bool, night: bool) {
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
        let objects_json: Vec<ObjectEntryJson> = self.objects
            .iter()
            .map(|(id, obj)| ObjectEntryJson {
                object_id: *id,
                addon_name: obj.get_addon_name_en(),
                original_name: obj.get_original_name(),
                type_id: obj.get_type_id_name(),      // UnitaryTools::typeIDToTypeName 相当
                direction_id: obj.get_direction_name(), // UnitaryTools::directionIDToDirectionName 相当
                origin_coordinate: CoordinateJson {
                    x: obj.origin_coordinate.x,
                    y: obj.origin_coordinate.y,
                },
            })
            .collect();

        // 2. マップタイルデータの集計 (m_tiles[y][x] 相当)
        let mut map_json = Vec::new();
        for y in 0..self.map_size[1] {
            let mut row_json = Vec::new();
            for x in 0..self.map_size[0] {
                let tile = &self.tiles[y as usize][x as usize];
                
                // タイル内のオブジェクトリスト
                let tile_objects = tile.object_structs
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
                let work_places = tile.work_places
                    .iter()
                    .map(|wp| WorkPlaceJson {
                        work_kind: wp.kind,
                        serial_number: wp.serial_number,
                    })
                    .collect();

                // 学校リスト
                let school = tile.schools
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
            version: self.version.clone(),
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
                    2 => if is_leap_year { 29 } else { 28 },
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
        ffi::TimeStruct { year: self.year, month: self.month, date: self.date, hour: self.hour, minutes: self.minutes }
    }
}

// セーブデータ用の構造体（serde用）
#[derive(Serialize, Deserialize, Debug)]
pub struct SaveDataJson {
    #[serde(rename = "Version")]
    pub version: String,
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
    pub gender: Vec<i32>,
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
    pub gender: Vec<i32>,
    pub work_places: Vec<RustWorkPlace>,
    pub schools: Vec<RustSchool>,
    
    // タイル上のオブジェクト情報 (ObjectStruct相当)
    pub object_structs: Vec<RustTileObjectReference>,
}

pub struct RustObject {
    pub id: i32,
    pub addon_name_en: String,
    pub original_name: String,
    pub type_id: i32,      // 内部数値
    pub direction_id: i32, // 内部数値
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

    // TypeID (数値) を C++ 側と互換性のある文字列に変換
    pub fn get_type_id_name(&self) -> String {
        // C++ の UnitaryTools::typeIDToTypeName の移植
        match self.type_id {
            0 => "Normal".to_string(),
            1 => "Road".to_string(),
            2 => "Rail".to_string(),
            3 => "Bridge".to_string(),
            // 必要に応じて C++ 側の定義に合わせて追加
            _ => "Unknown".to_string(),
        }
    }

    // DirectionID (数値) を C++ 側と互換性のある文字列に変換
    pub fn get_direction_name(&self) -> String {
        // C++ の UnitaryTools::directionIDToDirectionName の移植
        match self.direction_id {
            0 => "North".to_string(),
            1 => "East".to_string(),
            2 => "South".to_string(),
            3 => "West".to_string(),
            _ => "None".to_string(),
        }
    }
}

pub struct RustTileObjectReference {
    pub object_id: i32,
    pub relative_x: i32,
    pub relative_y: i32,
    pub visible: bool,
}

pub struct RustWorkPlace {
    pub kind: i32,
    pub serial_number: i32,
}

pub struct RustSchool {
    pub kind: i32,
    pub serial_number: i32,
}

// 新規マップ作成
fn new_city_map() -> Box<RustCityMap> {
    Box::new(RustCityMap {
        // --- 基本メタデータ ---
        version: String::from("1.0.0"), // RELEASE_NUMBER 相当
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
    })
}
