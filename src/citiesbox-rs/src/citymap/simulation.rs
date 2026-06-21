use super::*;
use rand::Rng;

const MINUTES_PER_HOUR: i64 = 60;
const HOURS_PER_DAY: i64 = 24;

trait SimulationRandomSource {
    fn next_demand_change(&mut self) -> f64;
    fn random_below(&mut self, upper_exclusive: i32) -> i32;
}

struct RandomSimulationSource {
    rng: rand::rngs::ThreadRng,
}

impl RandomSimulationSource {
    fn new() -> Self {
        Self { rng: rand::rng() }
    }
}

impl SimulationRandomSource for RandomSimulationSource {
    fn next_demand_change(&mut self) -> f64 {
        self.rng.random_range(-5.0..5.0)
    }

    fn random_below(&mut self, upper_exclusive: i32) -> i32 {
        if upper_exclusive <= 1 {
            0
        } else {
            self.rng.random_range(0..upper_exclusive)
        }
    }
}

impl RustCityMap {
    pub(super) fn simulation_snapshot(&self) -> ffi::SimulationSnapshot {
        self.simulation.snapshot()
    }

    pub(super) fn will_run_daily_update(&self, minutes_delta: i32) -> bool {
        let mut time = self.simulation.time.clone();
        time.advance_clock(minutes_delta) > 0
    }

    // C++が所有する現在の住宅タイルを値として受け取り、更新結果を返す。
    // 保存用のRustマップ写像は参照しないため、古いマップ状態では計算しない。
    pub(super) fn update_world(
        &mut self,
        minutes_delta: i32,
        mut residential_tiles: Vec<ffi::ResidentialTileState>,
        map_stats: ffi::SimulationMapStats,
    ) -> ffi::SimulationUpdate {
        let mut random = RandomSimulationSource::new();
        self.simulation.update_world_with_source(
            minutes_delta,
            &mut residential_tiles,
            &map_stats,
            &mut random,
        );
        ffi::SimulationUpdate {
            snapshot: self.simulation.snapshot(),
            residential_tiles,
        }
    }
}

impl SimulationState {
    fn snapshot(&self) -> ffi::SimulationSnapshot {
        ffi::SimulationSnapshot {
            time: self.time.clone(),
            population: self.population,
            money: self.money,
            temperature: self.temperature,
            demand: self.demand.clone(),
            budget_police: self.budget_police,
            budget_fire: self.budget_fire,
            budget_post: self.budget_post,
            budget_education: self.budget_education,
            tax_residential: self.tax_residential,
            tax_commercial: self.tax_commercial,
            tax_office: self.tax_office,
            tax_industrial: self.tax_industrial,
            tax_farm: self.tax_farm,
        }
    }

    fn update_world_with_source<S: SimulationRandomSource>(
        &mut self,
        minutes_delta: i32,
        residential_tiles: &mut [ffi::ResidentialTileState],
        map_stats: &ffi::SimulationMapStats,
        random: &mut S,
    ) -> u32 {
        let elapsed_days = self.time.advance_clock(minutes_delta);
        for _ in 0..elapsed_days {
            self.time.advance_one_day();
            self.update_daily_population(residential_tiles, random);
            if self.time.date == 1 {
                self.update_monthly_finances(map_stats);
            }
            self.update_daily_demand(random);
        }
        elapsed_days
    }

    fn update_daily_population<S: SimulationRandomSource>(
        &mut self,
        residential_tiles: &mut [ffi::ResidentialTileState],
        random: &mut S,
    ) {
        self.demand.residential = self.demand.residential.max(0.0);

        for tile in residential_tiles.iter_mut() {
            tile.residents = tile.residents.max(0);
            let before_residents = tile.residents;
            let change_chance = if before_residents == 0 { 10 } else { 100 };

            if random.random_below(change_chance) == 9 {
                if random.random_below(2) == 0 {
                    self.move_residents_in(tile, random);
                } else {
                    Self::move_residents_out(tile, random);
                }
            }

            if self.time.month == 1 && self.time.date == 1 {
                Self::age_residents(tile, random);
            }
        }

        let total = residential_tiles
            .iter()
            .map(|tile| i64::from(tile.residents.max(0)))
            .sum::<i64>();
        self.population = total.min(i64::from(i32::MAX)) as i32;
    }

    fn move_residents_in<S: SimulationRandomSource>(
        &mut self,
        tile: &mut ffi::ResidentialTileState,
        random: &mut S,
    ) {
        let free_capacity = tile.maximum_capacity.max(0) - tile.residents;
        if free_capacity <= 0 {
            return;
        }

        // 増加数は0..free_capacity未満
        let added = random.random_below(free_capacity);
        tile.residents += added;
        self.demand.residential -= f64::from(added);

        for _ in 0..added {
            let raw_age = random.random_below(120);
            tile.ages.push(if raw_age <= 4 { 0 } else { raw_age - 4 });
            tile.genders.push(if random.random_below(2) == 0 {
                "m".to_string()
            } else {
                "f".to_string()
            });
        }
    }

    fn move_residents_out<S: SimulationRandomSource>(
        tile: &mut ffi::ResidentialTileState,
        random: &mut S,
    ) {
        let before_residents = tile.residents;
        if before_residents <= 0 {
            return;
        }

        let cut_residents = random.random_below(before_residents.saturating_add(5)) - 5;
        tile.residents = if cut_residents < 0 {
            0
        } else {
            before_residents.saturating_sub(cut_residents.saturating_add(5))
        };
        tile.ages.truncate(tile.residents as usize);
        tile.genders.truncate(tile.residents as usize);
    }

    fn age_residents<S: SimulationRandomSource>(
        tile: &mut ffi::ResidentialTileState,
        random: &mut S,
    ) {
        let known_residents = tile.ages.len().min(tile.residents as usize);
        let mut index = known_residents;
        while index > 0 {
            index -= 1;
            let remaining_lifespan = (121 - tile.ages[index]).max(1);
            if random.random_below(remaining_lifespan) == 0 {
                tile.ages.remove(index);
                if index < tile.genders.len() {
                    tile.genders.remove(index);
                }
                tile.residents = tile.residents.saturating_sub(1);
            } else {
                tile.ages[index] = tile.ages[index].saturating_add(1);
            }
        }
    }

    fn update_daily_demand<S: SimulationRandomSource>(&mut self, random: &mut S) {
        self.demand.residential =
            (self.demand.residential + random.next_demand_change()).clamp(0.0, 100.0);
        self.demand.commercial =
            (self.demand.commercial + random.next_demand_change()).clamp(0.0, 100.0);
        self.demand.office = (self.demand.office + random.next_demand_change()).clamp(0.0, 100.0);
        self.demand.industrial =
            (self.demand.industrial + random.next_demand_change()).clamp(0.0, 100.0);
        self.demand.farm = (self.demand.farm + random.next_demand_change()).clamp(0.0, 100.0);
    }

    fn update_monthly_finances(&mut self, map_stats: &ffi::SimulationMapStats) {
        let expenses = [
            (map_stats.police_stations, self.budget_police),
            (map_stats.fire_departments, self.budget_fire),
            (map_stats.post_offices, self.budget_post),
            (map_stats.education_facilities, self.budget_education),
        ]
        .into_iter()
        .fold(0_i64, |total, (count, budget)| {
            total.saturating_add(
                i64::from(count.max(0))
                    .saturating_mul(i64::from(budget.max(0)))
                    .saturating_mul(10),
            )
        });

        let income = [
            (map_stats.residential_tiles, self.tax_residential),
            (map_stats.commercial_tiles, self.tax_commercial),
            (map_stats.office_tiles, self.tax_office),
            (map_stats.industrial_tiles, self.tax_industrial),
            (map_stats.farm_tiles, self.tax_farm),
        ]
        .into_iter()
        .map(|(count, tax)| f64::from(count.max(0)) * finite_nonnegative(tax))
        .sum::<f64>()
        .trunc() as i64;

        let updated_money = i64::from(self.money)
            .saturating_add(income)
            .saturating_sub(expenses);
        self.money = updated_money.clamp(i64::from(i32::MIN), i64::from(i32::MAX)) as i32;
    }
}

fn finite_nonnegative(value: f64) -> f64 {
    if value.is_finite() {
        value.max(0.0)
    } else {
        0.0
    }
}

#[cfg(test)]
pub(super) fn empty_map_stats() -> ffi::SimulationMapStats {
    ffi::SimulationMapStats {
        residential_tiles: 0,
        commercial_tiles: 0,
        office_tiles: 0,
        industrial_tiles: 0,
        farm_tiles: 0,
        police_stations: 0,
        fire_departments: 0,
        post_offices: 0,
        education_facilities: 0,
    }
}

impl ffi::TimeStruct {
    fn advance_clock(&mut self, minutes_delta: i32) -> u32 {
        if minutes_delta <= 0 {
            return 0;
        }

        let total_minutes = i64::from(self.minutes) + i64::from(minutes_delta);
        self.minutes = (total_minutes % MINUTES_PER_HOUR) as i32;

        let total_hours = i64::from(self.hour) + total_minutes / MINUTES_PER_HOUR;
        self.hour = (total_hours % HOURS_PER_DAY) as i32;
        (total_hours / HOURS_PER_DAY) as u32
    }

    fn advance_one_day(&mut self) {
        self.date += 1;
        if self.date > days_in_month(self.year, self.month) {
            self.date = 1;
            self.month += 1;
            if self.month > 12 {
                self.month = 1;
                self.year += 1;
            }
        }
    }
}

fn is_leap_year(year: i32) -> bool {
    year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)
}

fn days_in_month(year: i32, month: i32) -> i32 {
    match month {
        4 | 6 | 9 | 11 => 30,
        2 if is_leap_year(year) => 29,
        2 => 28,
        _ => 31,
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::VecDeque;

    struct FixedRandom {
        demand_change: f64,
        demand_calls: usize,
        integers: VecDeque<i32>,
        upper_bounds: Vec<i32>,
    }

    impl FixedRandom {
        fn new(integers: impl IntoIterator<Item = i32>) -> Self {
            Self {
                demand_change: 0.0,
                demand_calls: 0,
                integers: integers.into_iter().collect(),
                upper_bounds: Vec::new(),
            }
        }
    }

    impl SimulationRandomSource for FixedRandom {
        fn next_demand_change(&mut self) -> f64 {
            self.demand_calls += 1;
            self.demand_change
        }

        fn random_below(&mut self, upper_exclusive: i32) -> i32 {
            self.upper_bounds.push(upper_exclusive);
            let value = self.integers.pop_front().unwrap_or_default();
            assert!(value >= 0 && value < upper_exclusive.max(1));
            value
        }
    }

    fn state_at(year: i32, month: i32, date: i32, hour: i32, minutes: i32) -> SimulationState {
        let mut state = SimulationState::new();
        state.time = ffi::TimeStruct {
            year,
            month,
            date,
            hour,
            minutes,
        };
        state
    }

    fn residential_tile(residents: i32, maximum_capacity: i32) -> ffi::ResidentialTileState {
        ffi::ResidentialTileState {
            x: 1,
            y: 2,
            residents,
            maximum_capacity,
            ages: (0..residents).map(|age| age + 20).collect(),
            genders: (0..residents).map(|_| "m".to_string()).collect(),
        }
    }

    fn advance(state: &mut SimulationState, minutes: i32) -> (u32, usize) {
        let mut random = FixedRandom::new([]);
        let days =
            state.update_world_with_source(minutes, &mut [], &empty_map_stats(), &mut random);
        (days, random.demand_calls)
    }

    #[test]
    fn advances_sixty_minutes() {
        let mut state = state_at(2024, 1, 1, 10, 15);
        assert_eq!(advance(&mut state, 60), (0, 0));
        assert_eq!((state.time.hour, state.time.minutes), (11, 15));
    }

    #[test]
    fn advances_twenty_four_hours() {
        let mut state = state_at(2024, 1, 1, 10, 15);
        assert_eq!(advance(&mut state, 24 * 60), (1, 5));
        assert_eq!(
            (state.time.year, state.time.month, state.time.date),
            (2024, 1, 2)
        );
        assert_eq!((state.time.hour, state.time.minutes), (10, 15));
    }

    #[test]
    fn advances_across_month_and_year_ends() {
        let mut month_end = state_at(2024, 4, 30, 23, 30);
        advance(&mut month_end, 60);
        assert_eq!(
            (
                month_end.time.year,
                month_end.time.month,
                month_end.time.date,
                month_end.time.hour,
                month_end.time.minutes
            ),
            (2024, 5, 1, 0, 30)
        );

        let mut year_end = state_at(2024, 12, 31, 23, 59);
        advance(&mut year_end, 1);
        assert_eq!(
            (
                year_end.time.year,
                year_end.time.month,
                year_end.time.date,
                year_end.time.hour,
                year_end.time.minutes
            ),
            (2025, 1, 1, 0, 0)
        );
    }

    #[test]
    fn handles_common_and_leap_year_february() {
        let mut common = state_at(2023, 2, 28, 23, 59);
        advance(&mut common, 1);
        assert_eq!((common.time.month, common.time.date), (3, 1));

        let mut leap = state_at(2024, 2, 28, 23, 59);
        advance(&mut leap, 1);
        assert_eq!((leap.time.month, leap.time.date), (2, 29));
        advance(&mut leap, 24 * 60);
        assert_eq!((leap.time.month, leap.time.date), (3, 1));

        let mut century_common = state_at(2100, 2, 28, 23, 59);
        advance(&mut century_common, 1);
        assert_eq!(
            (century_common.time.month, century_common.time.date),
            (3, 1)
        );

        let mut century_leap = state_at(2000, 2, 28, 23, 59);
        advance(&mut century_leap, 1);
        assert_eq!((century_leap.time.month, century_leap.time.date), (2, 29));
    }

    #[test]
    fn runs_daily_processing_for_every_elapsed_day() {
        let mut state = state_at(2024, 2, 27, 12, 0);
        assert_eq!(advance(&mut state, 4 * 24 * 60 + 90), (4, 20));
        assert_eq!(
            (
                state.time.year,
                state.time.month,
                state.time.date,
                state.time.hour,
                state.time.minutes
            ),
            (2024, 3, 2, 13, 30)
        );
    }

    #[test]
    fn does_not_run_daily_processing_without_a_date_change() {
        let mut state = state_at(2024, 6, 1, 10, 0);
        assert_eq!(advance(&mut state, 12 * 60), (0, 0));
    }

    #[test]
    fn empty_home_has_ten_percent_move_in_check() {
        let mut state = state_at(2024, 1, 1, 23, 59);
        state.demand.residential = 50.0;
        let mut tiles = vec![residential_tile(0, 8)];
        let mut random = FixedRandom::new([9, 0, 3, 4, 0, 5, 1, 119, 0]);

        state.update_world_with_source(1, &mut tiles, &empty_map_stats(), &mut random);

        assert_eq!(tiles[0].residents, 3);
        assert_eq!(tiles[0].ages, [0, 1, 115]);
        assert_eq!(tiles[0].genders, ["m", "f", "m"]);
        assert_eq!(state.population, 3);
        assert_eq!(state.demand.residential, 47.0);
        assert_eq!(random.upper_bounds[0], 10);
    }

    #[test]
    fn occupied_home_has_one_percent_check_and_moves_at_least_five_out() {
        let mut state = state_at(2024, 1, 1, 23, 59);
        let mut tiles = vec![residential_tile(10, 20)];
        let mut random = FixedRandom::new([9, 1, 5]);

        state.update_world_with_source(1, &mut tiles, &empty_map_stats(), &mut random);

        assert_eq!(tiles[0].residents, 5);
        assert_eq!(tiles[0].ages.len(), 5);
        assert_eq!(tiles[0].genders.len(), 5);
        assert_eq!(state.population, 5);
        assert_eq!(random.upper_bounds[0], 100);
    }

    #[test]
    fn recalculates_total_population_from_all_residential_tiles() {
        let mut state = state_at(2024, 1, 1, 23, 59);
        let mut tiles = vec![residential_tile(4, 8), residential_tile(7, 20)];
        let mut random = FixedRandom::new([0, 0]);

        state.update_world_with_source(1, &mut tiles, &empty_map_stats(), &mut random);

        assert_eq!(state.population, 11);
    }

    #[test]
    fn ages_residents_and_removes_deaths_on_january_first() {
        let mut state = state_at(2024, 12, 31, 23, 59);
        let mut tile = residential_tile(2, 8);
        tile.ages = vec![119, 120];
        tile.genders = vec!["m".to_string(), "f".to_string()];
        let mut tiles = vec![tile];
        // 人口変動なし、120歳は必ず死亡、119歳は生存。
        let mut random = FixedRandom::new([0, 0, 1]);

        state.update_world_with_source(1, &mut tiles, &empty_map_stats(), &mut random);

        assert_eq!(tiles[0].residents, 1);
        assert_eq!(tiles[0].ages, [120]);
        assert_eq!(tiles[0].genders, ["m"]);
        assert_eq!(state.population, 1);
    }

    #[test]
    fn clamps_demand_to_its_allowed_range() {
        let mut high = state_at(2024, 1, 1, 0, 0);
        high.demand.residential = 99.0;
        high.demand.commercial = 99.0;
        high.demand.office = 99.0;
        high.demand.industrial = 99.0;
        high.demand.farm = 99.0;
        let mut increase = FixedRandom::new([]);
        increase.demand_change = 4.9;
        high.update_world_with_source(24 * 60, &mut [], &empty_map_stats(), &mut increase);
        assert_eq!(high.demand.residential, 100.0);
        assert_eq!(high.demand.commercial, 100.0);
        assert_eq!(high.demand.office, 100.0);
        assert_eq!(high.demand.industrial, 100.0);
        assert_eq!(high.demand.farm, 100.0);

        let mut decrease = FixedRandom::new([]);
        decrease.demand_change = -4.9;
        high.demand.residential = 1.0;
        high.demand.commercial = 1.0;
        high.demand.office = 1.0;
        high.demand.industrial = 1.0;
        high.demand.farm = 1.0;
        high.update_world_with_source(24 * 60, &mut [], &empty_map_stats(), &mut decrease);
        assert_eq!(high.demand.residential, 0.0);
        assert_eq!(high.demand.commercial, 0.0);
        assert_eq!(high.demand.office, 0.0);
        assert_eq!(high.demand.industrial, 0.0);
        assert_eq!(high.demand.farm, 0.0);
    }

    #[test]
    fn ignores_negative_minutes() {
        let mut state = state_at(2024, 1, 1, 10, 15);
        assert_eq!(advance(&mut state, -1), (0, 0));
        assert_eq!((state.time.hour, state.time.minutes), (10, 15));
    }

    #[test]
    fn collects_taxes_and_pays_budgets_on_the_first_of_the_month() {
        let mut state = state_at(2024, 4, 30, 23, 59);
        state.money = 100_000;
        state.budget_police = 100;
        state.budget_fire = 200;
        state.budget_post = 300;
        state.budget_education = 400;
        state.tax_residential = 7.0;
        state.tax_commercial = 8.0;
        state.tax_office = 9.0;
        state.tax_industrial = 10.0;
        state.tax_farm = 11.0;
        let stats = ffi::SimulationMapStats {
            residential_tiles: 10,
            commercial_tiles: 20,
            office_tiles: 30,
            industrial_tiles: 40,
            farm_tiles: 50,
            police_stations: 1,
            fire_departments: 2,
            post_offices: 3,
            education_facilities: 4,
        };
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(1, &mut [], &stats, &mut random);

        assert_eq!((state.time.month, state.time.date), (5, 1));
        assert_eq!(state.money, 71_450);
    }

    #[test]
    fn does_not_update_finances_on_other_dates() {
        let mut state = state_at(2024, 5, 1, 23, 59);
        state.money = 100_000;
        let mut stats = empty_map_stats();
        stats.residential_tiles = 10;
        stats.police_stations = 1;
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(1, &mut [], &stats, &mut random);

        assert_eq!((state.time.month, state.time.date), (5, 2));
        assert_eq!(state.money, 100_000);
    }

    #[test]
    fn updates_finances_for_every_crossed_month() {
        let mut state = state_at(2024, 1, 31, 23, 59);
        state.money = 100;
        state.tax_residential = 10.0;
        state.budget_police = 0;
        state.budget_fire = 0;
        state.budget_post = 0;
        state.budget_education = 0;
        let mut stats = empty_map_stats();
        stats.residential_tiles = 1;
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(30 * 24 * 60, &mut [], &stats, &mut random);

        assert_eq!((state.time.month, state.time.date), (3, 1));
        assert_eq!(state.money, 120);
    }
}
