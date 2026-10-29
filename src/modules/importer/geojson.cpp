//
// Created by dte on 10/22/2025.
//

#include "geojson.hpp"
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <fmt/base.h>
#include <json/json.h>
#include "modules/misc/thread_pool.hpp"

namespace geoma::importer::private_
{
#define json_assert(c)\
    if (not (c))\
    {\
        std::cout << '[' << __FUNCTION__ << " json_assert failed = " << #c "]"<< std::endl;\
        return {};\
    }
    extern std::string read_file(const std::string& file_name);
}

/**
 * @brief See geojson.hpp.
 *
 * Implementation notes:
 *  - Vertices are deduplicated with a map keyed by the raw JSON text of
 *    "lon + lat" concatenated, so the same coordinate pair reused across
 *    features maps to a single shared vertex.
 *  - Coordinates are stored as x = lat, y = lon (GeoJSON order is
 *    [lon, lat]).
 *  - Consecutive coordinate pairs are joined in both directions, so any
 *    linear geometry becomes a two-way path through the graph.
 *  - extract_coords tolerates both flat ([lon, lat]) and nested
 *    ([[lon, lat], ...]) coordinate layouts: the loop calls it once per
 *    sub-array and once per element.
 */
std::list<std::shared_ptr<geoma::core::vertex>> geoma::importer::geojson::import(const std::string& filename)
{
    std::list<std::shared_ptr<geoma::core::vertex>> vertices;
    std::unordered_map<std::string,std::shared_ptr<geoma::core::vertex>> vertex_map;
    unsigned long long vertex_count = 0;
    unsigned long long connection_count = 0;
    std::cout << '[' << __FUNCTION__ << " filename = " << filename << ']' << std::endl;
    Json::Value geojson;
    Json::Reader reader;
    std::cout << '[' << __FUNCTION__ << " reading file]" << std::endl;
    const auto content = private_::read_file(filename);
    std::cout << '[' << __FUNCTION__ << " reading complete]" << std::endl;
    std::cout << '[' << __FUNCTION__ << " parsing]" << std::endl;
    if (not reader.parse(content.data(), geojson))
    {
        std::cout << '[' << __FUNCTION__ << " failed to parse file" << std::endl;
        return {};
    }
    std::cout << '[' << __FUNCTION__ << " parsing complete]" << std::endl;
    json_assert(geojson.isObject());
    const auto& features = geojson["features"];
    json_assert(features.isArray());

    /**
     * @brief Links one coordinate into the current geometry path.
     *
     * Reuses a vertex when its coordinate pair was seen before, otherwise
     * creates one. When @p vertex already points somewhere, the new vertex
     * is joined to it in both directions and becomes the new path head.
     */
    auto extract_coords = [&vertex_map, &vertex_count, &connection_count, &vertices](std::shared_ptr<core::vertex>& vertex, const Json::Value& sub_coordinate) -> void
    {
        if(sub_coordinate.isArray() and not sub_coordinate.empty() and sub_coordinate[0].isNumeric())
        {
            // Dedup key: raw text of "[lon][lat]" concatenated.
            const std::string key = sub_coordinate[0].asString()+sub_coordinate[1].asString();
            const auto it = vertex_map.find(key);
            const bool it_exists = it != vertex_map.end();
            const std::shared_ptr<core::vertex> new_vertex =  it_exists ? it->second : core::vertex::make(sub_coordinate[1].asDouble(), sub_coordinate[0].asDouble());
            if (not it_exists)
            {
                vertices.push_back(new_vertex);
            }
            vertex_count++;
            if (not vertex)
            {
                // First coordinate of the geometry: just set the path head.
                vertex = new_vertex;
            } else
            {
                // Extend the path: two-way edge with the previous coordinate.
                core::vertex::join(vertex, new_vertex);
                core::vertex::join(new_vertex, vertex);
                connection_count+=2;
                vertex = new_vertex;
            }
        }
    };


    for (const auto& feature : features)
    {
        std::shared_ptr<core::vertex> vertex;
        const auto& geometry = feature["geometry"];
        json_assert(geometry.isObject());
        const auto& coordinates = geometry["coordinates"];
        json_assert(coordinates.isArray());
        for (const auto& sub_coordinates : coordinates)
        {
            extract_coords(vertex, sub_coordinates);
            for (auto & sub_coordinate : sub_coordinates)
            {
                extract_coords(vertex, sub_coordinate);
            }
        }
    }

    fmt::println("[{} {} vertices loaded with {} connections]", __FUNCTION__,vertex_count, connection_count);
    return std::move(vertices);
};
