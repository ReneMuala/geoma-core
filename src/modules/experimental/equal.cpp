//
// Created by dte on 11/27/2025.
//

#include "experimental.hpp"
#include "modules/core/vertex.hpp"

#include <iostream>
#include <memory>
#include <thread>
#include <cmath>
#include <fmt/base.h>
#include <algorithm>

namespace geoma::experimental
{
    bool equal(std::shared_ptr<geoma::core::vertex>& lhs, std::shared_ptr<geoma::core::vertex>& rhs) {

        if(not core::vertex::equals(lhs, rhs))
            return false;
        if(lhs->neighbors.size() != rhs->neighbors.size())
            return false;
        auto it = lhs->neighbors.begin(), it2 = rhs->neighbors.begin();
        while (it != lhs->neighbors.end()) {
            auto& vertex = *it;
            auto& vertex2 = *it2;
            if(not core::vertex::equals(vertex, vertex2))
                return false;
            it++,it2++;
        }
        return true;
    }

    bool equal(std::list<std::shared_ptr<geoma::core::vertex>>& world, std::list<std::shared_ptr<geoma::core::vertex>>& world2)
    {
        if(world.size() != world2.size()) return false;
        auto it = world.begin(), it2 = world2.begin();
        while (it != world.end()) {
            auto& lhs = *it;
            auto& rhs = *it2;
            if(not equal(lhs, rhs))
                return false;
            it++,it2++;
        }
        return true;
    }
}
