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
        special_demand: &ffi::SpecialDemandState,
        random: &mut S,
    ) {
        let residential = if self.population > 0 {
            let happiness_average = average_happiness(demand_tiles);
            hsp_demand_limit(
                happiness_average
                    .saturating_add(10 - random.random_below(20))
                    .saturating_add(special_demand.residential),
            )
        } else {
            100.0
        };
        self.demand.residential = apply_tax_penalty(residential, self.tax_residential);

        self.demand.commercial = apply_tax_penalty(
            update_business_demand(
                self.demand.commercial,
                has_capacity(work_place_tiles, COMMERCIAL),
                special_demand.commercial,
                random,
            ),
            self.tax_commercial,
        );
        self.demand.office = apply_tax_penalty(
            update_business_demand(
                self.demand.office,
                has_capacity(work_place_tiles, OFFICE),
                special_demand.office,
                random,
            ),
            self.tax_office,
        );
        self.demand.industrial = apply_tax_penalty(
            update_business_demand(
                self.demand.industrial,
                has_capacity(work_place_tiles, INDUSTRIAL),
                special_demand.industrial,
                random,
            ),
            self.tax_industrial,
        );
        self.demand.farm = apply_tax_penalty(
            update_business_demand(
                self.demand.farm,
                has_capacity(work_place_tiles, FARM),
                special_demand.farm,
                random,
            ),
            self.tax_farm,
        );
    }
}

fn apply_tax_penalty(demand: f64, tax: f64) -> f64 {
    let penalty = (1.0 - super::state::tax_attractiveness(tax)) * 100.0;
    (demand - penalty).clamp(0.0, 100.0)
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
    // HSPの日次処理式を移植。各項はHSPのint変換と同じく個別に切り捨てる。
    let land_price = ((f64::from(tile.land_price) / 200.0) * 100.0 * 0.40) as i32;
    let crime = (f64::from(tile.crime_rate / 50) * 0.20) as i32;
    let education = ((f64::from(tile.education_rate) / 60.0) * 100.0 * 0.20) as i32;
    let noise = (f64::from(tile.noise_rate) * 0.20) as i32;
    land_price
        .saturating_sub(crime)
        .saturating_add(education)
        .saturating_sub(noise)
}

fn has_capacity(tiles: &[ffi::WorkPlaceTileState], kind: i32) -> bool {
    tiles
        .iter()
        .any(|tile| tile.kind == kind && tile.maximum_capacity > 0)
}

fn update_business_demand<S: SimulationRandomSource>(
    current: f64,
    capacity_exists: bool,
    special_demand: i32,
    random: &mut S,
) -> f64 {
    if !capacity_exists {
        return 100.0;
    }
    hsp_demand_limit(
        (current as i32)
            .saturating_add(random.random_below(30) - 10)
            .saturating_add(special_demand),
    )
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
            noise_rate: 0,
        }
    }

    fn demand_tile_with_noise(
        land_price: i32,
        crime_rate: i32,
        education_rate: i32,
        noise_rate: i32,
    ) -> ffi::DemandTileState {
        ffi::DemandTileState {
            land_price,
            crime_rate,
            education_rate,
            noise_rate,
        }
    }

    fn special_demand() -> ffi::SpecialDemandState {
        ffi::SpecialDemandState {
            residential: 0,
            commercial: 0,
            office: 0,
            industrial: 0,
            farm: 0,
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

        state.update_daily_demand(&[], &[], &special_demand(), &mut random);

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

        state.update_daily_demand(&tiles, &[], &special_demand(), &mut random);

        // 幸福度は40と0、平均20。乱数0なら +10。
        assert_eq!(state.demand.residential, 30.0);
        assert_eq!(random.upper_bounds, [20]);
    }

    #[test]
    fn noise_reduces_residential_happiness_by_twenty_percent() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        state.population = 1;
        let mut random = FixedRandom::new([0]);

        state.update_daily_demand(
            &[demand_tile_with_noise(100, 0, 60, 25)],
            &[],
            &special_demand(),
            &mut random,
        );

        // 地価20 + 教育20 - 騒音5 + 乱数補正10。
        assert_eq!(state.demand.residential, 45.0);
        assert_eq!(random.upper_bounds, [20]);
    }

    #[test]
    fn special_demand_is_applied_to_each_hsp_demand_formula_and_clamped() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        state.population = 1;
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
        let special = ffi::SpecialDemandState {
            residential: 5,
            commercial: 6,
            office: -7,
            industrial: 100,
            farm: -100,
        };
        let mut random = FixedRandom::new([10, 10, 10, 10, 10]);

        state.update_daily_demand(
            &[demand_tile(100, 0, 60)],
            &workplaces,
            &special,
            &mut random,
        );

        assert_eq!(state.demand.residential, 45.0);
        assert_eq!(state.demand.commercial, 56.0);
        assert_eq!(state.demand.office, 43.0);
        assert_eq!(state.demand.industrial, 100.0);
        assert_eq!(state.demand.farm, 1.0);
        assert_eq!(random.upper_bounds, [20, 30, 30, 30, 30]);
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

        state.update_daily_demand(&[], &workplaces, &special_demand(), &mut random);

        assert_eq!(state.demand.commercial, 40.0);
        assert_eq!(state.demand.office, 50.0);
        assert_eq!(state.demand.industrial, 69.0);
        assert_eq!(state.demand.farm, 45.0);
        assert_eq!(random.upper_bounds, [30, 30, 30, 30]);
    }

    #[test]
    fn high_taxes_reduce_only_the_corresponding_demands() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        state.population = 1;
        state.tax_residential = 30.0;
        state.tax_commercial = 30.0;
        state.tax_office = 10.0;
        state.tax_industrial = 10.0;
        state.tax_farm = 10.0;
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
        let mut random = FixedRandom::new([0, 10, 10, 10, 10]);

        state.update_daily_demand(
            &[demand_tile(200, 0, 60)],
            &workplaces,
            &special_demand(),
            &mut random,
        );

        assert_eq!(state.demand.residential, 20.0);
        assert_eq!(state.demand.commercial, 0.0);
        assert_eq!(state.demand.office, 50.0);
        assert_eq!(state.demand.industrial, 50.0);
        assert_eq!(state.demand.farm, 50.0);
    }

    #[test]
    fn hsp_limits_negative_to_one_but_allows_exact_zero() {
        let workplaces = [workplace(COMMERCIAL, 1)];

        let mut negative = state_at(2024, 1, 1, 0, 0);
        let mut negative_random = FixedRandom::new([0]);
        negative.update_daily_demand(&[], &workplaces, &special_demand(), &mut negative_random);
        assert_eq!(negative.demand.commercial, 1.0);

        let mut zero = state_at(2024, 1, 1, 0, 0);
        zero.demand.commercial = 10.0;
        let mut zero_random = FixedRandom::new([0]);
        zero.update_daily_demand(&[], &workplaces, &special_demand(), &mut zero_random);
        assert_eq!(zero.demand.commercial, 0.0);
    }

    #[test]
    fn all_demands_stay_within_the_supported_range() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        state.population = 1;
        state.demand = ffi::RCOIFstruct {
            residential: -1_000.0,
            commercial: -1_000.0,
            office: 1_000.0,
            industrial: -1_000.0,
            farm: 1_000.0,
        };
        let workplaces = [
            workplace(COMMERCIAL, 1),
            workplace(OFFICE, 1),
            workplace(INDUSTRIAL, 1),
            workplace(FARM, 1),
        ];
        let mut random = FixedRandom::new([19, 0, 29, 0, 29]);

        state.update_daily_demand(
            &[demand_tile(i32::MAX, i32::MIN, i32::MAX)],
            &workplaces,
            &special_demand(),
            &mut random,
        );

        for demand in [
            state.demand.residential,
            state.demand.commercial,
            state.demand.office,
            state.demand.industrial,
            state.demand.farm,
        ] {
            assert!(
                (0.0..=100.0).contains(&demand),
                "out-of-range demand: {demand}"
            );
        }
    }
}
