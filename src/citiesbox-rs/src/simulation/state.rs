use super::random::SimulationRandomSource;
use crate::citymap::ffi;

pub(crate) struct SimulationState {
    pub(crate) time: ffi::TimeStruct,
    pub(crate) population: i32,
    pub(crate) money: i32,
    pub(crate) temperature: i32,
    pub(crate) demand: ffi::RCOIFstruct,
    pub(crate) budget_police: i32,
    pub(crate) budget_fire: i32,
    pub(crate) budget_post: i32,
    pub(crate) budget_education: i32,
    pub(crate) tax_residential: f64,
    pub(crate) tax_commercial: f64,
    pub(crate) tax_office: f64,
    pub(crate) tax_industrial: f64,
    pub(crate) tax_farm: f64,
}

impl SimulationState {
    pub(crate) fn new() -> Self {
        Self {
            time: ffi::TimeStruct {
                year: 2024,
                month: 1,
                date: 1,
                hour: 0,
                minutes: 0,
            },
            population: 0,
            money: 100_000,
            temperature: 20,
            demand: ffi::RCOIFstruct {
                residential: 0.0,
                commercial: 0.0,
                office: 0.0,
                industrial: 0.0,
                farm: 0.0,
            },
            budget_police: 100,
            budget_fire: 100,
            budget_post: 100,
            budget_education: 100,
            tax_residential: 7.0,
            tax_commercial: 7.0,
            tax_office: 7.0,
            tax_industrial: 7.0,
            tax_farm: 7.0,
        }
    }

    pub(super) fn snapshot(&self) -> ffi::SimulationSnapshot {
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

    pub(super) fn set_finance_settings(
        &mut self,
        budget: ffi::BudgetSettings,
        tax: ffi::TaxSettings,
    ) {
        self.budget_police = normalize_budget(budget.police);
        self.budget_fire = normalize_budget(budget.fire);
        self.budget_post = normalize_budget(budget.post);
        self.budget_education = normalize_budget(budget.education);
        self.tax_residential = normalize_tax(tax.residential);
        self.tax_commercial = normalize_tax(tax.commercial);
        self.tax_office = normalize_tax(tax.office);
        self.tax_industrial = normalize_tax(tax.industrial);
        self.tax_farm = normalize_tax(tax.farm);
    }

    #[cfg(test)]
    #[allow(clippy::too_many_arguments)]
    pub(super) fn update_world_with_source<S: SimulationRandomSource>(
        &mut self,
        minutes_delta: i32,
        residential_tiles: &mut [ffi::ResidentialTileState],
        work_place_tiles: &mut [ffi::WorkPlaceTileState],
        school_tiles: &mut [ffi::SchoolTileState],
        demand_tiles: &[ffi::DemandTileState],
        simulation_objects: &[ffi::SimulationObjectState],
        random: &mut S,
    ) -> u32 {
        self.update_world_with_demand_inputs(
            minutes_delta,
            residential_tiles,
            work_place_tiles,
            school_tiles,
            demand_tiles,
            &ffi::SpecialDemandState {
                residential: 0,
                commercial: 0,
                office: 0,
                industrial: 0,
                farm: 0,
            },
            simulation_objects,
            random,
        )
    }

    #[allow(clippy::too_many_arguments)]
    pub(super) fn update_world_with_demand_inputs<S: SimulationRandomSource>(
        &mut self,
        minutes_delta: i32,
        residential_tiles: &mut [ffi::ResidentialTileState],
        work_place_tiles: &mut [ffi::WorkPlaceTileState],
        school_tiles: &mut [ffi::SchoolTileState],
        demand_tiles: &[ffi::DemandTileState],
        special_demand: &ffi::SpecialDemandState,
        simulation_objects: &[ffi::SimulationObjectState],
        random: &mut S,
    ) -> u32 {
        if minutes_delta > 0 {
            self.update_temperature(random);
        }
        let elapsed_days = self.time.advance_clock(minutes_delta);
        for _ in 0..elapsed_days {
            self.time.advance_one_day();
            self.update_daily_population(residential_tiles, random);
            self.update_daily_employment(residential_tiles, work_place_tiles, school_tiles, random);
            if self.time.date == 1 {
                self.update_monthly_finances(work_place_tiles, simulation_objects);
            }
            self.update_daily_demand(demand_tiles, work_place_tiles, special_demand, random);
        }
        elapsed_days
    }
}

pub(crate) fn normalize_budget(value: i32) -> i32 {
    value.clamp(0, 200)
}

pub(crate) fn normalize_tax(value: f64) -> f64 {
    if value.is_finite() {
        value.clamp(0.0, 100.0)
    } else {
        0.0
    }
}

pub(super) fn tax_attractiveness(value: f64) -> f64 {
    let excess = (normalize_tax(value) - 10.0).max(0.0);
    100.0 / (100.0 + excess * 5.0)
}

pub(super) fn scale_by_tax(value: i32, tax: f64) -> i32 {
    (f64::from(value.max(0)) * tax_attractiveness(tax)).floor() as i32
}

#[cfg(test)]
mod tests {
    use super::{normalize_tax, tax_attractiveness};
    use crate::{
        citymap::ffi,
        simulation::test_support::{FixedRandom, advance, state_at},
    };

    fn special_commercial(value: i32) -> ffi::SpecialDemandState {
        ffi::SpecialDemandState {
            residential: 0,
            commercial: value,
            office: 0,
            industrial: 0,
            farm: 0,
        }
    }

    fn commercial_workplace() -> ffi::WorkPlaceTileState {
        ffi::WorkPlaceTileState {
            x: 0,
            y: 0,
            kind: 1,
            serial_number: 1,
            maximum_capacity: 1,
            workers: 0,
        }
    }

    #[test]
    fn normalizes_tax_and_applies_penalty_only_above_ten_percent() {
        assert_eq!(normalize_tax(-1.0), 0.0);
        assert_eq!(normalize_tax(101.0), 100.0);
        assert_eq!(normalize_tax(f64::NAN), 0.0);
        assert_eq!(normalize_tax(f64::INFINITY), 0.0);
        assert_eq!(tax_attractiveness(0.0), 1.0);
        assert_eq!(tax_attractiveness(10.0), 1.0);
        assert!((tax_attractiveness(15.0) - 0.8).abs() < f64::EPSILON);
        assert!((tax_attractiveness(30.0) - 0.5).abs() < f64::EPSILON);
        assert!((tax_attractiveness(100.0) - (100.0 / 550.0)).abs() < f64::EPSILON);
    }

    #[test]
    fn runs_daily_processing_for_every_elapsed_day() {
        let mut state = state_at(2024, 2, 27, 12, 0);
        assert_eq!(advance(&mut state, 4 * 24 * 60 + 90), 4);
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
        assert_eq!(advance(&mut state, 12 * 60), 0);
    }

    #[test]
    fn demand_does_not_update_without_a_date_change() {
        let mut state = state_at(2024, 6, 1, 10, 0);
        state.demand.commercial = 40.0;
        let mut workplaces = [commercial_workplace()];
        let mut random = FixedRandom::new([]);

        let days = state.update_world_with_demand_inputs(
            12 * 60,
            &mut [],
            &mut workplaces,
            &mut [],
            &[],
            &special_commercial(5),
            &[],
            &mut random,
        );

        assert_eq!(days, 0);
        assert_eq!(state.demand.commercial, 40.0);
        assert!(random.upper_bounds.is_empty());
    }

    #[test]
    fn demand_updates_once_for_each_elapsed_day() {
        let mut state = state_at(2024, 6, 1, 10, 0);
        state.demand.commercial = 40.0;
        let mut workplaces = [commercial_workplace()];
        // Cは各日、乱数10（増減0）+ special 5。
        let mut random = FixedRandom::new([10, 10]);

        let days = state.update_world_with_demand_inputs(
            2 * 24 * 60,
            &mut [],
            &mut workplaces,
            &mut [],
            &[],
            &special_commercial(5),
            &[],
            &mut random,
        );

        assert_eq!(days, 2);
        assert_eq!(state.demand.commercial, 50.0);
        assert_eq!(random.upper_bounds, [30, 30]);
    }
}
