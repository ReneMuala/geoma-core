//
// Created by dte on 10/22/2025.
//

#include "native.hpp"
#include "fmt/base.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "modules/core/vertex.hpp"
#include <fstream>
#include <iostream>
#include <unordered_map>


#define native_assert(c)\
    if (not (c))\
    {\
        std::cout << '[' << __FUNCTION__ << " native_assert failed = " << #c "]"<< std::endl;\
        return {};\
    }

#define native_assert_message(c, m)\
        if (not (c))\
        {\
            std::cout << '[' << __FUNCTION__ << " native_assert failed = " << #c  << " description =" << m << "]"<< std::endl;\
            return {};\
        }

std::list<std::shared_ptr<geoma::core::vertex>> geoma::importer::native::import(const std::string& filename)
{
    unsigned long long size = 0;
    std::vector<std::shared_ptr<geoma::core::vertex>> vertices;
    std::ifstream in(filename,  std::ios::binary);
    native_assert_message(in.is_open(), fmt::format("failed to open file {}", filename));
    // Header: number of vertices that follow.
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    unsigned long long index = 0;
    for(auto _ = 0 ; _ < size; _++){
        double x;
        double y;
        native_assert_message(not in.read(reinterpret_cast<char*>(&x), sizeof(x)).eof(), "premature end of file");
        native_assert_message(not in.read(reinterpret_cast<char*>(&y), sizeof(y)).eof(), "premature end of file");
        vertices.push_back(geoma::core::vertex::make(x, y));
    }

    {
        unsigned long long checkpoint = 0;
        native_assert_message(not in.read(reinterpret_cast<char*>(&checkpoint), sizeof(checkpoint)).eof(), "premature end of file");
        native_assert(size == checkpoint);
    }
    unsigned long long join_count = 0;
    while(true) {
        unsigned long long first;
        unsigned long long neighbor;
        if(not in.read(reinterpret_cast<char*>(&first), sizeof(first)).eof()) {
            if(not in.read(reinterpret_cast<char*>(&neighbor), sizeof(neighbor)).eof()) {
                const auto vertex = first;
                native_assert(vertex < vertices.size());
                native_assert(neighbor < vertices.size());
                geoma::core::vertex::join(vertices.at(vertex), vertices.at(neighbor));
                join_count++;
            } else {
                const auto expected_size = first;
                constexpr auto HEADER_SIZE = (sizeof(unsigned long long));
                constexpr auto VERTEX_SIZE = (sizeof(double)*2);
                constexpr auto JOIN_SIZE = (sizeof(double)*2);
                // fmt::println("expected size = {},", expected_size);

                const auto actual_size = 3*HEADER_SIZE  + VERTEX_SIZE*vertices.size() + join_count * JOIN_SIZE;
                // fmt::println("{} == {}", expected_size , actual_size);
                native_assert(expected_size == actual_size);
                break;
            }
        }
    }
    return std::move(std::list<std::shared_ptr<geoma::core::vertex>>(std::make_move_iterator(vertices.begin()),
                       std::make_move_iterator(vertices.end())));
}
