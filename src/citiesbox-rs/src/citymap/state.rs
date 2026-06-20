use super::*;

impl RustCityMap {
    pub(super) fn bulk_set_tiles(&mut self, data: &[ffi::RawTileData], width: i32, height: i32) {
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
    pub(super) fn set_status(
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
    pub(super) fn init_map_size(&mut self, width: i32, height: i32) {
        self.map_size = [width, height];
        self.tiles = (0..height)
            .map(|y| (0..width).map(|x| RustTile::default(x, y)).collect())
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

    // タイルの労働者情報を一括設定
    pub(super) fn set_tile_workers(
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
