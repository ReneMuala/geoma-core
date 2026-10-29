//
// Created by dte on 10/22/2025.
//

#include "timed_counter.hpp"

/** @brief Starts the countdown immediately: construction time is the deadline origin. */
geoma::core::timed_counter::timed_counter(const std::chrono::duration<long long>& duration): duration(duration)
{
    start = std::chrono::high_resolution_clock::now();
}

/**
 * @brief True once the elapsed time (truncated to seconds) reaches the budget.
 */
bool geoma::core::timed_counter::done()
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).
        count() >= std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

/** @brief No-op: solutions are not tracked by a pure time budget. */
void geoma::core::timed_counter::count()
{
}

/** @brief See timed_counter.hpp. */
std::shared_ptr<geoma::core::counter> geoma::core::timed_counter::make(const std::chrono::duration<long long>& duration)
{
    return std::make_shared<timed_counter>(duration);
}
