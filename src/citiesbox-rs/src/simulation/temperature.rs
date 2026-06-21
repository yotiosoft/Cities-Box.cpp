use super::{SimulationState, random::SimulationRandomSource};

impl SimulationState {
    pub(super) fn update_temperature<S: SimulationRandomSource>(&mut self, random: &mut S) {
        if !random.should_change_temperature() {
            return;
        }

        let direction_roll = random.temperature_direction_roll();
        let daytime = (8..=14).contains(&self.time.hour);
        let increase = if daytime {
            direction_roll <= 5
        } else {
            direction_roll <= 1
        };
        let (minimum, maximum) = seasonal_temperature_range(self.time.month);

        if increase {
            if self.temperature < maximum {
                self.temperature += 1;
            }
        } else if self.temperature > minimum {
            self.temperature -= 1;
        }
    }
}

fn seasonal_temperature_range(month: i32) -> (i32, i32) {
    match month {
        12 | 1 | 2 => (-10, 15),
        3..=5 => (10, 25),
        6..=8 => (20, 35),
        9..=11 => (10, 25),
        _ => (10, 25),
    }
}

#[cfg(test)]
mod tests {
    use crate::simulation::{
        empty_map_stats,
        test_support::{FixedRandom, state_at},
    };

    fn changing_random(direction: i32) -> FixedRandom {
        let mut random = FixedRandom::new([]);
        random.temperature_change = true;
        random.temperature_direction = direction;
        random
    }

    #[test]
    fn keeps_temperature_when_the_change_check_fails() {
        let mut state = state_at(2024, 1, 1, 12, 0);
        state.temperature = 10;
        let mut random = FixedRandom::new([]);

        state.update_temperature(&mut random);

        assert_eq!(state.temperature, 10);
    }

    #[test]
    fn daytime_and_nighttime_use_the_hsp_direction_weights() {
        let mut daytime_increase = state_at(2024, 4, 1, 8, 0);
        daytime_increase.temperature = 20;
        daytime_increase.update_temperature(&mut changing_random(5));
        assert_eq!(daytime_increase.temperature, 21);

        let mut daytime_decrease = state_at(2024, 4, 1, 14, 0);
        daytime_decrease.temperature = 20;
        daytime_decrease.update_temperature(&mut changing_random(6));
        assert_eq!(daytime_decrease.temperature, 19);

        let mut nighttime_increase = state_at(2024, 4, 1, 7, 0);
        nighttime_increase.temperature = 20;
        nighttime_increase.update_temperature(&mut changing_random(1));
        assert_eq!(nighttime_increase.temperature, 21);

        let mut nighttime_decrease = state_at(2024, 4, 1, 15, 0);
        nighttime_decrease.temperature = 20;
        nighttime_decrease.update_temperature(&mut changing_random(2));
        assert_eq!(nighttime_decrease.temperature, 19);
    }

    #[test]
    fn respects_each_seasons_temperature_limits() {
        for (month, minimum, maximum) in [(1, -10, 15), (4, 10, 25), (7, 20, 35), (10, 10, 25)] {
            let mut state = state_at(2024, month, 1, 12, 0);
            state.temperature = maximum;
            state.update_temperature(&mut changing_random(0));
            assert_eq!(state.temperature, maximum);

            state.temperature = minimum;
            state.update_temperature(&mut changing_random(7));
            assert_eq!(state.temperature, minimum);
        }
    }

    #[test]
    fn updates_at_most_once_per_positive_world_update_using_the_starting_time() {
        let mut state = state_at(2024, 4, 1, 7, 0);
        state.temperature = 20;
        let mut random = changing_random(2);

        state.update_world_with_source(
            60,
            &mut [],
            &mut [],
            &mut [],
            &[],
            &empty_map_stats(),
            &mut random,
        );

        assert_eq!((state.time.hour, state.time.minutes), (8, 0));
        assert_eq!(state.temperature, 19);
    }

    #[test]
    fn does_not_update_temperature_without_positive_elapsed_time() {
        let mut state = state_at(2024, 4, 1, 12, 0);
        state.temperature = 20;
        let mut random = changing_random(0);

        state.update_world_with_source(
            0,
            &mut [],
            &mut [],
            &mut [],
            &[],
            &empty_map_stats(),
            &mut random,
        );
        state.update_world_with_source(
            -1,
            &mut [],
            &mut [],
            &mut [],
            &[],
            &empty_map_stats(),
            &mut random,
        );

        assert_eq!(state.temperature, 20);
    }
}
