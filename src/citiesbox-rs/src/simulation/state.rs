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

    pub(super) fn update_world_with_source<S: SimulationRandomSource>(
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
}

#[cfg(test)]
mod tests {
    use crate::simulation::test_support::{advance, state_at};

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
}
