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
     * @brief A counter that stops the search after a fixed number of solutions.
     *
     * Every call to @c count() advances @p current; once it reaches
     * @p limit, @c done() reports true and the navigator stops.
     */
    struct incremental_counter : public counter
    {
        /** @brief Maximum number of solutions allowed. */
        unsigned long long limit = 0;
        /** @brief Number of solutions counted so far. */
        unsigned long long current = 0;

        /**
         * @brief Builds a counter with the given solution limit.
         * @param limit Maximum number of @c count() calls before @c done().
         */
        explicit incremental_counter(const unsigned long long limit);

        /**
         * @brief Checks whether the solution limit has been reached.
         * @return True when @p current >= @p limit.
         */
        bool done() override;

        /**
         * @brief Records one more solution.
         */
        void count() override;

        /**
         * @brief Factory: allocates an incremental counter.
         * @param limit Maximum number of solutions.
         * @return The counter, typed as its @ref counter interface.
         */
        static std::shared_ptr<counter> make(unsigned long long limit);
    };
}