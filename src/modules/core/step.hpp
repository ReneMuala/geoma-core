//
// Created by dte on 10/22/2025.
//

#ifndef STEP_HPP
#define STEP_HPP
#include <algorithm>
#include <iostream>
#include <list>
#include <math.h>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include "vertex.hpp"

namespace geoma::core
{

    /**
     * @brief A node in the search tree built by @ref navigator::navigate.
     *
     * Each step remembers the vertex it lands on, the step it came from
     * (@p predecessor) and a heuristic @p distance — the gap remaining to
     * the target. Following @p predecessor links from any step leads back
     * to the origin, which is how complete routes are recovered.
     */
    struct step
    {
        /** @brief The vertex this step sits on. */
        std::shared_ptr<struct vertex> vertex;
        /** @brief The step this one extends (@c nullptr for the root). */
        std::shared_ptr<step> predecessor;
        /** @brief Heuristic cost of this step (distance remaining to the target). */
        double distance;
        /** @brief Child steps that extend this one. */
        std::list<std::shared_ptr<step>> successors = {};

        /**
         * @brief Debug helper: prints the whole chain of a step to stdout.
         * @param step Leaf of the chain to print.
         */
        static void print(const std::shared_ptr<step>& step);

        /**
         * @brief Flattens a step chain into a vertex route.
         * @param step Leaf (usually a final step reaching the target).
         * @return The vertices along the chain, ordered from the leaf back
         *         to the root (i.e. target-first, origin-last).
         */
        static std::list<std::shared_ptr<struct vertex>> walkthrough(const std::shared_ptr<step>& step);

        /**
         * @brief Cycle guard: checks whether a vertex already appears in a chain.
         * @param step Chain to inspect (usually the current step).
         * @param vertex Vertex about to be expanded.
         * @return True if @p vertex is @b not already part of the chain,
         *         i.e. expanding it would not loop back on itself.
         * @note Comparison is by pointer identity, not by coordinates.
         */
        static bool unique(const std::shared_ptr<step>& step, const std::shared_ptr<struct vertex>& vertex);

        /**
         * @brief Factory: allocates a new step.
         * @param vertex Vertex the step lands on.
         * @param predecessor Step this one extends (may be @c nullptr for a root).
         * @param distance Heuristic distance to the target.
         * @return A shared pointer to the freshly created step.
         */
        static std::shared_ptr<step> make(const std::shared_ptr<struct vertex>& vertex,
                                          const std::shared_ptr<step>& predecessor, const double& distance);

        /**
         * @brief Registers @p ss as a child of @p s.
         * @param s Parent step.
         * @param ss Child step.
         */
        static void add_successor(const std::shared_ptr<step>& s, const std::shared_ptr<step>& ss);
    };

} // geoma

#endif //STEP_HPP
