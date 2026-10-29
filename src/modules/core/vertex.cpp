//
// Created by dte on 10/22/2025.
//

#include "vertex.hpp"
#include <cmath>

#include <sstream>

/**
 * @brief Formats a floating-point value with fixed precision.
 * @tparam T A floating-point type.
 * @param a_value Value to format.
 * @param n Number of decimals (defaults to 14 for near-full double precision).
 * @return The fixed-notation string representation.
 */
namespace geoma
{
    template <typename T>
static std::string to_string(const T a_value, const int n = 14)
    {
        std::ostringstream out;
        out.precision(n);
        out << std::fixed << a_value;
        return std::move(out).str();
    }
}

/**
 * @brief Renders the vertex as "{ x = <x>, y = <y> }".
 */
geoma::core::vertex::operator geoma::core::string() const
{
    return "{ x = " + geoma::to_string(x) + ", y = " + geoma::to_string(y) + "}";
}

std::string geoma::core::vertex::to_simple_string() const {
    return geoma::to_string(x) + "," + geoma::to_string(y);
}

/** @brief See vertex.hpp: plain Euclidean distance. */
double geoma::core::vertex::distance(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb)
{
    return sqrt(pow(va->x - vb->x, 2) + pow(va->y - vb->y, 2));
}

/** @brief See vertex.hpp. */
std::shared_ptr<geoma::core::vertex> geoma::core::vertex::make(double x, double y)
{
    return std::make_shared<vertex>(x, y);
}

/**
 * @brief See vertex.hpp.
 * @note One-directional: only @p va learns about @p vb.
 */
void geoma::core::vertex::join(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb)
{
    va->neighbors.push_back(vb);
}

/** @brief See vertex.hpp: linear scan over @p va's neighbors. */
bool geoma::core::vertex::joined(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb)
{
    for (const auto& v : va->neighbors)
    {
        if (equals(v, vb))
        {
            return true;
        }
    }
    return false;
}

/** @brief See vertex.hpp: exact comparison of both coordinates. */
bool geoma::core::vertex::equals(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb)
{
    return va->x == vb->x && va->y == vb->y;
}

/** @brief See vertex.hpp. */
bool geoma::core::vertex::lower(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb)
{
    return va->x < vb->x or va->y < vb->y;
}

/** @brief See vertex.hpp. */
bool geoma::core::vertex::greater(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb)
{
    return va->x > vb->x or va->y > vb->y;
}

/**
 * @brief See vertex.hpp.
 * @note Linear scan tracking the running minimum; the first vertex wins ties.
 */
std::shared_ptr<geoma::core::vertex> geoma::core::vertex::closest(const std::shared_ptr<vertex>& v,
                                                                  const std::list<std::shared_ptr<vertex>>& world)
{
    if (world.empty())
        return nullptr;
    std::shared_ptr<vertex> closest = world.front();
    double d = distance(v, closest);
    for (auto& wv : world)
    {
        const double d2 = distance(v, wv);
        if (d2 < d)
        {
            closest = wv;
            d = d2;
        }
    }
    return closest;
}
