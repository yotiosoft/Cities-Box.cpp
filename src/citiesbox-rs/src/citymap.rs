#[cxx::bridge(namespace = "rust::citymap")]
mod ffi {
    // C++ 側の構造体を定義（POD: Plain Old Data として）
    struct TimeStruct {
        year: i32,
        month: i32,
        date: i32,
        hour: i32,
        minutes: i32,
    }

    struct RCOIFstruct {
        r: f64,
        c: f64,
        o: f64,
        i: f64,
        f: f64,
    }

    extern "Rust" {
        type RustCityMap;

        fn new_city_map() -> Box<RustCityMap>;

        // ステータス取得
        fn get_population(&self) -> i32;
        fn get_money(&self) -> i32;
        fn get_temperature(&self) -> i32;
        fn set_status(&mut self, pop: i32, money: i32, temp: i32);

        // 時間進行ロジック
        fn city_time(&mut self, minutes_delta: i32) -> TimeStruct;
    }
}

pub struct RustCityMap {
    population: i32,
    money: i32,
    temperature: i32,
    time: ffi::TimeStruct,
}

impl RustCityMap {
    fn get_population(&self) -> i32 { self.population }
    fn get_money(&self) -> i32 { self.money }
    fn get_temperature(&self) -> i32 { self.temperature }

    fn set_status(&mut self, pop: i32, money: i32, temp: i32) {
        self.population = pop;
        self.money = money;
        self.temperature = temp;
    }

    // CityMap_GetData.cpp の cityTime ロジックを移植
    fn city_time(&mut self, minutes_delta: i32) -> ffi::TimeStruct {
        self.time.minutes += minutes_delta;

        if self.time.minutes >= 60 {
            self.time.hour += 1;
            self.time.minutes -= 60;

            if self.time.hour >= 24 {
                self.time.date += 1;
                self.time.hour -= 24;

                let month = self.time.month;
                let is_leap_year = self.time.year % 4 == 0;

                let days_in_month = match month {
                    4 | 6 | 9 | 11 => 30,
                    2 => if is_leap_year { 29 } else { 28 },
                    _ => 31,
                };

                if self.time.date > days_in_month {
                    self.time.month += 1;
                    self.time.date = 1;

                    if self.time.month > 12 {
                        self.time.year += 1;
                        self.time.month = 1;
                    }
                }
            }
        }
        ffi::TimeStruct { ..self.time }
    }
}

fn new_city_map() -> Box<RustCityMap> {
    Box::new(RustCityMap {
        population: 0,
        money: 0,
        temperature: 0,
        time: ffi::TimeStruct { year: 2026, month: 1, date: 1, hour: 10, minutes: 0 },
    })
}
