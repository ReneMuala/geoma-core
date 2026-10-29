//
// Created by dte on 10/22/2025.
//

#include "svg.hpp"
#include "fmt/core.h"
#include <fstream>
#include <iostream>
#include "modules/core/counter.hpp"

/**
 * @brief See svg.hpp.
 *
 * The bounding box is computed over every vertex *and* every neighbor
 * (neighbors may lie outside the world list), the content is translated
 * so the box starts at (0, 0), and the canvas is sized from the box
 * diagonal times @p zoom. Styling lives in an embedded <style> block:
 * orange lines that highlight cyan on hover, blue dots with coordinate
 * labels — useful for eyeballing a world in a browser.
 */
bool geoma::exporter::svg::export_(const std::list<std::shared_ptr<geoma::core::vertex>>& world,
                                   const std::string& filename, const double zoom)
{
    if (world.empty())
    {
        std::cout << '[' << __FUNCTION__ << " cannot export empty world]" << std::endl;
        return false;
    }
    // Axis-aligned bounding box over all vertices and their neighbors.
    double tlx = world.front()->x, tly = world.front()->y;
    double brx = world.front()->x, bry = world.front()->y;
    for (const auto& v : world)
    {
        if (v->x < tlx)
            tlx = v->x;
        else if (v->x > brx)
            brx = v->x;
        if (v->y < tly)
            tly = v->y;
        else if (v->y > bry)
            bry = v->y;
        for (const auto& vv : v->neighbors)
        {
            if (vv->x < tlx)
                tlx = vv->x;
            else if (vv->x > brx)
                brx = vv->x;
            if (vv->y < tly)
                tly = vv->y;
            else if (vv->y > bry)
                bry = vv->y;
        }
    }
    // Canvas side length: diagonal of the box, scaled by zoom.
    auto size = geoma::core::vertex::distance(geoma::core::vertex::make(tlx, tly), geoma::core::vertex::make(brx, bry));
    if (size == 0.0)
    {
        std::cout << '[' << __FUNCTION__ << " cannot export world with size = 0]" << std::endl;
        return false;
    }
    std::ofstream out(filename);
    if (not out.is_open())
    {
        std::cout << '[' << __FUNCTION__ << " failed to open file " << filename << "]" << std::endl;
        return false;
    }
    size *= zoom;
    out << fmt::format(
        R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {0} {0}" width="{0}" height="{0}"><style>line {{ stroke: #ff7400; stroke-linecap:round;}} line:hover {{ stroke: #00ffff;  }} circle {{ fill: blue; }} </style> )SVG",
        std::floor(size));
    // One <line> per directed edge, one labeled dot per vertex.
    for (const auto& v : world)
    {
        for (const auto& n : v->neighbors)
        {
            out << fmt::format(R"(<line x1="{}" y1="{}" x2="{}" y2="{}" />)""\n",
                                      (v->x - tlx) * zoom,
                                      (v->y - tly) * zoom,
                                      (n->x - tlx) * zoom,
                                      (n->y - tly) * zoom
            );
        }
        out << fmt::format(R"(<circle cx="{0}" cy="{1}" r="1" /> <text x="{0}" y="{1}" fill="blue" font-size="2">{2}</text>)""\n",
                                   (v->x - tlx) * zoom,
                                   (v->y - tly) * zoom,
                                   v->to_simple_string() + " "+ std::to_string(v->neighbors.size())
         );
    }
    out << "</svg>";
    std::cout << '[' << __FUNCTION__ << " dimension = " << std::ceil(size) << "]" << std::endl;
    return true;
}
