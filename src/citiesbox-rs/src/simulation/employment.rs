use super::{SimulationState, random::SimulationRandomSource};
use crate::citymap::ffi;
use std::collections::{HashMap, HashSet};

const COMMERCIAL: i32 = 1;
const OFFICE: i32 = 2;
const INDUSTRIAL: i32 = 3;
const FARM: i32 = 4;
const PUBLIC: i32 = 5;

const ELEMENTARY_SCHOOL: i32 = 0;
const JUNIOR_HIGH_SCHOOL: i32 = 1;
const HIGH_SCHOOL: i32 = 2;
const UNIVERSITY: i32 = 3;

#[derive(Clone, Copy, Debug, Eq, Hash, PartialEq)]
struct FacilityKey {
    kind: i32,
    serial_number: i32,
}

struct Facilities {
    capacities: HashMap<FacilityKey, i32>,
    candidates: Vec<FacilityKey>,
    counts: HashMap<FacilityKey, i32>,
}

impl Facilities {
    fn workplaces(tiles: &[ffi::WorkPlaceTileState]) -> Self {
        Self::new(tiles.iter().map(|tile| {
            (
                FacilityKey {
                    kind: tile.kind,
                    serial_number: tile.serial_number,
                },
                tile.maximum_capacity,
            )
        }))
    }

    fn schools(tiles: &[ffi::SchoolTileState]) -> Self {
        Self::new(tiles.iter().map(|tile| {
            (
                FacilityKey {
                    kind: tile.kind,
                    serial_number: tile.serial_number,
                },
                tile.maximum_capacity,
            )
        }))
    }

    fn new(entries: impl Iterator<Item = (FacilityKey, i32)>) -> Self {
        let mut capacities = HashMap::new();
        let mut candidates = Vec::new();
        let mut seen = HashSet::new();
        for (key, capacity) in entries {
            capacities
                .entry(key)
                .and_modify(|current: &mut i32| *current = (*current).max(capacity.max(0)))
                .or_insert(capacity.max(0));
            if seen.insert(key) {
                candidates.push(key);
            }
        }
        Self {
            capacities,
            candidates,
            counts: HashMap::new(),
        }
    }

    fn try_reserve(&mut self, key: FacilityKey) -> bool {
        let Some(capacity) = self.capacities.get(&key).copied() else {
            return false;
        };
        let count = self.counts.entry(key).or_default();
        if *count >= capacity {
            return false;
        }
        *count += 1;
        true
    }

    fn random_candidate<S: SimulationRandomSource>(
        &self,
        kind: Option<i32>,
        random: &mut S,
    ) -> Option<FacilityKey> {
        let candidates = self
            .candidates
            .iter()
            .copied()
            .filter(|candidate| kind.is_none_or(|kind| candidate.kind == kind))
            .collect::<Vec<_>>();
        if candidates.is_empty() {
            None
        } else {
            Some(candidates[random.random_below(candidates.len() as i32) as usize])
        }
    }

    fn count(&self, key: FacilityKey) -> i32 {
        self.counts.get(&key).copied().unwrap_or_default()
    }
}

impl SimulationState {
    pub(super) fn update_daily_employment<S: SimulationRandomSource>(
        &mut self,
        residential_tiles: &mut [ffi::ResidentialTileState],
        work_place_tiles: &mut [ffi::WorkPlaceTileState],
        school_tiles: &mut [ffi::SchoolTileState],
        random: &mut S,
    ) {
        let mut workplaces = Facilities::workplaces(work_place_tiles);
        let mut schools = Facilities::schools(school_tiles);

        for tile in residential_tiles {
            let known_residents = tile.residents.max(0) as usize;
            let known_residents = known_residents.min(tile.ages.len());
            let mut resident_workplaces = vec![None; known_residents];
            let mut resident_schools = vec![None; known_residents];

            // セーブ形式は家庭内の割当一覧なので、学校を先に年齢へ再対応付けする。
            // これにより就職と就学が同じ住民へ重複することを防ぐ。
            for key in assignment_keys(&tile.school_kinds, &tile.school_serial_numbers) {
                let Some(index) = (0..known_residents).find(|&index| {
                    resident_schools[index].is_none()
                        && school_kind_for_age(tile.ages[index]) == Some(key.kind)
                }) else {
                    continue;
                };
                if schools.try_reserve(key) {
                    resident_schools[index] = Some(key);
                }
            }

            for key in assignment_keys(&tile.work_place_kinds, &tile.work_place_serial_numbers) {
                let Some(index) = (0..known_residents).find(|&index| {
                    resident_workplaces[index].is_none()
                        && resident_schools[index].is_none()
                        && is_working_age(tile.ages[index])
                }) else {
                    continue;
                };
                if workplaces.try_reserve(key) {
                    resident_workplaces[index] = Some(key);
                }
            }

            for index in 0..known_residents {
                if resident_workplaces[index].is_none() && resident_schools[index].is_none() {
                    let age = tile.ages[index];
                    if is_working_age(age)
                        && random.random_below((age / 10).max(1)) == 0
                        && let Some(workplace) = workplaces.random_candidate(None, random)
                        && workplaces.try_reserve(workplace)
                    {
                        resident_workplaces[index] = Some(workplace);
                        self.decrease_demand_for_new_worker(workplace.kind);
                    }
                }

                if resident_workplaces[index].is_none() && resident_schools[index].is_none() {
                    let age = tile.ages[index];
                    let Some(school_kind) = school_kind_for_age(age) else {
                        continue;
                    };
                    let attends = match school_kind {
                        ELEMENTARY_SCHOOL | JUNIOR_HIGH_SCHOOL => true,
                        HIGH_SCHOOL => random.random_below(3) == 0,
                        UNIVERSITY => random.random_below(age.max(1)) == 0,
                        _ => false,
                    };
                    if attends
                        && let Some(school) = schools.random_candidate(Some(school_kind), random)
                        && schools.try_reserve(school)
                    {
                        resident_schools[index] = Some(school);
                    }
                }
            }

            write_assignments(
                &resident_workplaces,
                &mut tile.work_place_kinds,
                &mut tile.work_place_serial_numbers,
            );
            write_assignments(
                &resident_schools,
                &mut tile.school_kinds,
                &mut tile.school_serial_numbers,
            );
        }

        for tile in work_place_tiles {
            tile.workers = workplaces.count(FacilityKey {
                kind: tile.kind,
                serial_number: tile.serial_number,
            });
        }
        for tile in school_tiles {
            tile.students = schools.count(FacilityKey {
                kind: tile.kind,
                serial_number: tile.serial_number,
            });
        }
    }

    fn decrease_demand_for_new_worker(&mut self, kind: i32) {
        match kind {
            COMMERCIAL => self.demand.commercial -= 1.0,
            OFFICE => self.demand.office -= 1.0,
            INDUSTRIAL => self.demand.industrial -= 1.0,
            FARM => self.demand.farm -= 1.0,
            PUBLIC => {}
            _ => {}
        }
    }
}

fn assignment_keys<'a>(
    kinds: &'a [i32],
    serial_numbers: &'a [i32],
) -> impl Iterator<Item = FacilityKey> + 'a {
    kinds
        .iter()
        .copied()
        .zip(serial_numbers.iter().copied())
        .map(|(kind, serial_number)| FacilityKey {
            kind,
            serial_number,
        })
}

fn write_assignments(
    assignments: &[Option<FacilityKey>],
    kinds: &mut Vec<i32>,
    serial_numbers: &mut Vec<i32>,
) {
    kinds.clear();
    serial_numbers.clear();
    for assignment in assignments.iter().flatten() {
        kinds.push(assignment.kind);
        serial_numbers.push(assignment.serial_number);
    }
}

fn is_working_age(age: i32) -> bool {
    (18..=60).contains(&age)
}

fn school_kind_for_age(age: i32) -> Option<i32> {
    match age {
        6..=12 => Some(ELEMENTARY_SCHOOL),
        13..=15 => Some(JUNIOR_HIGH_SCHOOL),
        16..=18 => Some(HIGH_SCHOOL),
        19..=50 => Some(UNIVERSITY),
        _ => None,
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::simulation::test_support::{FixedRandom, residential_tile, state_at};

    fn workplace(kind: i32, serial_number: i32, capacity: i32) -> ffi::WorkPlaceTileState {
        ffi::WorkPlaceTileState {
            x: 3,
            y: 4,
            kind,
            serial_number,
            maximum_capacity: capacity,
            workers: 99,
        }
    }

    fn school(kind: i32, serial_number: i32, capacity: i32) -> ffi::SchoolTileState {
        ffi::SchoolTileState {
            x: 5,
            y: 6,
            kind,
            serial_number,
            maximum_capacity: capacity,
            students: 99,
        }
    }

    #[test]
    fn removes_missing_ineligible_and_over_capacity_assignments() {
        let mut state = state_at(2024, 1, 2, 0, 0);
        let mut home = residential_tile(4, 4);
        home.ages = vec![10, 20, 30, 70];
        home.work_place_kinds = vec![COMMERCIAL, COMMERCIAL, OFFICE];
        home.work_place_serial_numbers = vec![10, 10, 999];
        home.school_kinds = vec![ELEMENTARY_SCHOOL, UNIVERSITY];
        home.school_serial_numbers = vec![20, 20];
        let mut homes = vec![home];
        let mut workplaces = vec![workplace(COMMERCIAL, 10, 1)];
        let mut schools = vec![school(ELEMENTARY_SCHOOL, 20, 1)];
        let mut random = FixedRandom::new([1, 2]);

        state.update_daily_employment(&mut homes, &mut workplaces, &mut schools, &mut random);

        assert_eq!(homes[0].work_place_kinds, [COMMERCIAL]);
        assert_eq!(homes[0].work_place_serial_numbers, [10]);
        assert_eq!(homes[0].school_kinds, [ELEMENTARY_SCHOOL]);
        assert_eq!(homes[0].school_serial_numbers, [20]);
        assert_eq!(workplaces[0].workers, 1);
        assert_eq!(schools[0].students, 1);
    }

    #[test]
    fn assigns_work_using_hsp_age_probability_and_updates_demand() {
        let mut state = state_at(2024, 1, 2, 0, 0);
        state.demand.commercial = 50.0;
        let mut home = residential_tile(1, 1);
        home.ages = vec![25];
        let mut homes = vec![home];
        let mut workplaces = vec![workplace(COMMERCIAL, 10, 1)];
        let mut random = FixedRandom::new([0, 0]);

        state.update_daily_employment(&mut homes, &mut workplaces, &mut [], &mut random);

        assert_eq!(homes[0].work_place_kinds, [COMMERCIAL]);
        assert_eq!(homes[0].work_place_serial_numbers, [10]);
        assert_eq!(workplaces[0].workers, 1);
        assert_eq!(state.demand.commercial, 49.0);
        assert_eq!(random.upper_bounds, [2, 1]);
    }

    #[test]
    fn assigns_each_school_age_band_without_overlapping_work() {
        let mut state = state_at(2024, 1, 2, 0, 0);
        let mut home = residential_tile(4, 4);
        home.ages = vec![10, 14, 17, 20];
        let mut homes = vec![home];
        let mut schools = vec![
            school(ELEMENTARY_SCHOOL, 10, 1),
            school(JUNIOR_HIGH_SCHOOL, 11, 1),
            school(HIGH_SCHOOL, 12, 1),
            school(UNIVERSITY, 13, 1),
        ];
        // 小中の施設選択、高校1/3と選択、20歳の就職判定失敗、大学1/20と選択。
        let mut random = FixedRandom::new([0, 0, 0, 0, 1, 0, 0]);

        state.update_daily_employment(&mut homes, &mut [], &mut schools, &mut random);

        assert!(homes[0].work_place_kinds.is_empty());
        assert_eq!(
            homes[0].school_kinds,
            [
                ELEMENTARY_SCHOOL,
                JUNIOR_HIGH_SCHOOL,
                HIGH_SCHOOL,
                UNIVERSITY
            ]
        );
        assert_eq!(homes[0].school_serial_numbers, [10, 11, 12, 13]);
        assert_eq!(
            schools
                .iter()
                .map(|school| school.students)
                .collect::<Vec<_>>(),
            [1, 1, 1, 1]
        );
    }

    #[test]
    fn mirrors_counts_to_every_tile_of_a_multi_tile_facility() {
        let mut state = state_at(2024, 1, 2, 0, 0);
        let mut home = residential_tile(1, 1);
        home.ages = vec![30];
        home.work_place_kinds = vec![OFFICE];
        home.work_place_serial_numbers = vec![42];
        let mut homes = vec![home];
        let mut workplaces = vec![workplace(OFFICE, 42, 10), workplace(OFFICE, 42, 10)];
        let mut random = FixedRandom::new([]);

        state.update_daily_employment(&mut homes, &mut workplaces, &mut [], &mut random);

        assert_eq!(workplaces[0].workers, 1);
        assert_eq!(workplaces[1].workers, 1);
    }
}
