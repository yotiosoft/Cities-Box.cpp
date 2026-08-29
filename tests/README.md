# Connectable behavior characterization tests

These tests freeze the current C++ contracts used by road, railroad, waterway, runway, and taxiway connection handling without requiring OpenSiv3D at runtime.

Covered behavior:

- stable numeric values for the TypeID, DirectionID, and CategoryID values persisted across the C++/Rust/save boundary;
- isolated, dead-end, straight, turn, T-intersection, and cross-intersection shapes;
- waterway-specific estuary, T-intersection, turn, offshore, and missing-corner shapes;
- current `canConnect()` compatibility through Road, Train, Waterway, and Airport categories;
- current `isMatch()` distinction between Railroad/Station and Runway/Taxiway;
- train-crossing and bridge category combinations;
- representative connection-extension and removal shape transitions;
- coordinate-to-direction decisions without OpenSiv3D types;
- value-based connection and removal decision results.

Run from a Visual Studio Developer PowerShell:

```powershell
./tests/run_connectable_behavior_tests.ps1
```

The executable is built under the system temporary directory. No generated test binary is written into the repository.

Save/reload preservation of a connectable object's TypeID, DirectionID, and tile reference is tested separately by the Rust test `connectable_shape_and_tile_reference_survive_save_and_reload`.
