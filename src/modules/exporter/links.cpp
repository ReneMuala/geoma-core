//
// Created by dte on 10/22/2025.
//

#include "links.hpp"
#include "fmt/core.h"
#include <fstream>
#include <iostream>
#include "modules/core/counter.hpp"

/**
 * @brief See links.hpp.
 *
 * Writes one text line per directed edge in "{ x = .., y = .. } -> { .. }"
 * form. Note that unlike the other exporters the stream is not verified
 * before writing; if the file could not be opened the dump is silently
 * skipped (an error line is printed) and true is still returned.
 */
bool geoma::exporter::links::export_(const std::list<std::shared_ptr<geoma::core::vertex>>& world,
                                   const std::string& filename)
{
    if (world.empty())
    {
        std::cout << '[' << __FUNCTION__ << " cannot export empty world]" << std::endl;
        return false;
    }
    std::ofstream out(filename);
    if (not out.is_open())
    {
        std::cout << '[' << __FUNCTION__ << " failed to open file " << filename << "]" << std::endl;
    }
    for (const auto& v : world)
    {
        for (const auto& n : v->neighbors)
        {
            out << fmt::format("{} -> {}\n",std::string(*v), std::string(*n));
        }
    }
    return true;
}
