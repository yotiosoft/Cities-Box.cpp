use super::{SimulationState, empty_map_stats, random::SimulationRandomSource};
use crate::citymap::ffi;
use std::collections::VecDeque;

pub(super) struct FixedRandom {
    pub(super) demand_change: f64,
    pub(super) demand_calls: usize,
    integers: VecDeque<i32>,
    pub(super) upper_bounds: Vec<i32>,
    pub(super) temperature_change: bool,
    pub(super) temperature_direction: i32,
}

impl FixedRandom {
    pub(super) fn new(integers: impl IntoIterator<Item = i32>) -> Self {
        Self {
            demand_change: 0.0,
            demand_calls: 0,
            integers: integers.into_iter().collect(),
            upper_bounds: Vec::new(),
            temperature_change: false,
            temperature_direction: 0,
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

    fn should_change_temperature(&mut self) -> bool {
        self.temperature_change
    }

    fn temperature_direction_roll(&mut self) -> i32 {
        self.temperature_direction
    }
}

pub(super) fn state_at(
    year: i32,
    month: i32,
    date: i32,
    hour: i32,
    minutes: i32,
) -> SimulationState {
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

pub(super) fn residential_tile(residents: i32, maximum_capacity: i32) -> ffi::ResidentialTileState {
    ffi::ResidentialTileState {
        x: 1,
        y: 2,
        residents,
        maximum_capacity,
        ages: (0..residents).map(|age| age + 20).collect(),
        genders: (0..residents).map(|_| "m".to_string()).collect(),
        work_place_kinds: Vec::new(),
        work_place_serial_numbers: Vec::new(),
        school_kinds: Vec::new(),
        school_serial_numbers: Vec::new(),
    }
}

pub(super) fn advance(state: &mut SimulationState, minutes: i32) -> (u32, usize) {
    let mut random = FixedRandom::new([]);
    let days = state.update_world_with_source(
        minutes,
        &mut [],
        &mut [],
        &mut [],
        &empty_map_stats(),
        &mut random,
    );
    (days, random.demand_calls)
}
