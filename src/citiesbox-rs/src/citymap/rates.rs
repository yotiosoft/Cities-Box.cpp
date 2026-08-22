use super::{RustCityMap, ffi};

const LAND_PRICE: i32 = 1;
const CRIME_RATE: i32 = 2;
const EDUCATION_RATE: i32 = 3;
const NOISE_RATE: i32 = 4;
const HAPPINESS_RATE: i32 = 5;

impl RustCityMap {
    pub(super) fn update_rates(
        &mut self,
        width: i32,
        height: i32,
        mut tiles: Vec<ffi::TileRateState>,
        effects: Vec<ffi::RateEffect>,
    ) -> Vec<ffi::TileRateState> {
        apply_effects(width, height, &mut tiles, &effects);
        self.sync_rates(&tiles);
        tiles
    }

    fn sync_rates(&mut self, tiles: &[ffi::TileRateState]) {
        for state in tiles {
            let Some(tile) = self
                .tiles
                .get_mut(state.y as usize)
                .and_then(|row| row.get_mut(state.x as usize))
            else {
                continue;
            };
            tile.rate.clear();
            insert_nonzero(&mut tile.rate, "LandPrice", state.land_price);
            insert_nonzero(&mut tile.rate, "CrimeRate", state.crime_rate);
            insert_nonzero(&mut tile.rate, "EducationRate", state.education_rate);
            insert_nonzero(&mut tile.rate, "NoiseRate", state.noise_rate);
            insert_nonzero(&mut tile.rate, "HappinessRate", state.happiness_rate);
        }
    }
}

fn insert_nonzero(rates: &mut std::collections::HashMap<String, i32>, name: &str, value: i32) {
    if value != 0 {
        rates.insert(name.to_owned(), value);
    }
}

fn apply_effects(
    width: i32,
    height: i32,
    tiles: &mut [ffi::TileRateState],
    effects: &[ffi::RateEffect],
) {
    if width <= 0 || height <= 0 || tiles.len() != (i64::from(width) * i64::from(height)) as usize {
        return;
    }

    for effect in effects {
        if effect.range <= 0 || !(LAND_PRICE..=HAPPINESS_RATE).contains(&effect.rate_kind) {
            continue;
        }
        let effect_per_tile = effect.influence / effect.range;
        for relative_y in -effect.range..=effect.range {
            let y = effect.origin_y + relative_y;
            if !(0..height).contains(&y) {
                continue;
            }
            for relative_x in -effect.range..effect.range {
                let x = effect.origin_x + relative_x;
                if !(0..width).contains(&x) {
                    continue;
                }
                let distance_factor = (effect.range - 1 - relative_y)
                    .abs()
                    .max((effect.range - 1 - relative_x).abs());
                let mut delta = effect_per_tile.wrapping_mul(distance_factor);
                if effect.will_be_deleted {
                    delta = delta.wrapping_neg();
                }
                add_rate(
                    &mut tiles[(y * width + x) as usize],
                    effect.rate_kind,
                    delta,
                );
            }
        }
    }

    for tile in tiles {
        tile.crime_rate = tile.crime_rate.clamp(0, 100);
    }
}

fn add_rate(tile: &mut ffi::TileRateState, kind: i32, delta: i32) {
    if kind == CRIME_RATE {
        tile.crime_rate = tile.crime_rate.saturating_add(delta);
        return;
    }
    let rate = match kind {
        LAND_PRICE => &mut tile.land_price,
        EDUCATION_RATE => &mut tile.education_rate,
        NOISE_RATE => &mut tile.noise_rate,
        HAPPINESS_RATE => &mut tile.happiness_rate,
        _ => return,
    };
    *rate = rate.wrapping_add(delta);
}

#[cfg(test)]
mod tests {
    use super::*;

    fn tiles(width: i32, height: i32) -> Vec<ffi::TileRateState> {
        (0..height)
            .flat_map(|y| {
                (0..width).map(move |x| ffi::TileRateState {
                    x,
                    y,
                    land_price: 0,
                    crime_rate: 0,
                    education_rate: 0,
                    noise_rate: 0,
                    happiness_rate: 0,
                })
            })
            .collect()
    }

    fn effect(kind: i32, influence: i32, range: i32, deleted: bool) -> ffi::RateEffect {
        ffi::RateEffect {
            rate_kind: kind,
            influence,
            range,
            origin_x: 2,
            origin_y: 2,
            will_be_deleted: deleted,
        }
    }

    #[test]
    fn single_effect_updates_only_the_current_cpp_footprint() {
        let mut states = tiles(5, 5);
        apply_effects(5, 5, &mut states, &[effect(LAND_PRICE, 20, 2, false)]);
        assert_ne!(states[(2 * 5 + 2) as usize].land_price, 0);
        assert_ne!(states[(4 * 5 + 2) as usize].land_price, 0);
        assert_eq!(states[(2 * 5 + 4) as usize].land_price, 0);
    }

    #[test]
    fn multiple_crime_effects_add_and_clamp_to_percentage_bounds() {
        let mut states = tiles(5, 5);
        apply_effects(
            5,
            5,
            &mut states,
            &[
                effect(CRIME_RATE, 200, 2, false),
                effect(CRIME_RATE, -40, 2, false),
            ],
        );
        assert_eq!(states[0].crime_rate, 100);

        apply_effects(5, 5, &mut states, &[effect(CRIME_RATE, -1_000, 2, false)]);
        assert_eq!(states[0].crime_rate, 0);
    }

    #[test]
    fn deletion_is_the_inverse_of_construction() {
        let mut states = tiles(5, 5);
        apply_effects(
            5,
            5,
            &mut states,
            &[
                effect(EDUCATION_RATE, 60, 3, false),
                effect(EDUCATION_RATE, 60, 3, true),
            ],
        );
        assert!(states.iter().all(|tile| tile.education_rate == 0));
    }

    #[test]
    fn loaded_rates_are_preserved_while_another_kind_changes() {
        let mut states = tiles(5, 5);
        states[(2 * 5 + 2) as usize].land_price = 77;
        apply_effects(5, 5, &mut states, &[effect(NOISE_RATE, -10, 1, false)]);
        assert_eq!(states[(2 * 5 + 2) as usize].land_price, 77);
        assert!(states.iter().any(|tile| tile.noise_rate < 0));
    }

    #[test]
    fn updated_rates_are_immediately_visible_to_the_save_state() {
        let mut city = super::super::new_city_map();
        city.bulk_set_tiles(
            &[ffi::RawTileData {
                residents: 0,
                workers_commercial: 0,
                workers_office: 0,
                workers_industrial: 0,
                workers_farm: 0,
                workers_public: 0,
                students: 0,
                reservation: 0,
            }],
            1,
            1,
        );
        let input = vec![ffi::TileRateState {
            x: 0,
            y: 0,
            land_price: 77,
            crime_rate: -12,
            education_rate: 34,
            noise_rate: 5,
            happiness_rate: 6,
        }];

        city.update_rates(1, 1, input, Vec::new());

        let saved: serde_json::Value = serde_json::from_str(&city.generate_save_json()).unwrap();
        assert_eq!(saved["Map"][0][0]["rate"]["LandPrice"], 77);
        assert!(saved["Map"][0][0]["rate"].get("CrimeRate").is_none());
        assert_eq!(saved["Map"][0][0]["rate"]["EducationRate"], 34);
        assert_eq!(saved["Map"][0][0]["rate"]["NoiseRate"], 5);
        assert_eq!(saved["Map"][0][0]["rate"]["HappinessRate"], 6);
    }
}
