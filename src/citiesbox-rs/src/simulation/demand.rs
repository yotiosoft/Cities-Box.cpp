use super::{SimulationState, random::SimulationRandomSource};
use crate::citymap::ffi;

const COMMERCIAL: i32 = 1;
const OFFICE: i32 = 2;
const INDUSTRIAL: i32 = 3;
const FARM: i32 = 4;

impl SimulationState {
    pub(super) fn update_daily_demand<S: SimulationRandomSource>(
        &mut self,
        demand_tiles: &[ffi::DemandTileState],
        work_place_tiles: &[ffi::WorkPlaceTileState],
        random: &mut S,
    ) {
        self.demand.residential = if self.population > 0 {
            let happiness_average = average_happiness(demand_tiles);
            hsp_demand_limit(happiness_average + 10 - random.random_below(20))
        } else {
            100.0
        };

        self.demand.commercial = update_business_demand(
            self.demand.commercial,
            has_capacity(work_place_tiles, COMMERCIAL),
            random,
        );
        self.demand.office = update_business_demand(
            self.demand.office,
            has_capacity(work_place_tiles, OFFICE),
            random,
        );
        self.demand.industrial = update_business_demand(
            self.demand.industrial,
            has_capacity(work_place_tiles, INDUSTRIAL),
            random,
        );
        self.demand.farm = update_business_demand(
            self.demand.farm,
            has_capacity(work_place_tiles, FARM),
            random,
        );
    }
}

fn average_happiness(tiles: &[ffi::DemandTileState]) -> i32 {
    if tiles.is_empty() {
        return 0;
    }
    let total = tiles
        .iter()
        .map(|tile| i64::from(tile_happiness(tile)))
        .sum::<i64>();
    (total / tiles.len() as i64).clamp(i64::from(i32::MIN), i64::from(i32::MAX)) as i32
}

fn tile_happiness(tile: &ffi::DemandTileState) -> i32 {
    // HSPの日次処理式を移植。現行モデルにNoiseがないため、その項は0として扱う。
    let land_price = ((f64::from(tile.land_price) / 200.0) * 100.0 * 0.40) as i32;
    let crime = (f64::from(tile.crime_rate / 50) * 0.20) as i32;
    let education = ((f64::from(tile.education_rate) / 60.0) * 100.0 * 0.20) as i32;
    land_price - crime + education
}

fn has_capacity(tiles: &[ffi::WorkPlaceTileState], kind: i32) -> bool {
    tiles
        .iter()
        .any(|tile| tile.kind == kind && tile.maximum_capacity > 0)
}

fn update_business_demand<S: SimulationRandomSource>(
    current: f64,
    capacity_exists: bool,
    random: &mut S,
) -> f64 {
    if !capacity_exists {
        return 100.0;
    }
    hsp_demand_limit(current as i32 + random.random_below(30) - 10)
}

fn hsp_demand_limit(value: i32) -> f64 {
    if value < 0 {
        1.0
    } else if value > 100 {
        100.0
    } else {
        f64::from(value)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::simulation::test_support::{FixedRandom, state_at};

    fn demand_tile(land_price: i32, crime_rate: i32, education_rate: i32) -> ffi::DemandTileState {
        ffi::DemandTileState {
            land_price,
            crime_rate,
            education_rate,
        }
    }

    fn workplace(kind: i32, capacity: i32) -> ffi::WorkPlaceTileState {
        ffi::WorkPlaceTileState {
            x: 0,
            y: 0,
            kind,
            serial_number: kind,
            maximum_capacity: capacity,
            workers: 0,
        }
    }

    #[test]
    fn empty_city_and_missing_business_zones_have_full_demand() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        let mut random = FixedRandom::new([]);

        state.update_daily_demand(&[], &[], &mut random);

        assert_eq!(state.demand.residential, 100.0);
        assert_eq!(state.demand.commercial, 100.0);
        assert_eq!(state.demand.office, 100.0);
        assert_eq!(state.demand.industrial, 100.0);
        assert_eq!(state.demand.farm, 100.0);
        assert!(random.upper_bounds.is_empty());
    }

    #[test]
    fn residential_demand_uses_average_happiness_and_hsp_roll() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        state.population = 1;
        let tiles = [demand_tile(100, 0, 60), demand_tile(0, 0, 0)];
        let mut random = FixedRandom::new([0]);

        state.update_daily_demand(&tiles, &[], &mut random);

        // 幸福度は40と0、平均20。乱数0なら +10。
        assert_eq!(state.demand.residential, 30.0);
        assert_eq!(random.upper_bounds, [20]);
    }

    #[test]
    fn business_demands_use_previous_value_and_hsp_roll() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        state.demand.commercial = 50.0;
        state.demand.office = 50.0;
        state.demand.industrial = 50.0;
        state.demand.farm = 50.0;
        let workplaces = [
            workplace(COMMERCIAL, 1),
            workplace(OFFICE, 1),
            workplace(INDUSTRIAL, 1),
            workplace(FARM, 1),
        ];
        let mut random = FixedRandom::new([0, 10, 29, 5]);

        state.update_daily_demand(&[], &workplaces, &mut random);

        assert_eq!(state.demand.commercial, 40.0);
        assert_eq!(state.demand.office, 50.0);
        assert_eq!(state.demand.industrial, 69.0);
        assert_eq!(state.demand.farm, 45.0);
        assert_eq!(random.upper_bounds, [30, 30, 30, 30]);
    }

    #[test]
    fn hsp_limits_negative_to_one_but_allows_exact_zero() {
        let workplaces = [workplace(COMMERCIAL, 1)];

        let mut negative = state_at(2024, 1, 1, 0, 0);
        let mut negative_random = FixedRandom::new([0]);
        negative.update_daily_demand(&[], &workplaces, &mut negative_random);
        assert_eq!(negative.demand.commercial, 1.0);

        let mut zero = state_at(2024, 1, 1, 0, 0);
        zero.demand.commercial = 10.0;
        let mut zero_random = FixedRandom::new([0]);
        zero.update_daily_demand(&[], &workplaces, &mut zero_random);
        assert_eq!(zero.demand.commercial, 0.0);
    }
}
