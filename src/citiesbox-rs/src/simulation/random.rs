use rand::Rng;

pub(super) trait SimulationRandomSource {
    fn next_demand_change(&mut self) -> f64;
    fn random_below(&mut self, upper_exclusive: i32) -> i32;
    fn should_change_temperature(&mut self) -> bool;
    fn temperature_direction_roll(&mut self) -> i32;
}

pub(super) struct RandomSimulationSource {
    rng: rand::rngs::ThreadRng,
}

impl RandomSimulationSource {
    pub(super) fn new() -> Self {
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

    fn should_change_temperature(&mut self) -> bool {
        self.rng.random_range(0..20) == 0
    }

    fn temperature_direction_roll(&mut self) -> i32 {
        self.rng.random_range(0..8)
    }
}
