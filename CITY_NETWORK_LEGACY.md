# CityNetwork legacy implementation memo

This memo records the C++ `CityNetwork` implementation that was removed in August 2026 so that a future Rust implementation does not have to infer its original intent from history alone.

## Removed API and storage

`CityNetwork` was declared in `src/CityNetwork.hpp` with the following public API:

```cpp
bool append(int x, int y);
bool del(int x, int y);
LineString solve(CoordinateStruct start, CoordinateStruct end);
```

Its private state was:

```cpp
Polygon m_polygon;
NavMesh m_navmesh;
```

Only `append()` had an implementation. It constructed an OpenSiv3D `Circle` of radius 1 centered at `(x, y)`, converted it to a polygon, and appended that polygon to `m_polygon`. `del()` and `solve()` were declared but never implemented. No code called any of the three methods.

`CityMap` nevertheless owned a `CityNetwork road_network` member. `Object::connect()`, `Object::connectWithSpecifiedType()`, and `Object::del()` accepted it by reference, and the argument was forwarded through `CityMap`, `NormalObject`, and `ConnectableObject`. None of those functions read or modified it.

## Actual connection implementation at removal time

Road, railroad, waterway, taxiway, and runway shape updates did not use `CityNetwork`. The active implementation was distributed across:

- `CityMap_Road.cpp`: placement, drag-to-connect behavior, crossings, bridges, and unfinished-connectable tracking.
- `ConnectableObject.cpp`: direction accumulation, TypeID selection, connection removal, and neighboring-object updates.
- `Object::m_connects`: a two-dimensional array of `ConnectStruct`; each entry stored direction plus raw `Object*` links.
- `CityMap::m_constructing_connectable_objects`: raw `Object*` values for isolated connectable objects removed when the build menu closes.

There were no distinct road IDs, edge IDs, connected-component IDs, or route data. Placement objects used the common Object ID. Connection edges were not serialized, and loading recreated `ConnectableObject` instances and their TypeID/DirectionID but did not rebuild `m_connects`.

Removing `CityNetwork` therefore removed only an unused OpenSiv3D `Polygon`/`NavMesh` skeleton and unused function parameters. It did not remove the active connection or road-shape logic.

## Guidance for a future Rust implementation

Do not reproduce the removed polygon placeholder as-is. Start from value types without OpenSiv3D types or C++ pointers, for example:

- `ObjectId` and `Coordinate { x, y }`
- a connectable kind such as road, railroad, waterway, taxiway, or runway
- nodes keyed by Object ID
- explicit edges containing both endpoint IDs and endpoint directions
- construction state and connected-component/isolation state
- pure shape calculation from neighboring connection directions

The C++ boundary should exchange DTO snapshots and mutation results. It must not pass `Object*`, `Tile*`, `CBAddon*`, `Polygon`, `NavMesh`, or other OpenSiv3D types to Rust.

Before making Rust graph state authoritative, preserve save compatibility and define how connections are reconstructed from the currently serialized object/tile data. Existing saves contain object IDs, TypeID, DirectionID, origins, and tile references, but no explicit edge list.
