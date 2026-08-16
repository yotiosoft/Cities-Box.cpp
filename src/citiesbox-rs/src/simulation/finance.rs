use super::SimulationState;
use crate::citymap::ffi;
use std::collections::HashSet;

const CONSTRUCTION_COST: i32 = 5;

// Values are shared with CategoryID::Type in src/Enums.hpp.
const CATEGORY_RESIDENTIAL: i32 = 19;
const CATEGORY_COMMERCIAL: i32 = 20;
const CATEGORY_OFFICE: i32 = 21;
const CATEGORY_INDUSTRIAL: i32 = 22;
const CATEGORY_FARM: i32 = 23;
const CATEGORY_FIRE_DEPARTMENT: i32 = 25;
const CATEGORY_POLICE: i32 = 26;
const CATEGORY_POST_OFFICE: i32 = 27;
const CATEGORY_EDUCATION: i32 = 28;

#[derive(Default)]
struct FinanceObjectCounts {
    residential: i64,
    commercial: i64,
    office: i64,
    industrial: i64,
    farm: i64,
    police: i64,
    fire: i64,
    post: i64,
    education: i64,
}

impl SimulationState {
    pub(super) fn charge_construction_cost(&mut self) {
        self.money = self.money.saturating_sub(CONSTRUCTION_COST);
    }

    pub(super) fn update_monthly_finances(
        &mut self,
        simulation_objects: &[ffi::SimulationObjectState],
    ) {
        let counts = count_finance_objects(simulation_objects);
        let expenses = [
            (counts.police, self.budget_police),
            (counts.fire, self.budget_fire),
            (counts.post, self.budget_post),
            (counts.education, self.budget_education),
        ]
        .into_iter()
        .fold(0_i64, |total, (count, budget)| {
            total.saturating_add(
                count
                    .saturating_mul(i64::from(budget.max(0)))
                    .saturating_mul(10),
            )
        });

        let income = [
            (counts.residential, self.tax_residential),
            (counts.commercial, self.tax_commercial),
            (counts.office, self.tax_office),
            (counts.industrial, self.tax_industrial),
            (counts.farm, self.tax_farm),
        ]
        .into_iter()
        .map(|(count, tax)| count as f64 * finite_nonnegative(tax))
        .sum::<f64>()
        .trunc() as i64;

        let updated_money = i64::from(self.money)
            .saturating_add(income)
            .saturating_sub(expenses);
        self.money = updated_money.clamp(i64::from(i32::MIN), i64::from(i32::MAX)) as i32;
    }
}

fn count_finance_objects(simulation_objects: &[ffi::SimulationObjectState]) -> FinanceObjectCounts {
    let mut counts = FinanceObjectCounts::default();
    let mut seen_object_ids = HashSet::new();

    for object in simulation_objects {
        if !seen_object_ids.insert(object.object_id) {
            continue;
        }

        let categories = &object.category_ids;
        counts.residential += i64::from(categories.contains(&CATEGORY_RESIDENTIAL));
        counts.commercial += i64::from(categories.contains(&CATEGORY_COMMERCIAL));
        counts.office += i64::from(categories.contains(&CATEGORY_OFFICE));
        counts.industrial += i64::from(categories.contains(&CATEGORY_INDUSTRIAL));
        counts.farm += i64::from(categories.contains(&CATEGORY_FARM));
        counts.police += i64::from(categories.contains(&CATEGORY_POLICE));
        counts.fire += i64::from(categories.contains(&CATEGORY_FIRE_DEPARTMENT));
        counts.post += i64::from(categories.contains(&CATEGORY_POST_OFFICE));
        counts.education += i64::from(categories.contains(&CATEGORY_EDUCATION));
    }

    counts
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
    use super::{
        CATEGORY_COMMERCIAL, CATEGORY_EDUCATION, CATEGORY_FARM, CATEGORY_FIRE_DEPARTMENT,
        CATEGORY_INDUSTRIAL, CATEGORY_OFFICE, CATEGORY_POLICE, CATEGORY_POST_OFFICE,
        CATEGORY_RESIDENTIAL,
    };
    use crate::{
        citymap::ffi,
        simulation::test_support::{state_at, FixedRandom},
    };

    fn simulation_objects(
        categories_and_counts: &[(i32, usize)],
    ) -> Vec<ffi::SimulationObjectState> {
        let mut next_id = 1;
        let mut objects = Vec::new();
        for &(category_id, count) in categories_and_counts {
            for _ in 0..count {
                objects.push(ffi::SimulationObjectState {
                    object_id: next_id,
                    category_ids: vec![category_id],
                });
                next_id += 1;
            }
        }
        objects
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
        let objects = simulation_objects(&[
            (CATEGORY_RESIDENTIAL, 10),
            (CATEGORY_COMMERCIAL, 20),
            (CATEGORY_OFFICE, 30),
            (CATEGORY_INDUSTRIAL, 40),
            (CATEGORY_FARM, 50),
            (CATEGORY_POLICE, 1),
            (CATEGORY_FIRE_DEPARTMENT, 2),
            (CATEGORY_POST_OFFICE, 3),
            (CATEGORY_EDUCATION, 4),
        ]);
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(1, &mut [], &mut [], &mut [], &[], &objects, &mut random);

        assert_eq!((state.time.month, state.time.date), (5, 1));
        assert_eq!(state.money, 71_450);
    }

    #[test]
    fn does_not_update_finances_on_other_dates() {
        let mut state = state_at(2024, 5, 1, 23, 59);
        state.money = 100_000;
        let objects = simulation_objects(&[(CATEGORY_RESIDENTIAL, 10), (CATEGORY_POLICE, 1)]);
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(1, &mut [], &mut [], &mut [], &[], &objects, &mut random);

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
        let objects = simulation_objects(&[(CATEGORY_RESIDENTIAL, 1)]);
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(
            30 * 24 * 60,
            &mut [],
            &mut [],
            &mut [],
            &[],
            &objects,
            &mut random,
        );

        assert_eq!((state.time.month, state.time.date), (3, 1));
        assert_eq!(state.money, 120);
    }

    #[test]
    fn updates_finances_across_the_year_boundary() {
        let mut state = state_at(2024, 12, 31, 23, 59);
        state.money = 100;
        state.tax_residential = 10.0;
        state.budget_police = 0;
        state.budget_fire = 0;
        state.budget_post = 0;
        state.budget_education = 0;
        let objects = simulation_objects(&[(CATEGORY_RESIDENTIAL, 1)]);
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(1, &mut [], &mut [], &mut [], &[], &objects, &mut random);

        assert_eq!(
            (state.time.year, state.time.month, state.time.date),
            (2025, 1, 1)
        );
        assert_eq!(state.money, 110);
    }

    #[test]
    fn applies_changed_settings_at_the_next_monthly_update() {
        let mut state = state_at(2024, 4, 30, 23, 59);
        state.money = 100;
        state.set_finance_settings(
            ffi::BudgetSettings {
                police: 2,
                fire: 0,
                post: 0,
                education: 0,
            },
            ffi::TaxSettings {
                residential: 7.0,
                commercial: 0.0,
                office: 0.0,
                industrial: 0.0,
                farm: 0.0,
            },
        );
        let objects = simulation_objects(&[(CATEGORY_RESIDENTIAL, 10), (CATEGORY_POLICE, 1)]);
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(1, &mut [], &mut [], &mut [], &[], &objects, &mut random);

        assert_eq!(state.money, 150);
    }

    #[test]
    fn saturates_money_for_extreme_tax_income() {
        let mut state = state_at(2024, 4, 30, 23, 59);
        state.money = i32::MAX - 1;
        state.tax_residential = f64::MAX;
        state.budget_police = 100;
        let objects = simulation_objects(&[(CATEGORY_RESIDENTIAL, 1)]);
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(1, &mut [], &mut [], &mut [], &[], &objects, &mut random);

        assert_eq!(state.money, i32::MAX);
    }

    #[test]
    fn counts_multi_tile_object_and_each_category_only_once() {
        let objects = vec![
            ffi::SimulationObjectState {
                object_id: 10,
                category_ids: vec![CATEGORY_POLICE, CATEGORY_POLICE, CATEGORY_EDUCATION],
            },
            ffi::SimulationObjectState {
                object_id: 10,
                category_ids: vec![CATEGORY_POLICE],
            },
            ffi::SimulationObjectState {
                object_id: 11,
                category_ids: vec![CATEGORY_POLICE],
            },
        ];

        let counts = super::count_finance_objects(&objects);

        assert_eq!(counts.police, 2);
        assert_eq!(counts.education, 1);
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
