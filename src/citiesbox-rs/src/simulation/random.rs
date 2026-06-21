use rand::Rng;

pub(super) trait SimulationRandomSource {
    fn next_demand_change(&mut self) -> f64;
    fn random_below(&mut self, upper_exclusive: i32) -> i32;
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
}
