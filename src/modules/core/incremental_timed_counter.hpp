//
// Created by dte on 10/22/2025.
//

#pragma once
#include <iostream>
#include <math.h>
#include <memory>
#include <chrono>
#include "incremental_counter.hpp"
#include "timed_counter.hpp"

namespace geoma::core
{

    /**
     * @brief A hybrid counter: stops after N solutions *or* when time is up.
     *
     * Combines an @ref incremental_counter (its base) with an internal
     * @ref timed_counter. The search stops as soon as either budget is
     * exhausted, making this the safest default for interactive use.
     */
    struct incremental_timed_counter : public incremental_counter
    {
        /** @brief The embedded time budget. */
        std::shared_ptr<counter> _timed_counter;

        /**
         * @brief Builds a counter with both a solution limit and a deadline.
         * @param limit Maximum number of solutions.
         * @param duration Time budget; the clock starts at construction.
         */
        incremental_timed_counter(const unsigned long long& limit, const std::chrono::duration<long long>& duration);

        /**
         * @brief Checks both budgets.
         * @return True when the solution limit is reached <b>or</b> the
         *         deadline has passed.
         */
        bool done() override;

        /**
         * @brief Records one more solution (delegates to the incremental part).
         */
        void count() override;

        /**
         * @brief Factory: allocates a hybrid counter.
         * @param limit Maximum number of solutions.
         * @param duration Time budget for the search.
         * @return The counter, typed as its @ref counter interface.
         */
        static std::shared_ptr<counter> make(const unsigned long long& limit,
                                             const std::chrono::duration<long long>& duration);
    };

} // geoma

