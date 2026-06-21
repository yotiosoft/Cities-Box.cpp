use super::{SimulationState, random::SimulationRandomSource};

impl SimulationState {
    pub(super) fn update_daily_demand<S: SimulationRandomSource>(&mut self, random: &mut S) {
        self.demand.residential =
            (self.demand.residential + random.next_demand_change()).clamp(0.0, 100.0);
        self.demand.commercial =
            (self.demand.commercial + random.next_demand_change()).clamp(0.0, 100.0);
        self.demand.office = (self.demand.office + random.next_demand_change()).clamp(0.0, 100.0);
        self.demand.industrial =
            (self.demand.industrial + random.next_demand_change()).clamp(0.0, 100.0);
        self.demand.farm = (self.demand.farm + random.next_demand_change()).clamp(0.0, 100.0);
    }
}

#[cfg(test)]
mod tests {
    use crate::simulation::{
        empty_map_stats,
        test_support::{FixedRandom, state_at},
    };

    #[test]
    fn clamps_demand_to_its_allowed_range() {
        let mut high = state_at(2024, 1, 1, 0, 0);
        high.demand.residential = 99.0;
        high.demand.commercial = 99.0;
        high.demand.office = 99.0;
        high.demand.industrial = 99.0;
        high.demand.farm = 99.0;
        let mut increase = FixedRandom::new([]);
        increase.demand_change = 4.9;
        high.update_world_with_source(
            24 * 60,
            &mut [],
            &mut [],
            &mut [],
            &empty_map_stats(),
            &mut increase,
        );
        assert_eq!(high.demand.residential, 100.0);
        assert_eq!(high.demand.commercial, 100.0);
        assert_eq!(high.demand.office, 100.0);
        assert_eq!(high.demand.industrial, 100.0);
        assert_eq!(high.demand.farm, 100.0);

        let mut decrease = FixedRandom::new([]);
        decrease.demand_change = -4.9;
        high.demand.residential = 1.0;
        high.demand.commercial = 1.0;
        high.demand.office = 1.0;
        high.demand.industrial = 1.0;
        high.demand.farm = 1.0;
        high.update_world_with_source(
            24 * 60,
            &mut [],
            &mut [],
            &mut [],
            &empty_map_stats(),
            &mut decrease,
        );
        assert_eq!(high.demand.residential, 0.0);
        assert_eq!(high.demand.commercial, 0.0);
        assert_eq!(high.demand.office, 0.0);
        assert_eq!(high.demand.industrial, 0.0);
        assert_eq!(high.demand.farm, 0.0);
    }
}
