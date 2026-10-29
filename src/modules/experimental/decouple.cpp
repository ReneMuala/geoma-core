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
     * @brief See experimental.hpp.
     *
     * For every vertex the neighbor list is rebuilt from scratch: each
     * entry is attempted to be removed from the "unique" accumulation
     * list first — std::list::remove returns whether anything was
     * removed — so a duplicate bumps @p count instead of being appended
     * twice. The result replaces the vertex's neighbor list, keeping the
     * first occurrence of each target in original order.
     */
    unsigned long long decouple(std::list<std::shared_ptr<geoma::core::vertex>>& world)
    {
        unsigned long long count = 0, conn = 0;
        for (const auto& vertex : world)
        {
            std::list<std::shared_ptr<geoma::core::vertex>> unique_neighbors;
            for (const auto& neighbor : vertex->neighbors)
            {
                conn ++;
                if (unique_neighbors.remove(neighbor))
                {
                    count++;
                }
                unique_neighbors.push_back(neighbor);
            }
            vertex->neighbors.clear();
            vertex->neighbors = unique_neighbors;
        }
        return count;
    }
}
