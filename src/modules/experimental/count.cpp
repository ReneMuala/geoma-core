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
     * Walks the world once, summing every vertex's neighbor count into
     * @p connections (directed edges — a two-way road counts twice) and
     * storing the world's length into @p size.
     */
    void count(std::list<std::shared_ptr<geoma::core::vertex>>& world, unsigned long long & size, unsigned long long & connections, bool display)
    {
        unsigned long long conn = 0;
        for (const auto& vertex : world)
        {
            if (display)
            {
                fmt::println("[{} vertex = {} connections = {}]", __FUNCTION__, std::string(*vertex), vertex->neighbors.size());
            }
            conn+=vertex->neighbors.size();
        }
        size = world.size();
        connections = conn;
    }
}
