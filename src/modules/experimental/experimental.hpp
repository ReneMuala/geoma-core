//
// Created by dte on 11/27/2025.
//

#pragma once

#include "modules/core/vertex.hpp"

/**
 * @namespace geoma::experimental
 * @brief Graph cleanup and analysis utilities.
 *
 * These functions edit a world in place and all return a count of the
 * elements they touched. They exist to repair real-world data before
 * routing: raw imports tend to contain duplicated points, crossing edges
 * and dangling dead-ends.
 */
namespace geoma::experimental
{
    /**
     * @brief Merges vertices that are closer than @p threshold.
     *
     * Vertices are scanned front to back; the first vertex found within
     * @p threshold of the current one absorbs all of its edges and the
     * current vertex is dropped from the world (but kept aside so its
     * stale links can be pruned). The world shrinks accordingly.
     *
     * @param world The world to clean up; modified in place.
     * @param threshold Maximum distance (in coordinate units) at which two
     *                  vertices are considered the same point.
     * @return The number of vertices removed (absorbed into another one).
     */
    unsigned long long dissolve(std::list<std::shared_ptr<geoma::core::vertex>>& world, double threshold);

    /**
     * @brief Snaps dangling dead-end vertices onto nearby edges.
     *
     * Every vertex with exactly one neighbor is tested against each edge
     * (pair of joined vertices) in the world. If its perpendicular
     * distance to the edge is within @p threshold, the edge is split: a
     * new vertex is inserted at the closest point and the dead-end is
     * attached to it, while the original edge endpoints are unlinked from
     * each other.
     *
     * @param world The world to clean up; new vertices are appended to it.
     * @param threshold Maximum distance (in coordinate units) at which a
     *                  dead-end is snapped onto an edge.
     * @return The number of vertices snapped (i.e. new junctions created).
     */
    unsigned long long close(std::list<std::shared_ptr<geoma::core::vertex>>& world, double threshold);

    /**
     * @brief Removes duplicated neighbor links.
     *
     * Rebuilds every vertex's neighbor list keeping only the first
     * occurrence of each target, so repeated @c join() calls between the
     * same pair collapse into a single edge per direction.
     *
     * @param world The world to clean up; modified in place.
     * @return The number of duplicate links removed.
     */
    unsigned long long decouple(std::list<std::shared_ptr<geoma::core::vertex>>& world);

    /// NICE TO HAVE: remove direct edges when two edges have indirect paths
    // unsigned long long dechord_indirects(std::list<std::shared_ptr<geoma::core::vertex>>& world);

    /**
     * @brief Splits edges that cross each other at their intersection.
     *
     * Every pair of non-adjacent edges is tested for intersection. For
     * each hit, both edges are removed and four new edges are created
     * through a fresh vertex placed at the crossing point, which is
     * appended to the world.
     *
     * @param world The world to clean up; intersection vertices are
     *              appended to it.
     * @return The number of intersections found (and junctions created).
     */
    unsigned long long subdivide(std::list<std::shared_ptr<geoma::core::vertex>>& world);

    /**
     * @brief Measures the world: how many vertices, how many edges.
     *
     * @param world The world to measure.
     * @param size Output: the number of vertices in the world.
     * @param connections Output: the total number of directed edges
     *                    (each two-way edge counts twice).
     * @param display When true, prints one line per vertex with its
     *                coordinates and neighbor count.
     */
    void count(std::list<std::shared_ptr<geoma::core::vertex>>& world, unsigned long long & size, unsigned long long & connections, bool display = false);

    bool equal(std::list<std::shared_ptr<geoma::core::vertex>>& world, std::list<std::shared_ptr<geoma::core::vertex>>& world2);
}
