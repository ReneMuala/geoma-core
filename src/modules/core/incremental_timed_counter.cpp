//
// Created by dte on 10/22/2025.
//

#include "incremental_timed_counter.hpp"

/** @brief Initializes the base limit and starts the embedded time budget. */
geoma::core::incremental_timed_counter::incremental_timed_counter(const unsigned long long& limit,
    const std::chrono::duration<long long>& duration):
    incremental_counter(limit), _timed_counter(timed_counter::make(duration))
{
}

/**
 * @brief True when either budget is exhausted: the solution limit or the deadline.
 */
bool geoma::core::incremental_timed_counter::done()
{
    return incremental_counter::done() or _timed_counter->done();
}

/** @brief Only the incremental part counts solutions; time flows on its own. */
void geoma::core::incremental_timed_counter::count()
{
    return incremental_counter::count();
}

/** @brief See incremental_timed_counter.hpp. */
std::shared_ptr<geoma::core::counter> geoma::core::incremental_timed_counter::make(const unsigned long long& limit,
    const std::chrono::duration<long long>& duration)
{
    return std::make_shared<incremental_timed_counter>(limit, duration);
}
