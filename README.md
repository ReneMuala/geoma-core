# geoma

A compact C++20 toolkit for loading real-world road networks, repairing their graphs,
and finding routes across them. 

## Features

- **Import**: OpenStreetMap `.osm` XML (thread-pool parsing, country-sized extracts),
  GeoJSON feature collections, and geoma's own compact native binary format for
  faster reloads.
- **Graph repair**: in-place cleanup operations for messy map data: merge
  near-duplicate points (`dissolve`), snap dead-ends onto edges (`close`), split
  crossing edges (`subdivide`), and collapse duplicated links (`decouple`).
- **Routing**: a greedy best-first search with explicit budgets: stop after N
  solutions, after a wall-clock deadline, or both; an optional frontier cap
  (`navigator::flags::next_steps_limit`) bounds per-level work on dense graphs.
- **Export**: interactive SVG maps (hoverable roads, labeled vertices), native
  binary dumps, and plain-text edge lists for debugging and diffing.

## Requirements

- A C++20 compiler (MSVC, GCC or Clang; builds on Windows and Linux)
- [xmake](https://xmake.io) (recommended) or CMake ≥ 3.15
- Dependencies fetched automatically by xmake: `rapidxml`, `jsoncpp`, `lunasvg`, `fmt`

## Build & run

```sh
xmake                 # configure + build
xmake run geoma       # run the demo program
```

## Example

```cpp
#include "geoma.hpp"                   
#include "modules/importer/geojson.hpp"
#include <fmt/core.h>

int main()
{
    // 1. Load a road network into a "world"
    auto world = geoma::importer::geojson::import("city.geojson");

    // 2. Clean the graph so routes look sane
    geoma::experimental::decouple(world);                      // drop duplicated links
    auto merged = geoma::experimental::dissolve(world, 5e-5);  // merge near-duplicate points
    auto closed = geoma::experimental::close(world, 1e-4);     // snap dead-ends onto edges

    // 3. Define the journey: A to B, at most 10 routes within 2 seconds
    auto nav    = geoma::core::navigator::make(
                      geoma::core::vertex::make(-19.8209, 34.8341),
                      geoma::core::vertex::make(-19.8161, 34.8387));
    auto budget = geoma::core::incremental_timed_counter::make(10, std::chrono::seconds(2));

    geoma::core::navigator::solutions solutions;
    geoma::core::navigator::navigate(nav, world, budget, solutions);

    // 4. Inspect the result
    fmt::println("found {} route(s)", solutions.size());
    geoma::exporter::svg::export_(world, "city.svg", 1e5);
    return 0;
}
```

Thresholds are expressed in coordinate units: for latitude/longitude data,
`5e-5` degrees is roughly 5 meters. The SVG `zoom` argument is pixels per
coordinate unit; `1e5` is a good starting point for city-scale degree coordinates.

## How it works

```
Import  ──►  Clean up  ──►  Navigate  ──►  Export
OSM / GeoJSON / binary     greedy best-first     SVG / binary / edge list
                           with budgets
```

Every stage is optional and independent. You can import and immediately export to
inspect your data, skip cleanup entirely, or build a world by hand with
`vertex::make` / `vertex::join` and never touch a file.

For large datasets, convert once with the OSM importer and reload through
`importer::native::import` afterwards: the binary format skips the XML parse entirely.

## Project layout

```
src/
├── geoma.hpp                  umbrella header (core, osm importer, svg/links exporters, experimental)
├── main.cpp                   demo routines
└── modules/
    ├── core/                  vertex, navigator (+ flags, counters), step
    ├── importer/              osm, geojson, native
    ├── exporter/              svg, native, links
    ├── experimental/          dissolve, close, subdivide, decouple, count, equal
    └── misc/                  thread_pool
docs/
└── index.html                 full documentation (open in a browser)
```

## Documentation

Open [`docs/index.html`](docs/index.html) in a browser for the full guide: core
concepts, cleanup operations, data formats (including the native binary layout),
the complete API reference, FAQ and glossary.

## Data sources

Any `.osm` export or any GeoJSON with linear features works. Start small: a
neighborhood-sized extract is plenty while learning the API.

## License

MIT: see [LICENSE](LICENSE).
