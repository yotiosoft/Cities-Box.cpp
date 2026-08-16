use super::{
    SimulationState,
    employment::{COMMERCIAL, FARM, INDUSTRIAL, OFFICE},
};
use crate::citymap::ffi;
use std::collections::{HashMap, HashSet};

const CONSTRUCTION_COST: i32 = 5;
const RESIDENTIAL_TAX_BASE_PER_RESIDENT: f64 = 100.0;
const BUSINESS_TAX_BASE_PER_WORKER: f64 = 100.0;

// Values are shared with CategoryID::Type in src/Enums.hpp.
const CATEGORY_FIRE_DEPARTMENT: i32 = 25;
const CATEGORY_POLICE: i32 = 26;
const CATEGORY_POST_OFFICE: i32 = 27;
const CATEGORY_EDUCATION: i32 = 28;

#[derive(Default)]
struct FinanceObjectCounts {
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
        work_place_tiles: &[ffi::WorkPlaceTileState],
        simulation_objects: &[ffi::SimulationObjectState],
    ) {
        let counts = count_finance_objects(simulation_objects);
        let workers = count_business_workers(work_place_tiles);
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

        let residential_income = f64::from(self.population.max(0))
            * RESIDENTIAL_TAX_BASE_PER_RESIDENT
            * finite_nonnegative(self.tax_residential)
            / 100.0;
        let income = residential_income
            + [
                (workers.commercial, self.tax_commercial),
                (workers.office, self.tax_office),
                (workers.industrial, self.tax_industrial),
                (workers.farm, self.tax_farm),
            ]
            .into_iter()
            .map(|(count, tax)| {
                count as f64 * BUSINESS_TAX_BASE_PER_WORKER * finite_nonnegative(tax) / 100.0
            })
            .sum::<f64>();
        let income = income.trunc() as i64;

        let updated_money = i64::from(self.money)
            .saturating_add(income)
            .saturating_sub(expenses);
        self.money = updated_money.clamp(i64::from(i32::MIN), i64::from(i32::MAX)) as i32;
    }
}

#[derive(Default)]
struct BusinessWorkerCounts {
    commercial: i64,
    office: i64,
    industrial: i64,
    farm: i64,
}

fn count_business_workers(work_place_tiles: &[ffi::WorkPlaceTileState]) -> BusinessWorkerCounts {
    let mut workplaces = HashMap::new();
    for tile in work_place_tiles {
        if tile.serial_number < 0 || !matches!(tile.kind, COMMERCIAL | OFFICE | INDUSTRIAL | FARM) {
            continue;
        }

        workplaces
            .entry((tile.kind, tile.serial_number))
            .and_modify(|workers: &mut i64| {
                *workers = (*workers).max(i64::from(tile.workers.max(0)));
            })
            .or_insert_with(|| i64::from(tile.workers.max(0)));
    }

    let mut counts = BusinessWorkerCounts::default();
    for ((kind, _), workers) in workplaces {
        match kind {
            COMMERCIAL => counts.commercial = counts.commercial.saturating_add(workers),
            OFFICE => counts.office = counts.office.saturating_add(workers),
            INDUSTRIAL => counts.industrial = counts.industrial.saturating_add(workers),
            FARM => counts.farm = counts.farm.saturating_add(workers),
            _ => {}
        }
    }
    counts
}

fn count_finance_objects(simulation_objects: &[ffi::SimulationObjectState]) -> FinanceObjectCounts {
    let mut counts = FinanceObjectCounts::default();
    let mut seen_object_ids = HashSet::new();

    for object in simulation_objects {
        if !seen_object_ids.insert(object.object_id) {
            continue;
        }

        let categories = &object.category_ids;
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
        CATEGORY_EDUCATION, CATEGORY_FIRE_DEPARTMENT, CATEGORY_POLICE, CATEGORY_POST_OFFICE,
    };
    use crate::{
        citymap::ffi,
        simulation::{
            employment::{COMMERCIAL, FARM, INDUSTRIAL, OFFICE, PUBLIC},
            test_support::{FixedRandom, residential_tile, state_at},
        },
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

    fn workplace(
        kind: i32,
        serial_number: i32,
        maximum_capacity: i32,
        workers: i32,
    ) -> ffi::WorkPlaceTileState {
        ffi::WorkPlaceTileState {
            x: 0,
            y: 0,
            kind,
            serial_number,
            maximum_capacity,
            workers,
        }
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
        let mut residential_tiles = vec![residential_tile(10, 10)];
        let objects = simulation_objects(&[
            (CATEGORY_POLICE, 1),
            (CATEGORY_FIRE_DEPARTMENT, 2),
            (CATEGORY_POST_OFFICE, 3),
            (CATEGORY_EDUCATION, 4),
        ]);
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(
            1,
            &mut residential_tiles,
            &mut [],
            &mut [],
            &[],
            &objects,
            &mut random,
        );

        assert_eq!((state.time.month, state.time.date), (5, 1));
        assert_eq!(state.money, 70_070);
    }

    #[test]
    fn does_not_update_finances_on_other_dates() {
        let mut state = state_at(2024, 5, 1, 23, 59);
        state.money = 100_000;
        let objects = simulation_objects(&[(CATEGORY_POLICE, 1)]);
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
        let mut residential_tiles = vec![residential_tile(1, 1)];
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(
            30 * 24 * 60,
            &mut residential_tiles,
            &mut [],
            &mut [],
            &[],
            &[],
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
        let mut tile = residential_tile(1, 1);
        tile.ages.clear();
        tile.genders.clear();
        let mut residential_tiles = vec![tile];
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(
            1,
            &mut residential_tiles,
            &mut [],
            &mut [],
            &[],
            &[],
            &mut random,
        );

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
        let mut residential_tiles = vec![residential_tile(10, 10)];
        let objects = simulation_objects(&[(CATEGORY_POLICE, 1)]);
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(
            1,
            &mut residential_tiles,
            &mut [],
            &mut [],
            &[],
            &objects,
            &mut random,
        );

        assert_eq!(state.money, 150);
    }

    #[test]
    fn saturates_money_for_extreme_tax_income() {
        let mut state = state_at(2024, 4, 30, 23, 59);
        state.money = i32::MAX - 1;
        state.tax_residential = f64::MAX;
        state.budget_police = 100;
        let mut residential_tiles = vec![residential_tile(1, 1)];
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(
            1,
            &mut residential_tiles,
            &mut [],
            &mut [],
            &[],
            &[],
            &mut random,
        );

        assert_eq!(state.money, i32::MAX);
    }

    #[test]
    fn business_tax_uses_workers_for_each_tax_category() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        state.money = 100;
        state.population = 0;
        state.tax_residential = 0.0;
        state.tax_commercial = 7.0;
        state.tax_office = 8.0;
        state.tax_industrial = 9.0;
        state.tax_farm = 10.0;
        let workplaces = vec![
            workplace(COMMERCIAL, 1, 10, 10),
            workplace(OFFICE, 2, 20, 20),
            workplace(INDUSTRIAL, 3, 30, 30),
            workplace(FARM, 4, 40, 40),
        ];

        state.update_monthly_finances(&workplaces, &[]);

        assert_eq!(state.money, 1_000);
    }

    #[test]
    fn business_tax_supports_fractional_percent_rates() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        state.money = 0;
        state.population = 0;
        state.tax_residential = 0.0;
        state.tax_commercial = 7.5;
        let workplaces = vec![workplace(COMMERCIAL, 1, 10, 10)];

        state.update_monthly_finances(&workplaces, &[]);

        assert_eq!(state.money, 75);
    }

    #[test]
    fn deduplicates_multi_tile_workplaces_and_ignores_invalid_kinds() {
        let workplaces = vec![
            workplace(COMMERCIAL, 10, 10, 3),
            workplace(COMMERCIAL, 10, 10, 5),
            workplace(OFFICE, 10, 10, 7),
            workplace(INDUSTRIAL, 11, 10, -2),
            workplace(FARM, -1, 10, 9),
            workplace(PUBLIC, 12, 10, 8),
            workplace(999, 13, 10, 6),
        ];

        let counts = super::count_business_workers(&workplaces);

        assert_eq!(counts.commercial, 5);
        assert_eq!(counts.office, 7);
        assert_eq!(counts.industrial, 0);
        assert_eq!(counts.farm, 0);
    }

    #[test]
    fn monthly_tax_uses_workers_after_employment_update() {
        let mut state = state_at(2024, 4, 30, 23, 59);
        state.money = 100;
        state.tax_residential = 0.0;
        state.tax_commercial = 7.0;
        state.tax_office = 0.0;
        state.tax_industrial = 0.0;
        state.tax_farm = 0.0;
        state.budget_police = 0;
        state.budget_fire = 0;
        state.budget_post = 0;
        state.budget_education = 0;
        let mut home = residential_tile(1, 1);
        home.work_place_kinds = vec![COMMERCIAL];
        home.work_place_serial_numbers = vec![42];
        let mut homes = vec![home];
        let mut workplaces = vec![
            workplace(COMMERCIAL, 42, 1, 0),
            workplace(COMMERCIAL, 42, 1, 0),
        ];
        let mut random = FixedRandom::new([]);

        state.update_world_with_source(
            1,
            &mut homes,
            &mut workplaces,
            &mut [],
            &[],
            &[],
            &mut random,
        );

        assert_eq!(workplaces[0].workers, 1);
        assert_eq!(workplaces[1].workers, 1);
        assert_eq!(state.money, 107);
    }

    #[test]
    fn residential_tax_uses_population_and_fractional_percent_rate() {
        let mut state = state_at(2024, 1, 1, 0, 0);
        state.money = 100;
        state.population = 100;
        state.tax_residential = 7.5;
        state.tax_commercial = 0.0;
        state.tax_office = 0.0;
        state.tax_industrial = 0.0;
        state.tax_farm = 0.0;
        state.budget_police = 0;
        state.budget_fire = 0;
        state.budget_post = 0;
        state.budget_education = 0;

        state.update_monthly_finances(&[], &[]);

        assert_eq!(state.money, 850);
    }

    #[test]
    fn residential_object_count_does_not_change_residential_tax() {
        let mut without_homes = state_at(2024, 1, 1, 0, 0);
        without_homes.money = 0;
        without_homes.population = 10;
        without_homes.tax_residential = 7.0;
        without_homes.update_monthly_finances(&[], &[]);

        let mut with_multi_tile_home = state_at(2024, 1, 1, 0, 0);
        with_multi_tile_home.money = 0;
        with_multi_tile_home.population = 10;
        with_multi_tile_home.tax_residential = 7.0;
        with_multi_tile_home.update_monthly_finances(
            &[],
            &[
                ffi::SimulationObjectState {
                    object_id: 42,
                    category_ids: vec![19],
                },
                ffi::SimulationObjectState {
                    object_id: 42,
                    category_ids: vec![19],
                },
            ],
        );

        assert_eq!(without_homes.money, 70);
        assert_eq!(with_multi_tile_home.money, 70);
    }

    #[test]
    fn empty_homes_and_negative_population_produce_no_residential_tax() {
        let residential_objects = simulation_objects(&[(19, 10)]);
        let mut empty_city = state_at(2024, 1, 1, 0, 0);
        empty_city.money = 100;
        empty_city.population = 0;
        empty_city.tax_residential = 7.0;

        empty_city.update_monthly_finances(&[], &residential_objects);

        assert_eq!(empty_city.money, 100);

        empty_city.population = -10;
        empty_city.update_monthly_finances(&[], &residential_objects);

        assert_eq!(empty_city.money, 100);
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
