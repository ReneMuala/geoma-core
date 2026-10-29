//
// Created by dte on 10/22/2025.
//

#include "native.hpp"
#include "fmt/core.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

/**
 * @brief See native.hpp.
 *
 * Binary layout (all values little-endian as written by the host):
 *   - uint64            vertex count N (header)
 *   - N × (double x, double y)   vertex coordinates in list order
 *   - one (uint64, uint64) pair per directed edge: source and destination
 *     indices into the vertex sequence
 *   - uint64            vertex count N again (footer sentinel)
 *
 * Every vertex pointer is mapped to its index first, so edges can be
 * written as stable indices instead of addresses.
 */
bool geoma::exporter::native::export_(const std::list<std::shared_ptr<geoma::core::vertex>>& world,
                                      const std::string& filename)
{
    unsigned long long size = 0;
    if (world.empty())
    {
        std::cout << '[' << __FUNCTION__ << " cannot export empty world]" << std::endl;
        return false;
    }
    std::ofstream out(filename,  std::ios::binary);
    if (not out.is_open())
    {
        std::cout << '[' << __FUNCTION__ << " failed to open file " << filename << "]" << std::endl;
        return false;
    }
    // Header: number of vertices that follow.
    const unsigned long long _size = world.size();
    out.write(reinterpret_cast<const char*>(&_size), sizeof(_size));
    size+=sizeof(_size);
    // Pass 1: write all coordinates and record vertex pointer -> index.
    unsigned long long index = 0;
    std::list<std::shared_ptr<geoma::core::vertex>> _;
    std::unordered_map<unsigned long long, unsigned long long> mapping;
    for (const auto& v : world)
    {
        size += sizeof(v->x)*2;
        out.write(reinterpret_cast<const char*>(&v->x), sizeof(v->x));
        out.write(reinterpret_cast<const char*>(&v->y), sizeof(v->y));
        mapping.emplace((unsigned long long)v.get(), index++);
    }
    // Checkpoint
    out.write(reinterpret_cast<const char*>(&_size), sizeof(_size));
    size+=sizeof(_size);
    // Pass 2: write every directed edge as a pair of vertex indices.
    for (const auto& v : world)
    {
        for (const auto& n : v->neighbors)
        {
            // A neighbor outside the world list cannot be serialized.
            if (not mapping.contains((unsigned long long)v.get()) or not mapping.contains((unsigned long long)n.get()))
            {
                std::cerr << (unsigned long long)v.get() << " " <<  mapping.contains((unsigned long long)v.get()) << std::endl;
                std::cerr << (unsigned long long)n.get() << " " <<  mapping.contains((unsigned long long)n.get()) << std::endl;
                std::cerr << "["<< __FUNCTION__ << " detected ghost vertex, breaking operation]" << std::endl;
                return false;
            }
            auto _v = mapping.at((unsigned long long)v.get());
            auto _n = mapping.at((unsigned long long)n.get());
            out.write(reinterpret_cast<const char*>(&_v), sizeof(_v));
            out.write(reinterpret_cast<const char*>(&_n), sizeof(_n));
            size += sizeof(_v)*2;
        }
    }
    // Footer: repeat the vertex count as a simple integrity sentinel.
    size+=sizeof(size);
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    std::cout << '[' << __FUNCTION__ << " size = " << size << " byte(s)]" << std::endl;
    return true;
}
