//
// Created by dte on 11/27/2025.
//

#include "experimental.hpp"

#include <iostream>
#include <thread>
#include <cmath>
#include <fmt/base.h>

namespace geoma::experimental
{
    /**
     * @brief Intersection test of segments p1–p2 and p3–p4.
     *
     * Standard parametric line-line intersection: uA and uB locate the
     * crossing along each segment. When both lie in [0, 1] the segments
     * actually cross and the intersection point is returned; otherwise
     * (parallel or non-crossing) @c nullptr is returned.
     */
    static std::shared_ptr<core::vertex> line_line(std::shared_ptr<core::vertex> p1, std::shared_ptr<core::vertex> p2, std::shared_ptr<core::vertex> p3, std::shared_ptr<core::vertex> p4) {
        double dividend = (p4->y - p3->y) * (p2->x - p1->x) - (p4->x - p3->x) * (p2->y - p1->y);
        if (dividend == 0) return nullptr;
        // calculate the distance to intersection point
        double uA = ((p4->x - p3->x) * (p1->y - p3->y) - (p4->y - p3->y) * (p1->x - p3->x)) / dividend;
        double uB = ((p2->x - p1->x) * (p1->y - p3->y) - (p2->y - p1->y) * (p1->x - p3->x)) / dividend;
        // if uA and uB are between 0-1, lines are colliding
        if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
            return core::vertex::make(p1->x + (uA * (p2->x - p1->x)), p1->y + (uA * (p2->y - p1->y)));
        }
        return nullptr;
    }
    /**
     * @brief See experimental.hpp.
     *
     * Phase 1 collects every crossing: for each edge v1–n1 and each edge
     * v2–n2 that shares no endpoint with it, line_line() computes an
     * intersection vertex. Hits are queued as (v1, n1, v2, n2, c).
     *
     * Phase 2 rewires: both original edges are removed and replaced by
     * four edges through the intersection vertex c, which is appended to
     * the world. Collecting first and rewiring after keeps the loops from
     * iterating over a graph that mutates underneath them.
     */
    unsigned long long subdivide(std::list<std::shared_ptr<geoma::core::vertex>>& world)
    {
        unsigned long long count = 0;
        std::list<std::tuple<std::shared_ptr<geoma::core::vertex>, std::shared_ptr<geoma::core::vertex>, std::shared_ptr<geoma::core::vertex>, std::shared_ptr<geoma::core::vertex>, std::shared_ptr<geoma::core::vertex>>> v1n1v2n2c;
        for (const auto& v1 : world)
        {
            for (const auto& n1 : v1->neighbors)
            {
                for (const auto& v2 : world)
                {
                    if (geoma::core::vertex::joined(v1, v2) or geoma::core::vertex::joined(n1, v2)) continue;
                    for (const auto& n2 : v2->neighbors)
                    {
                        if (geoma::core::vertex::equals(v1, n2)) continue;
                        if (auto c = line_line(v1, n1, v2, n2))
                        {
                            // fmt::println("[{} {} -> {} with {} -> {}]", __FUNCTION__, std::string(*v1), std::string(*n1), std::string(*v2), std::string(*n2));
                            v1n1v2n2c.emplace_back(v1, n1, v2, n2, c);
                            count++;
                        }
                    }
                }
            }
        }
        for (const auto & [v1, n1, v2, n2, c] : v1n1v2n2c)
        {
            v1->neighbors.remove(n1);
            n1->neighbors.remove(v1);

            v2->neighbors.remove(n2);
            n2->neighbors.remove(v2);

            geoma::core::vertex::join(v1, c);
            geoma::core::vertex::join(c, v1);

            geoma::core::vertex::join(n1, c);
            geoma::core::vertex::join(c, n1);

            geoma::core::vertex::join(v2, c);
            geoma::core::vertex::join(c, v2);

            geoma::core::vertex::join(n2, c);
            geoma::core::vertex::join(c, n2);

            world.push_back(c);
        }
        return count;
    }
}
