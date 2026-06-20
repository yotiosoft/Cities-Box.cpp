use super::*;

impl RustCityMap {
    pub(super) fn get_population(&self) -> i32 {
        self.population
    }
    pub(super) fn get_money(&self) -> i32 {
        self.money
    }
    pub(super) fn get_temperature(&self) -> i32 {
        self.temperature
    }
    pub(super) fn get_demand(&self) -> ffi::RCOIFstruct {
        self.demand.clone()
    }

    // 時間進行
    pub(super) fn city_time(&mut self, minutes_delta: i32) -> ffi::TimeStruct {
        self.time.city_time(minutes_delta)
    }
    // 時間進行 + 需要度更新
    pub(super) fn update_world(&mut self, minutes_delta: i32) -> ffi::TimeStruct {
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
