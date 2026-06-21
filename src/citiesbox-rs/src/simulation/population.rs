use super::{SimulationState, random::SimulationRandomSource};
use crate::citymap::ffi;

impl SimulationState {
    pub(super) fn update_daily_population<S: SimulationRandomSource>(
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
}

#[cfg(test)]
mod tests {
    use crate::citymap::ffi;
    use crate::simulation::{
        empty_map_stats,
        test_support::{FixedRandom, residential_tile, state_at},
    };

    #[test]
    fn empty_home_has_ten_percent_move_in_check() {
        let mut state = state_at(2024, 1, 1, 23, 59);
        state.demand.residential = 50.0;
        let mut tiles = vec![residential_tile(0, 8)];
        let mut random = FixedRandom::new([9, 0, 3, 4, 0, 5, 1, 119, 0]);

        state.update_world_with_source(
            1,
            &mut tiles,
            &mut [],
            &mut [],
            &[],
            &empty_map_stats(),
            &mut random,
        );

        assert_eq!(tiles[0].residents, 3);
        assert_eq!(tiles[0].ages, [0, 1, 115]);
        assert_eq!(tiles[0].genders, ["m", "f", "m"]);
        assert_eq!(state.population, 3);
        // HSPと同様、入居による減算後に日次需要式で再計算される。
        assert_eq!(state.demand.residential, 10.0);
        assert_eq!(random.upper_bounds[0], 10);
    }

    #[test]
    fn occupied_home_has_one_percent_check_and_moves_at_least_five_out() {
        let mut state = state_at(2024, 1, 1, 23, 59);
        let mut tiles = vec![residential_tile(10, 20)];
        let mut random = FixedRandom::new([9, 1, 5]);

        state.update_world_with_source(
            1,
            &mut tiles,
            &mut [],
            &mut [],
            &[],
            &empty_map_stats(),
            &mut random,
        );

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

        state.update_world_with_source(
            1,
            &mut tiles,
            &mut [],
            &mut [],
            &[],
            &empty_map_stats(),
            &mut random,
        );

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

        state.update_world_with_source(
            1,
            &mut tiles,
            &mut [],
            &mut [],
            &[],
            &empty_map_stats(),
            &mut random,
        );

        assert_eq!(tiles[0].residents, 1);
        assert_eq!(tiles[0].ages, [120]);
        assert_eq!(tiles[0].genders, ["m"]);
        assert_eq!(state.population, 1);
    }

    #[test]
    fn moving_out_reconciles_household_jobs_and_workplace_workers() {
        let mut state = state_at(2024, 1, 1, 23, 59);
        let mut tile = residential_tile(10, 20);
        tile.work_place_kinds = vec![1; 10];
        tile.work_place_serial_numbers = vec![42; 10];
        let mut tiles = vec![tile];
        let mut workplaces = vec![ffi::WorkPlaceTileState {
            x: 3,
            y: 4,
            kind: 1,
            serial_number: 42,
            maximum_capacity: 20,
            workers: 10,
        }];
        let mut random = FixedRandom::new([9, 1, 5]);

        state.update_world_with_source(
            1,
            &mut tiles,
            &mut workplaces,
            &mut [],
            &[],
            &empty_map_stats(),
            &mut random,
        );

        assert_eq!(tiles[0].residents, 5);
        assert_eq!(tiles[0].work_place_kinds.len(), 5);
        assert_eq!(tiles[0].work_place_serial_numbers.len(), 5);
        assert_eq!(workplaces[0].workers, 5);
    }
}
