//
// Created by dte on 11/27/2025.
//

#include "experimental.hpp"

#include <iostream>
#include <thread>
#include <cmath>

namespace geoma::experimental
{
    /**
     * @brief Computes the shortest distance from @p p to segment start–end.
     *
     * The closest point is the perpendicular projection of @p p onto the
     * segment's line, clamped to the segment endpoints. The projected
     * point itself is written into @p closest, so callers get both the
     * distance and the snap position in one call.
     *
     * @param p The point to measure from.
     * @param start Segment start.
     * @param end Segment end.
     * @param closest Output vertex receiving the projected point.
     * @return The distance between @p p and the segment.
     */
    static double line_point_distance(const std::shared_ptr<core::vertex>& p,
                                      const std::shared_ptr<core::vertex>& start,
                                      const std::shared_ptr<core::vertex>& end, std::shared_ptr<core::vertex> closest = core::vertex::make(0,0))
    {
        const double x = p->x, y = p->y, x1 = start->x, y1 = start->y, x2 = end->x, y2 = end->y;
        const double A = x - x1;
        const double B = y - y1;
        const double C = x2 - x1;
        const double D = y2 - y1;

        const double dot = A * C + B * D;
        const double len_sq = C * C + D * D;
        double param = -1;
        if (len_sq != 0) //in case of 0 length line
            param = dot / len_sq;

        // double xx, yy;

        if (param < 0)
        {
            closest->x = x1;
            closest->y = y1;
        }
        else if (param > 1)
        {
            closest->x = x2;
            closest->y = y2;
        }
        else
        {
            closest->x = x1 + param * C;
            closest->y = y1 + param * D;
        }

        const double dx = x - closest->x;
        const double dy = y - closest->y;
        return std::sqrt(dx * dx + dy * dy);
    }

    /**
     * @brief See experimental.hpp.
     *
     * Phase 1 scans every dead-end (vertex with exactly one neighbor) and
     * every edge (a, b): the dead-end's perpendicular projection onto a–b
     * is computed with line_point_distance(); when it is within
     * @p threshold, the tuple (vertex, closest, a, b) is queued and the
     * dead-end is marked solved.
     *
     * Phase 2 applies the queued repairs: a and b are unlinked from each
     * other, the projection point becomes a new junction joined to a, b
     * and the original dead-end, and the junction is appended to the world.
     *
     * @note Each dead-end is repaired at most once (the inner loop breaks
     *       after the first hit), and the scan is O(dead-ends × edges).
     */
    unsigned long long close(std::list<std::shared_ptr<geoma::core::vertex>>& world, double threshold)
    {
        unsigned long long count = 0;
        std::list<std::tuple<std::shared_ptr<core::vertex>, std::shared_ptr<core::vertex>, std::shared_ptr<core::vertex>, std::shared_ptr<core::vertex>>> conjugated_tuples;
        for (const auto& vertex : world)
        {
            // Only dead-ends (degree-1 vertices) are candidates for snapping.
            if (vertex->neighbors.size() == 1)
            {
                for (const auto& a : world)
                {
                    bool solved = false;
                    for (const auto& b : a->neighbors)
                    {
                        if (not core::vertex::joined(a, vertex) and a != vertex and b != vertex)
                        {
                            const auto closest = core::vertex::make(0,0);
                            if (line_point_distance(vertex, a, b, closest) <= threshold)
                            {
                                conjugated_tuples.emplace_back(vertex, closest, a, b);
                                core::vertex::join(closest, vertex);
                                count++;
                                solved = true;
                                break;
                            }
                        }
                    }
                    if (solved)
                        break;
                }
            }
        }

        // Phase 2: split each matched edge at the projection point.
        for (auto & [vertex, closest, a, b] : conjugated_tuples)
        {
            world.push_back(closest);
            a->neighbors.remove(b);
            b->neighbors.remove(a);

            core::vertex::join(a, closest);
            core::vertex::join(closest, a);

            core::vertex::join(b, closest);
            core::vertex::join(closest, b);

            core::vertex::join(vertex, closest);
            core::vertex::join(closest, vertex);
        }
        return count;
    }
}
