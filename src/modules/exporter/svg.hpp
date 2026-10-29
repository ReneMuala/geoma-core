//
// Created by dte on 10/22/2025.
//
#pragma once
#include <list>
#include <memory>
#include <string>
#include "modules/core/vertex.hpp"

/**
 * @namespace geoma::exporter::svg
 * @brief Renders worlds as interactive SVG images.
 */
namespace geoma::exporter::svg
{
    /**
     * @brief Writes the world to an SVG file.
     *
     * Edges become orange @c <line> elements (cyan on hover) and vertices
     * become blue dots annotated with their coordinates and neighbor
     * count. The world's bounding box is computed first and everything is
     * translated into it, so the output starts at (0, 0).
     *
     * @param world The vertices to render.
     * @param filename Path of the SVG file to write.
     * @param zoom Scale factor: pixels per coordinate unit.
     * @return True on success, false when the world is empty, has zero
     *         extent, or the file cannot be opened.
     */
    bool export_(const std::list<std::shared_ptr<geoma::core::vertex>> & world,const std::string & filename, const double zoom);
}
