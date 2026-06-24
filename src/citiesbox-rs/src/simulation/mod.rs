mod demand;
mod employment;
mod finance;
mod population;
mod random;
mod state;
mod temperature;
mod time;

#[cfg(test)]
mod test_support;

use crate::citymap::{RustCityMap, ffi};
use random::RandomSimulationSource;
pub(crate) use state::SimulationState;

impl RustCityMap {
    pub(crate) fn simulation_snapshot(&self) -> ffi::SimulationSnapshot {
        self.simulation.snapshot()
    }

    pub(crate) fn charge_construction_cost(&mut self) {
        self.simulation.charge_construction_cost();
    }

    pub(crate) fn will_run_daily_update(&self, minutes_delta: i32) -> bool {
        let mut time = self.simulation.time.clone();
        time.advance_clock(minutes_delta) > 0
    }

    // C++が所有する現在の住宅タイルを値として受け取り、更新結果を返す。
    // 保存用のRustマップ写像は参照しないため、古いマップ状態では計算しない。
    pub(crate) fn update_world(
        &mut self,
        minutes_delta: i32,
        mut residential_tiles: Vec<ffi::ResidentialTileState>,
        mut work_place_tiles: Vec<ffi::WorkPlaceTileState>,
        mut school_tiles: Vec<ffi::SchoolTileState>,
        demand_tiles: Vec<ffi::DemandTileState>,
        map_stats: ffi::SimulationMapStats,
    ) -> ffi::SimulationUpdate {
        let mut random = RandomSimulationSource::new();
        self.simulation.update_world_with_source(
            minutes_delta,
            &mut residential_tiles,
            &mut work_place_tiles,
            &mut school_tiles,
            &demand_tiles,
            &map_stats,
            &mut random,
        );
        ffi::SimulationUpdate {
            snapshot: self.simulation.snapshot(),
            residential_tiles,
            work_place_tiles,
            school_tiles,
        }
    }
}

#[cfg(test)]
pub(crate) fn empty_map_stats() -> ffi::SimulationMapStats {
    ffi::SimulationMapStats {
        residential_tiles: 0,
        commercial_tiles: 0,
        office_tiles: 0,
        industrial_tiles: 0,
        farm_tiles: 0,
        police_stations: 0,
        fire_departments: 0,
        post_offices: 0,
        education_facilities: 0,
    }
}
