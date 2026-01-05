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

        // ステータス取得
        fn get_population(&self) -> i32;
        fn get_money(&self) -> i32;
        fn get_temperature(&self) -> i32;
        fn set_status(&mut self, pop: i32, money: i32, temp: i32, time: TimeStruct, demand: RCOIFstruct);
        fn get_demand(&self) -> RCOIFstruct;

        // 時間進行
        fn city_time(&mut self, minutes_delta: i32) -> TimeStruct;
        fn update_demand(&mut self);
    }
}

pub struct RustCityMap {
    population: i32,
    money: i32,
    temperature: i32,
    demand: ffi::RCOIFstruct,
    time: ffi::TimeStruct,
}

impl RustCityMap {
    fn get_population(&self) -> i32 { self.population }
    fn get_money(&self) -> i32 { self.money }
    fn get_temperature(&self) -> i32 { self.temperature }
    fn get_demand(&self) -> ffi::RCOIFstruct { self.demand.clone() }
    fn city_time(&mut self, minutes_delta: i32) -> ffi::TimeStruct {
        self.time.city_time(minutes_delta)
    }

    fn set_status(&mut self, pop: i32, money: i32, temp: i32, time: ffi::TimeStruct, demand: ffi::RCOIFstruct) {
        self.demand = demand;
        self.population = pop;
        self.money = money;
        self.temperature = temp;
        self.time = time;
    }

    fn update_demand(&mut self) {
        // Todo: 需要の更新ロジックを実装
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

fn new_city_map() -> Box<RustCityMap> {
    Box::new(RustCityMap {
        population: 0,
        money: 0,
        temperature: 0,
        demand: ffi::RCOIFstruct { residential: 0.0, commercial: 0.0, office: 0.0, industrial: 0.0, farm: 0.0 },
        time: ffi::TimeStruct { year: 0, month: 0, date: 0, hour: 0, minutes: 0 },
    })
}
