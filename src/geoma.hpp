//
// Created by dte on 10/22/2025.
//

/**
 * @file geoma.hpp
 * @brief Umbrella header for the whole geoma toolkit.
 *
 * Including this single header pulls in every public module of geoma:
 *
 * - @ref geoma::core         — the vertex graph, the navigator and its counters
 * - @ref geoma::importer     — world builders (OpenStreetMap XML, GeoJSON)
 * - @ref geoma::exporter     — world serializers (SVG, native binary, link lists)
 * - @ref geoma::experimental — graph cleanup and analysis utilities
 *
 * A typical program only needs:
 * @code
 * #include "geoma.hpp"
 * @endcode
 *
 * The library is header/interface driven and is built with C++20.
 */

#pragma once

#include "modules/core/counter.hpp"
#include "modules/core/incremental_counter.hpp"
#include "modules/core/timed_counter.hpp"
#include "modules/core/incremental_timed_counter.hpp"
#include "modules/core/navigator.hpp"
#include "modules/core/step.hpp"
#include "modules/core/vertex.hpp"

#include "modules/importer/osm.hpp"
#include "modules/exporter/svg.hpp"
#include "modules/exporter/links.hpp"
#include "modules/experimental/experimental.hpp"
