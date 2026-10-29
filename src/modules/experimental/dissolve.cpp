//
// Created by dte on 11/27/2025.
//

#include "experimental.hpp"

#include <iostream>
#include <thread>

namespace geoma::experimental
{
    /**
     * @brief See experimental.hpp.
     *
     * Algorithm: repeatedly take the front vertex and look for the first
     * remaining vertex within @p threshold. When found, the survivor
     * inherits every edge of the absorbed one (links that already exist
     * are skipped), all references to the absorbed vertex are erased from
     * its neighbors, and the absorbed vertex is parked in @p deleted for
     * later pruning of stale links. Vertices that survive untouched are
     * also scrubbed of links pointing at previously deleted vertices.
     *
     * Complexity is roughly O(n²) in the worst case — fine for cleanup of
     * regional extracts, expensive for whole continents.
     */
    unsigned long long dissolve(std::list<std::shared_ptr<geoma::core::vertex>>& world, double threshold)
    {
        unsigned long long count = 0;
        std::list<std::shared_ptr<geoma::core::vertex>> new_world;
        std::list<std::shared_ptr<geoma::core::vertex>> deleted;
        while (not world.empty())
        {
            bool unique = true;
            std::shared_ptr<geoma::core::vertex> vertex = world.front();
            world.pop_front();
            for (auto& other : world)
            {
                if (core::vertex::distance(vertex, other) <= threshold)
                {
                    // "other" survives and inherits vertex's edges.
                    for (const auto& neighbor : vertex->neighbors)
                    {
                        if (not core::vertex::joined(other, neighbor) and other != neighbor)
                        {
                            core::vertex::join(other, neighbor);
                            core::vertex::join(neighbor, other);
                        }
                        neighbor->neighbors.remove(vertex);
                    }
                    vertex->neighbors.clear();
                    count++;
                    unique = false;
                    break;
                }
            }
            if (unique)
            {
                new_world.push_back(vertex);
                // Prune links to vertices absorbed earlier in this pass.
                for (auto & deleted_vertex : deleted)
                {
                    if (core::vertex::joined(vertex, deleted_vertex))
                    {
                        vertex->neighbors.remove(deleted_vertex);
                    }
                }
            }
            else deleted.push_back(vertex);
        }
        world.swap(new_world);
        return count;
    }
}
