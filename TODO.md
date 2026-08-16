# TODO

## Finance and public-service budgets

- [ ] Apply each service budget percentage to the corresponding gameplay effect, not only to its monthly expense.
  - Define the effect curve for 0–200% funding before implementation; do not assume that effects scale linearly.
  - Determine behavior for underfunding, overfunding, delayed recovery, and whether changes take effect immediately or at the next monthly finance update.
  - Cover police, fire, post, and education separately because their affected rates and operating rules may differ.
  - Keep the gameplay calculation in Rust. C++ should continue to provide map/addon data and render the resulting state.

- [ ] Define how land price affects finance calculations.
  - Decide whether land price changes residential tax, business tax, maintenance costs, or a separate property-tax source.
  - Specify whether to use per-tile land price, an occupied-area average, a population-weighted average, or a building-level value.
  - Deduplicate multi-tile buildings so the same property is not counted once per tile.
  - Define normalization, caps, rounding, update timing, and behavior for missing or invalid land-price data.
  - Extend `SimulationMapStats` or the cxx input structs only after the required statistic is defined, and preserve the existing save-data format.

- [ ] Add addon-authoring UI for the category-specific monthly maintenance fields.
  - `monthly_maintenance_police`
  - `monthly_maintenance_fire`
  - `monthly_maintenance_post`
  - `monthly_maintenance_education`
  - Existing addons without these fields currently use 1,000 per matching service category for backward-compatible 100% funding costs.

- [ ] Complete a Windows Release x64 C++ build after resolving the local MSBuild `Path`/`PATH` duplicate environment-variable error (`MSB6001`).
