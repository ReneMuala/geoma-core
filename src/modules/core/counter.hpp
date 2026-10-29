//
// Created by dte on 10/22/2025.
//

#pragma once

/**
 * @namespace geoma::core
 * @brief (continued) Budget counters for the navigator.
 */

namespace geoma::core
{
    /**
     * @brief Abstract budget for limiting the navigator's work.
     *
     * The navigator polls @c done() constantly and calls @c count() every
     * time it records a solution. Concrete implementations decide what
     * "done" means: a fixed number of solutions (@ref incremental_counter),
     * a wall-clock deadline (@ref timed_counter), or both
     * (@ref incremental_timed_counter).
     */
    struct counter
    {
        virtual ~counter() = default;

        /**
         * @brief Reports whether the budget is exhausted.
         * @return True when the search should stop.
         */
        virtual bool done() = 0;

        /**
         * @brief Notifies the counter that a solution was found.
         */
        virtual void count() = 0;
    };
}