//
// Created by dte on 10/22/2025.
//

#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <math.h>
#include <memory>
#include <optional>
#include <string>
#include <chrono>
#include <deque>
#include <thread>
#include "vertex.hpp"
#include "counter.hpp"
#include "step.hpp"

namespace geoma::core
{
    /**
     * @brief Plans routes between two points across a world.
     *
     * A navigator bundles the @p origin and @p target of a journey. The
     * actual search happens in the static @c navigate() function, which
     * explores the world level by level, always expanding the frontier
     * steps that land closest to the target first (greedy best-first
     * search). Every route that reaches the target is appended to a
     * solutions list, and a @ref counter decides when enough is enough —
     * either after a number of solutions or after a time budget.
     */
    struct navigator
    {
        struct flags {
            std::optional<int> next_steps_limit;
        };

        /** @brief Where the journey starts. */
        std::shared_ptr<vertex> origin;
        /** @brief Where the journey ends. */
        std::shared_ptr<vertex> target;

        /** @brief A route: the ordered list of vertices from target back to origin. */
        typedef std::list<std::list<std::shared_ptr<vertex>>> solutions;

        /**
         * @brief Factory: allocates a navigator for an origin/target pair.
         * @param origin Starting point of the journey.
         * @param target Destination of the journey.
         * @return A shared pointer to the freshly created navigator.
         */
        static std::shared_ptr<navigator> make(const std::shared_ptr<vertex>& origin,
                                               const std::shared_ptr<vertex>& target);

        /**
         * @brief Searches @p world for routes from origin to target.
         *
         * The origin and target are first snapped to their closest vertices
         * in the world. The search then expands the frontier repeatedly;
         * at each level, candidate steps are sorted by their remaining
         * distance to the target so the most promising ones are explored
         * first. When the target's closest vertex is reached, the route is
         * recorded via @ref step::walkthrough and the counter is ticked.
         *
         * @param nav Navigator holding the origin and target.
         * @param world The vertex graph to search in.
         * @param counter Budget controlling the search (see @ref counter).
         * @param solutions Output list; every found route is appended to it.
         *
         * @note If either the origin or the target has no closest vertex
         *       (an empty world), the function returns without searching.
         *       Routes are appended in target-to-origin order.
         */
        static void navigate(const std::shared_ptr<navigator>& nav, const std::list<std::shared_ptr<vertex>>& world,
                             const std::shared_ptr<counter>& counter, solutions& solutions, const flags &flags = {});
    };
} // geoma
