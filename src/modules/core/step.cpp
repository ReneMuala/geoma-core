//
// Created by dte on 10/22/2025.
//

#include "step.hpp"

/** @brief Walks the predecessor chain to the root, printing each vertex. */
void geoma::core::step::print(const std::shared_ptr<step>& step)
{
    for (auto item = step; item != nullptr; item = item->predecessor)
    {
        std::cout << item->vertex->to_simple_string() << " (" << item->distance << ") <- ";
    }
    std::cout << std::endl;
}

/**
 * @brief See step.hpp.
 * @note Result is ordered leaf-to-root: the target comes first, the origin last.
 */
std::list<std::shared_ptr<struct geoma::core::vertex>> geoma::core::step::walkthrough(const std::shared_ptr<step>& step)
{
    std::list<std::shared_ptr<struct geoma::core::vertex>> result;
    for (auto item = step; item != nullptr; item = item->predecessor)
    {
        result.push_back(item->vertex);
    }
    return result;
}

/**
 * @brief See step.hpp.
 * @note Pointer-identity comparison: a vertex equal by coordinates but
 *       distinct in memory is still considered unique.
 */
bool geoma::core::step::unique(const std::shared_ptr<step>& step, const std::shared_ptr<struct geoma::core::vertex>& vertex)
{
    for (auto item = step; item != nullptr; item = item->predecessor)
    {
        if (item->vertex == vertex)
        {
            return false;
        }
    }
    return true;
}

/** @brief See step.hpp. */
std::shared_ptr<geoma::core::step> geoma::core::step::make(const std::shared_ptr<struct geoma::core::vertex>& vertex,
    const std::shared_ptr<step>& predecessor, const double& distance)
{
    return std::make_shared<step>(vertex, predecessor, distance);
}

/** @brief See step.hpp. */
void geoma::core::step::add_successor(const std::shared_ptr<step>& s, const std::shared_ptr<step>& ss)
{
    s->successors.push_back(ss);
}
