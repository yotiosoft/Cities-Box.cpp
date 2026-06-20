use super::*;

pub(super) fn sidecar_path(path: &Path, suffix: &str) -> PathBuf {
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

impl RustCityMap {
    pub(super) fn set_save_version(&mut self, version: i32) {
        self.version = version;
    }

    pub(super) fn set_city_metadata(
        &mut self,
        city_name: String,
        mayor_name: String,
        addon_set: String,
    ) {
        self.city_name = city_name;
        self.mayor_name = mayor_name;
        self.addon_set_name = addon_set;
    }

    pub(super) fn set_display_settings(&mut self, weather: bool, night: bool) {
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
            total_population: self.simulation.population,
            change_weather: self.change_weather,
            temperature: self.simulation.temperature,
            dark_on_night: self.dark_on_night,
            map_size: MapSizeJson {
                width: self.map_size[0],
                height: self.map_size[1],
            },
            time: TimeJson {
                year: self.simulation.time.year,
                month: self.simulation.time.month,
                date: self.simulation.time.date,
                hour: self.simulation.time.hour,
                minutes: self.simulation.time.minutes,
            },
            demand: DemandJson {
                residential: self.simulation.demand.residential,
                commercial: self.simulation.demand.commercial,
                office: self.simulation.demand.office,
                industrial: self.simulation.demand.industrial,
                farm: self.simulation.demand.farm,
            },
            money: self.simulation.money,
            budget: BudgetJson {
                police: self.simulation.budget_police,
                fire_depertment: self.simulation.budget_fire,
                post_office: self.simulation.budget_post,
                education: self.simulation.budget_education,
            },
            tax: TaxJson {
                residential: self.simulation.tax_residential,
                commercial: self.simulation.tax_commercial,
                office: self.simulation.tax_office,
                industrial: self.simulation.tax_industrial,
                farm: self.simulation.tax_farm,
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
