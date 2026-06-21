use super::*;

pub(super) struct SimulationState {
    pub(super) time: ffi::TimeStruct,
    pub(super) population: i32,
    pub(super) money: i32,
    pub(super) temperature: i32,
    pub(super) demand: ffi::RCOIFstruct,
    pub(super) budget_police: i32,
    pub(super) budget_fire: i32,
    pub(super) budget_post: i32,
    pub(super) budget_education: i32,
    pub(super) tax_residential: f64,
    pub(super) tax_commercial: f64,
    pub(super) tax_office: f64,
    pub(super) tax_industrial: f64,
    pub(super) tax_farm: f64,
}

impl SimulationState {
    pub(super) fn new() -> Self {
        Self {
            time: ffi::TimeStruct {
                year: 2024,
                month: 1,
                date: 1,
                hour: 0,
                minutes: 0,
            },
            population: 0,
            money: 100_000,
            temperature: 20,
            demand: ffi::RCOIFstruct {
                residential: 0.0,
                commercial: 0.0,
                office: 0.0,
                industrial: 0.0,
                farm: 0.0,
            },
            budget_police: 100,
            budget_fire: 100,
            budget_post: 100,
            budget_education: 100,
            tax_residential: 7.0,
            tax_commercial: 7.0,
            tax_office: 7.0,
            tax_industrial: 7.0,
            tax_farm: 7.0,
        }
    }
}

impl RustCityMap {
    pub(super) fn bulk_set_tiles(&mut self, data: &[ffi::RawTileData], width: i32, height: i32) {
        self.map_size = [width, height];
        // データを流し込む（統計やレート等は別途Setterを呼ぶか、一旦クリア）
        self.tiles = data
            .iter()
            .map(|d| RustTile {
                residents: d.residents,
                workers_commercial: d.workers_commercial,
                workers_office: d.workers_office,
                workers_industrial: d.workers_industrial,
                workers_farm: d.workers_farm,
                workers_public: d.workers_public,
                students: d.students,
                reservation: d.reservation,
                ..Default::default()
            })
            .collect::<Vec<_>>() // 一旦フラットなリストとして作り
            .chunks(width as usize) // widthごとに分割して Vec<Vec<RustTile>> に
            .map(|chunk| chunk.to_vec())
            .collect();
    }

    pub(super) fn add_object(
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

    pub(super) fn set_tile_basic(
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

    pub(super) fn add_tile_object_ref(
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

    pub(super) fn add_tile_rate(&mut self, x: i32, y: i32, key: String, value: i32) {
        if let Some(tile) = self
            .tiles
            .get_mut(y as usize)
            .and_then(|row| row.get_mut(x as usize))
        {
            tile.rate.insert(key, value);
        }
    }

    // オブジェクトリストを空にする（同期の開始時に呼ぶ）
    pub(super) fn clear_objects(&mut self) {
        self.objects.clear();
    }

    // タイルの統計データ（年齢・性別）を設定
    // cxxbridge を介して Vec<i32> を直接受け取る
    pub(super) fn set_tile_stats(&mut self, x: i32, y: i32, ages: Vec<i32>, genders: Vec<String>) {
        if let Some(tile) = self
            .tiles
            .get_mut(y as usize)
            .and_then(|row| row.get_mut(x as usize))
        {
            tile.age = ages;
            tile.gender = genders;
        }
    }

    pub(super) fn add_tile_work_place(&mut self, x: i32, y: i32, kind: i32, serial_number: i32) {
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

    pub(super) fn add_tile_school(&mut self, x: i32, y: i32, kind: i32, serial_number: i32) {
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
}
