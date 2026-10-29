//
// Created by dte on 10/22/2025.
//

#pragma once
#include <list>
#include <memory>
#include "modules/core/vertex.hpp"

/**
 * @namespace geoma::importer::osm
 * @brief Loads road networks from OpenStreetMap XML files.
 */
namespace geoma::importer::osm
{
    /**
     * @brief Imports an @c .osm file into a geoma world.
     *
     * The file is parsed as XML; every @c <node> becomes a vertex (keyed by
     * its OSM id) and every @c <way> tagged with @c highway = * links its
     * consecutive nodes with two-way edges. Parsing is parallelized with a
     * thread pool. Isolated vertices — those that ended up with no
     * neighbors — are discarded before returning.
     *
     * @param filename Path to the OpenStreetMap XML file.
     * @return The list of connected vertices, or an empty list when the
     *         file cannot be read.
     * @note The whole file is loaded into memory at once; @c read_file()
     *       warns when the input exceeds ~1 GB.
     */
    std::list<std::shared_ptr<geoma::core::vertex>> import(const std::string & filename);
}
