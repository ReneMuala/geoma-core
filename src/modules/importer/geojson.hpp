//
// Created by dte on 10/22/2025.
//

#pragma once
#include <list>
#include <memory>
#include "modules/core/vertex.hpp"

/**
 * @namespace geoma::importer::geojson
 * @brief Loads road networks from GeoJSON files.
 */
namespace geoma::importer::geojson
{
    /**
     * @brief Imports a GeoJSON file into a geoma world.
     *
     * Every feature's geometry coordinates are walked; each position
     * becomes a vertex (deduplicated by its coordinate pair) and
     * consecutive positions are joined with two-way edges, so linear
     * geometries (LineString and friends) turn into paths. Coordinates are
     * read as @c [lon, lat] and stored as @c x = lat, @c y = lon.
     *
     * @param filename Path to the GeoJSON file.
     * @return The list of vertices, or an empty list when the file cannot
     *         be read or does not parse as a GeoJSON feature collection.
     */
    std::list<std::shared_ptr<geoma::core::vertex>> import(const std::string & filename);
}
