//
// Created by dte on 10/22/2025.
//

#pragma once

#include <iostream>
#include <math.h>
#include <memory>
#include <chrono>
#include "counter.hpp"

namespace geoma::core
{
    /**
     * @brief A counter that stops the search after a wall-clock deadline.
     *
     * The clock starts at construction time; once @p duration has elapsed
     * @c done() reports true. Useful for very large worlds where any
     * answer now beats a perfect answer later.
     */
    struct timed_counter : public counter
    {
        /** @brief Construction timestamp used as the search start time. */
        std::chrono::time_point<std::chrono::high_resolution_clock> start;
        /** @brief How long the search may run. */
        std::chrono::duration<long long> duration;

        /**
         * @brief Builds a counter and starts its clock immediately.
         * @param duration Time budget granted to the search.
         */
        explicit timed_counter(const std::chrono::duration<long long>& duration);

        /**
         * @brief Checks whether the time budget has elapsed.
         * @return True once @p duration has passed since construction.
         * @note The comparison is truncated to whole seconds.
         */
        bool done() override;

        /**
         * @brief No-op: time-based counters do not track solutions.
         */
        void count() override;

        /**
         * @brief Factory: allocates a timed counter.
         * @param duration Time budget for the search.
         * @return The counter, typed as its @ref counter interface.
         */
        static std::shared_ptr<counter> make(const std::chrono::duration<long long>& duration);
    };
} // geoma
