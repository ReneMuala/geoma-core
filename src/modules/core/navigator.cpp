//
// Created by dte on 10/22/2025.
//

#include "navigator.hpp"
#include "modules/core/step.hpp"

#include <cstddef>
#include <fmt/base.h>

/** @brief See navigator.hpp. */
std::shared_ptr<geoma::core::navigator> geoma::core::navigator::make(const std::shared_ptr<geoma::core::vertex>& origin,
                                                                     const std::shared_ptr<geoma::core::vertex>& target)
{
    return std::make_shared<navigator>(origin, target);
}

/**
 * @brief Frontier ordering: steps closer to the target sort first.
 *
 * Used to sort each new level of the search frontier so the most
 * promising branches are expanded before the budget runs out.
 */
static bool local_sort(const std::shared_ptr<geoma::core::step>& a, const std::shared_ptr<geoma::core::step>& b)
{
    return a->distance < b->distance;
}

/**
 * @brief See navigator.hpp.
 *
 * Search outline:
 *  1. Snap origin and target to their closest world vertices (ocv / tcv).
 *  2. Seed the frontier with a step on the origin followed by one on ocv.
 *  3. Level by level, expand every frontier step's neighbors that are not
 *     already in the step's chain (cycle guard via step::unique).
 *  4. A step landing on tcv completes a route: it is closed with a final
 *     step on the exact target, recorded via walkthrough(), and counted.
 *  5. The next frontier is re-sorted by remaining distance each level.
 *  6. Stop when the counter's budget is exhausted or no steps remain.
 */
void geoma::core::navigator::navigate(const std::shared_ptr<navigator>& nav,const std::list<std::shared_ptr<vertex>>& world,
    const std::shared_ptr<counter>& counter, std::list<std::list<std::shared_ptr<vertex>>>& solutions, const flags &flags)
{
    const auto ocv = vertex::closest(nav->origin, world);
    const auto tcv = vertex::closest(nav->target, world);
    if (not ocv and not tcv)
    {
        return;
    }
    std::cout << '[' << __FUNCTION__ << " o = " << std::string(*nav->origin) << ']' << std::endl;
    std::cout << '[' << __FUNCTION__ << " ocv = " << std::string(*ocv) << ']' << std::endl;
    std::cout << '[' << __FUNCTION__ << " t = " << std::string(*nav->target) << ']' << std::endl;
    std::cout << '[' << __FUNCTION__ << " tcv = " << std::string(*tcv) << ']' << std::endl;

    std::list<std::shared_ptr<step>> steps;
    std::shared_ptr<step> st = step::make(nav->origin, nullptr, 0);
    {
        const auto _st1 = step::make(ocv, st, 0);
        step::add_successor(st, _st1);
        st = _st1;
    }
    steps.push_back(st);
    while (not counter->done())
    {
        if (steps.empty()) break;
        std::list<std::shared_ptr<step>> next_steps;
        for (auto& s : steps)
        {
            if (counter->done()) break;
            if (vertex::equals(s->vertex, tcv))
            {
                const auto final_step = step::make(nav->target, s, 0);
                solutions.push_back(step::walkthrough(final_step));
                counter->count();
                if (counter->done()) return;
            }
            else
            {
                for (auto& c : s->vertex->neighbors)
                {
                    if (counter->done()) break;
                    if (step::unique(s, c))
                    {
                        next_steps.push_back(step::make(c, s, vertex::distance(c, tcv)));
                    }
                }
            }
        }
        if (counter->done()) break;
        steps.swap(next_steps);
        steps.sort(local_sort);
        if(flags.next_steps_limit and steps.size() > flags.next_steps_limit.value()){
            steps.resize(flags.next_steps_limit.value());
        }
    }
#if 0
    if(not steps.empty()){
        auto lt = steps.front();
        for(auto v : lt->vertex->neighbors){
            fmt::println("> {}", v->to_simple_string());
        }
        fmt::println("lt");
        step::print(lt);
    }
#endif
}
