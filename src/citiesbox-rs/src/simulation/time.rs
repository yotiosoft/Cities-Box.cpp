use crate::citymap::ffi;

const MINUTES_PER_HOUR: i64 = 60;
const HOURS_PER_DAY: i64 = 24;

impl ffi::TimeStruct {
    pub(super) fn advance_clock(&mut self, minutes_delta: i32) -> u32 {
        if minutes_delta <= 0 {
            return 0;
        }

        let total_minutes = i64::from(self.minutes) + i64::from(minutes_delta);
        self.minutes = (total_minutes % MINUTES_PER_HOUR) as i32;

        let total_hours = i64::from(self.hour) + total_minutes / MINUTES_PER_HOUR;
        self.hour = (total_hours % HOURS_PER_DAY) as i32;
        (total_hours / HOURS_PER_DAY) as u32
    }

    pub(super) fn advance_one_day(&mut self) {
        self.date += 1;
        if self.date > days_in_month(self.year, self.month) {
            self.date = 1;
            self.month += 1;
            if self.month > 12 {
                self.month = 1;
                self.year += 1;
            }
        }
    }
}

fn is_leap_year(year: i32) -> bool {
    year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)
}

fn days_in_month(year: i32, month: i32) -> i32 {
    match month {
        4 | 6 | 9 | 11 => 30,
        2 if is_leap_year(year) => 29,
        2 => 28,
        _ => 31,
    }
}

#[cfg(test)]
mod tests {
    use crate::simulation::test_support::{advance, state_at};

    #[test]
    fn advances_sixty_minutes() {
        let mut state = state_at(2024, 1, 1, 10, 15);
        assert_eq!(advance(&mut state, 60), (0, 0));
        assert_eq!((state.time.hour, state.time.minutes), (11, 15));
    }

    #[test]
    fn advances_twenty_four_hours() {
        let mut state = state_at(2024, 1, 1, 10, 15);
        assert_eq!(advance(&mut state, 24 * 60), (1, 5));
        assert_eq!(
            (state.time.year, state.time.month, state.time.date),
            (2024, 1, 2)
        );
        assert_eq!((state.time.hour, state.time.minutes), (10, 15));
    }

    #[test]
    fn advances_across_month_and_year_ends() {
        let mut month_end = state_at(2024, 4, 30, 23, 30);
        advance(&mut month_end, 60);
        assert_eq!(
            (
                month_end.time.year,
                month_end.time.month,
                month_end.time.date,
                month_end.time.hour,
                month_end.time.minutes
            ),
            (2024, 5, 1, 0, 30)
        );

        let mut year_end = state_at(2024, 12, 31, 23, 59);
        advance(&mut year_end, 1);
        assert_eq!(
            (
                year_end.time.year,
                year_end.time.month,
                year_end.time.date,
                year_end.time.hour,
                year_end.time.minutes
            ),
            (2025, 1, 1, 0, 0)
        );
    }

    #[test]
    fn handles_common_and_leap_year_february() {
        let mut common = state_at(2023, 2, 28, 23, 59);
        advance(&mut common, 1);
        assert_eq!((common.time.month, common.time.date), (3, 1));

        let mut leap = state_at(2024, 2, 28, 23, 59);
        advance(&mut leap, 1);
        assert_eq!((leap.time.month, leap.time.date), (2, 29));
        advance(&mut leap, 24 * 60);
        assert_eq!((leap.time.month, leap.time.date), (3, 1));

        let mut century_common = state_at(2100, 2, 28, 23, 59);
        advance(&mut century_common, 1);
        assert_eq!(
            (century_common.time.month, century_common.time.date),
            (3, 1)
        );

        let mut century_leap = state_at(2000, 2, 28, 23, 59);
        advance(&mut century_leap, 1);
        assert_eq!((century_leap.time.month, century_leap.time.date), (2, 29));
    }

    #[test]
    fn ignores_negative_minutes() {
        let mut state = state_at(2024, 1, 1, 10, 15);
        assert_eq!(advance(&mut state, -1), (0, 0));
        assert_eq!((state.time.hour, state.time.minutes), (10, 15));
    }
}
