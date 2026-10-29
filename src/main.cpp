#include <fmt/base.h>
#include <iostream>

#include "geoma.hpp"
#include "modules/core/navigator.hpp"
#include "modules/experimental/experimental.hpp"
#include "modules/exporter/links.hpp"
#include "modules/exporter/native.hpp"
#include "modules/exporter/svg.hpp"
#include "modules/importer/geojson.hpp"
#include "modules/importer/native.hpp"

/**
 * @brief Demo: import an OSM extract, route across it, export results.
 *
 * Imports "pontagea.osm", asks the navigator for routes between two
 * coordinates near the extract, with a budget of 5 solutions / 3 seconds,
 * then writes the world both as SVG (for visual inspection) and as
 * native binary (for fast reloads). Timing of the search is printed.
 */
void pontagea_test() {
  const auto world = geoma::importer::osm::import("pontagea.osm");
  const auto navigator = geoma::core::navigator::make(
      geoma::core::vertex::make(-19.8450674, 34.8561643),
      geoma::core::vertex::make(-19.8469908, 34.8558899));
  const auto counter =
      geoma::core::incremental_timed_counter::make(5, std::chrono::seconds(3));
  geoma::core::navigator::solutions solutions;
  const auto start = std::chrono::high_resolution_clock::now();
  geoma::core::navigator::navigate(navigator, world, counter, solutions);
  const auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "[terminated in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   elapsed_seconds)
                   .count()
            << " milliseconds]" << std::endl;
  geoma::exporter::svg::export_(world, "pontagea.svg", 10000);
  geoma::exporter::native::export_(world, "pontagea.bin");
}

/**
 * @brief Demo: convert a large OSM extract to geoma's native binary.
 *
 * Imports a country-sized OSM file and serializes it straight to
 * "mozambique.bin" — a one-time conversion so later runs can skip the
 * expensive XML parse.
 */
void moz_test() {
  const auto world = geoma::importer::osm::import(
      R"(C:\Users\dte\Documents\osmosis\mozambique-251020.osm_01.osm)");

  geoma::exporter::native::export_(world, "mozambique.bin");
}

/**
 * @brief Demo: the full experimental pipeline on a GeoJSON extract.
 *
 * Walks a GeoJSON file through every experimental operation — count,
 * subdivide, dissolve, close, decouple, count again — reporting how many
 * elements each step touched, then routes between two coordinates with a
 * budget of 10 solutions / 2 seconds and renders the cleaned world to
 * "port.svg". This is the best reference for how the pieces fit together.
 */
void port_test() {
  auto world = geoma::importer::geojson::import(
      "/home/rmuala/Documents/personal/projects/cpp/geoma/assets/"
      "1048170925.geojson");
  unsigned long long size, connections;
  geoma::experimental::count(world, size, connections);
  fmt::println("[world size = {} connections = {}]", size, connections);
  auto subdivided = geoma::experimental::subdivide(world);
  std::cout << "\t[subdivided paths = " << subdivided << "]" << std::endl;
  auto dissolved_points = geoma::experimental::dissolve(world, 5e-5);
  // auto dissolved_points = geoma::experimental::dissolve(world, 5e-5);
  std::cout << "\t[dissolved points = " << dissolved_points << "]" << std::endl;
  const auto closed_points =
      true ? geoma::experimental::close(world, 1e-12) : 0;
  std::cout << "\t[closed points = " << closed_points << "]" << std::endl;
  dissolved_points = geoma::experimental::dissolve(world, 1e-5);
  std::cout << "\t[dissolved points = " << dissolved_points << "]" << std::endl;
  auto decoupled = geoma::experimental::decouple(world);
  std::cout << "\t[decoupled points = " << decoupled << "]" << std::endl;
  geoma::experimental::count(world, size, connections, false);
  fmt::println("[world size = {} connections = {}]", size, connections);
  const auto p1 =
      geoma::core::vertex::make(-19.82091919727018, 34.83411571790424);
  const auto p2 =
      geoma::core::vertex::make(-19.81611383897029, 34.8387300568233);
  const auto navigator = geoma::core::navigator::make(p1, p2);
  const auto counter =
      geoma::core::incremental_timed_counter::make(10, std::chrono::seconds(2));
  geoma::core::navigator::solutions solutions;
  const auto start = std::chrono::high_resolution_clock::now();
  geoma::core::navigator::navigate(navigator, world, counter, solutions);
  const auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  fmt::println(
      "[terminated in {} milliseconds with {} solutions]",
      std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_seconds)
          .count(),
      solutions.size());
  geoma::exporter::svg::export_(world, "port.svg", 1e5);
}

/// Whats happening here?
/// We are
/// 1. preprocessing a map (removing noise) to simplify its struture
/// 2. optimizing its file format (to Geoma's Binary Map-format) so that it loads fast and uses less disk space
void denoise_and_shrink_map() {
  const auto start = std::chrono::high_resolution_clock::now();

  auto world = geoma::importer::geojson::import(
      R"(/home/rmuala/Documents/personal/projects/cpp/geoma/assets/1048170925.geojson)"); // 27.9 kB

  {
    auto r = geoma::experimental::decouple(world);
    fmt::println("[{}: decoupled points = {}]", __FUNCTION__, r);
  }

  {
    auto r = geoma::experimental::dissolve(world, 3e-5);
    fmt::println("[{}: dissolved points = {}]", __FUNCTION__, r);
  }

  {
    auto r = geoma::experimental::close(world, 1e-4);
    fmt::println("[{}: closed points = {}]", __FUNCTION__, r);
  }

  {
    auto r = geoma::experimental::subdivide(world);
    fmt::println("[{}: subdivided points = {}]", __FUNCTION__, r);
  }
  {
    auto r = geoma::experimental::decouple(world);
    fmt::println("[{}: decoupled points = {}]", __FUNCTION__, r);
  }

  {
    auto r = geoma::experimental::dissolve(world, 5e-5);
    fmt::println("[{}: dissolved points = {}]", __FUNCTION__, r);
  }

  constexpr auto limit = 10;
  for (int i = 0; i < limit; i++) {
    fmt::println("[{}: pass {}/{}]", __FUNCTION__, i, limit);

    {
      auto r = geoma::experimental::subdivide(world);
      fmt::println("[{}: subdivided points = {}]", __FUNCTION__, r);
    }

    {
      auto r = geoma::experimental::dissolve(world, 5e-5);
      fmt::println("[{}: dissolved points = {}]", __FUNCTION__, r);
    }

    {
      auto r = geoma::experimental::decouple(world);
      fmt::println("[{}: decoupled points = {}]", __FUNCTION__, r);
    }
  }

  const auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "[terminated in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   elapsed_seconds)
                   .count()
            << " milliseconds]" << std::endl;
  geoma::exporter::svg::export_(world, "test.svg", 1e5);
  geoma::exporter::native::export_(world, "port.gbm"); // 16.1 kB
  auto world2 = geoma::importer::native::import("port.gbm");
  fmt::println("equal = {}", geoma::experimental::equal(world, world2));
}

void port_path_check() {
  const auto world = geoma::importer::native::import("port.gbm");
  const auto navigator = geoma::core::navigator::make(
      geoma::core::vertex::make(-19.82157568436394, 34.83398647753145),
      geoma::core::vertex::make(-19.81469761800921, 34.83974110285494)
      // geoma::core::vertex::make(-19.81622214009955,34.83704220037821),
  );
  const auto counter =
      geoma::core::incremental_timed_counter::make(2, std::chrono::seconds(15));
  geoma::core::navigator::solutions solutions;
  const auto start = std::chrono::high_resolution_clock::now();
  geoma::core::navigator::navigate(
      navigator, world, counter, solutions,
      geoma::core::navigator::flags{
          // this boost up performance by forcing the navigator to consider only the top N candidates and ignore the rest
          .next_steps_limit = 3
      });
  const auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  fmt::println(
      "[terminated in {} milliseconds with {} solutions]",
      std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_seconds)
          .count(),
      solutions.size());
  geoma::exporter::svg::export_(world, "port.svg", 1e6);
}

/**
 * @brief Program entry point.
 *
 * Currently wired to run the demo routines above (port_test by default;
 * pontagea_test and moz_test commented out). The code after the return
 * kept for reference times a bare import + SVG export.
 *
 * To use geoma as a library, link against these sources and write your
 * own main() following the demos: import → (experimental cleanup) →
 * navigate → export.
 */
int main(int argc, char **argv) {
  // port_test();
  // pontagea_test();
  // moz_test();
  // denoise_and_shrink_map();
  port_path_check();
  return 0;
  const auto start = std::chrono::high_resolution_clock::now();
  // const auto world = geoma::importer::osm::import("pontagea.osm");
  const auto world = geoma::importer::geojson::import(
      R"(/home/rmuala/Documents/personal/projects/cpp/geoma/assets/1048170925.geojson)");
  const auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "[terminated in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   elapsed_seconds)
                   .count()
            << " milliseconds]" << std::endl;
  geoma::exporter::svg::export_(world, "test.svg", 1e5);
  return 0;
}
