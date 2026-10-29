//
// Created by dte on 10/22/2025.
//

#include "incremental_counter.hpp"

/** @brief See incremental_counter.hpp. */
geoma::core::incremental_counter::incremental_counter(const unsigned long long limit): limit(limit)
{
}

/** @brief True once the number of counted solutions reaches the limit. */
bool geoma::core::incremental_counter::done()
{
    return current >= limit;
}

/** @brief Advances the solution count by one. */
void geoma::core::incremental_counter::count()
{
    current++;
}

/** @brief See incremental_counter.hpp. */
std::shared_ptr<geoma::core::counter> geoma::core::incremental_counter::make(unsigned long long limit)
{
    return std::make_shared<incremental_counter>(limit);
}
