use super::SimulationState;
use crate::citymap::ffi;

const CONSTRUCTION_COST: i32 = 5;

impl SimulationState {
    pub(super) fn charge_construction_cost(&mut self) {
        self.money = self.money.saturating_sub(CONSTRUCTION_COST);
    }

    pub(super) fn update_monthly_finances(&mut self, map_stats: &ffi::SimulationMapStats) {
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
mod tests {
    use crate::{
        citymap::ffi,
        simulation::{
            empty_map_stats,
            test_support::{FixedRandom, state_at},
        },
    };

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

        state.update_world_with_source(1, &mut [], &mut [], &mut [], &[], &stats, &mut random);

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

        state.update_world_with_source(1, &mut [], &mut [], &mut [], &[], &stats, &mut random);

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

        state.update_world_with_source(
            30 * 24 * 60,
            &mut [],
            &mut [],
            &mut [],
            &[],
            &stats,
            &mut random,
        );

        assert_eq!((state.time.month, state.time.date), (3, 1));
        assert_eq!(state.money, 120);
    }

    #[test]
    fn charges_five_for_each_successful_construction() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        state.money = 100;

        state.charge_construction_cost();
        state.charge_construction_cost();

        assert_eq!(state.money, 90);
    }
}
