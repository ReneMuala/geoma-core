//
// Created by dte on 10/22/2025.
//

#include "osm.hpp"
#include <rapidxml/rapidxml.hpp>
#include <unordered_map>
#include <iostream>


#include "modules/misc/thread_pool.hpp"

namespace geoma::importer::private_
{
    extern std::string read_file(const std::string& file_name);
}

/**
 * @brief Reads an XML attribute value as a string.
 * @param node Element to read from.
 * @param name Attribute name.
 * @return The attribute's value, or an empty string when absent.
 */
static std::string get_attribute(const rapidxml::xml_node<>* node, const std::string& name)
{
    auto n = node->first_attribute(name.c_str());
    if (not n) return "";
    return n->value();
}

/**
 * @brief See osm.hpp.
 *
 * Implementation notes:
 *  - Nodes and ways are processed on two separate thread pools (a new pool
 *    is created when the first <way> appears). A shared mutex guards the
 *    vertex map, since ways look up node ids created by the node tasks.
 *  - Nodes with action="delete" (OSM changefiles) are skipped.
 *  - A way only contributes edges when it carries a <tag k="highway">.
 *  - Each way links its consecutive <nd ref=...> nodes with two-way edges.
 *  - Vertices without any neighbor are dropped at the end, so only
 *    vertices that actually belong to a highway survive.
 */
std::list<std::shared_ptr<geoma::core::vertex>> geoma::importer::osm::import(const std::string& filename)
{
    std::unordered_map<std::string, std::shared_ptr<geoma::core::vertex>> vertices;
    std::cout << '[' << __FUNCTION__ << " filename = " << filename << ']' << std::endl;
    rapidxml::xml_document<> doc;
    std::cout << '[' << __FUNCTION__ << " reading file]" << std::endl;
    auto content = private_::read_file(filename);
    std::cout << '[' << __FUNCTION__ << " reading complete]" << std::endl;
    std::cout << '[' << __FUNCTION__ << " parsing]" << std::endl;
    doc.parse<0>(content.data());
    std::cout << '[' << __FUNCTION__ << " parsing complete]" << std::endl;
    auto n = doc.first_node();
    if (n) n = n->first_node();
    // Pool for <node> elements; a second pool is created for <way> elements
    // so that way-linking tasks (which look up finished nodes) are not
    // starved by node-parsing tasks.
    auto pool = std::make_shared<geoma::misc::thread_pool>();
    std::shared_ptr<geoma::misc::thread_pool> pool2;
    std::mutex mutex;
    while (n)
    {
        if (get_attribute(n, "action") != "delete")
        {
            if (n->name() == std::string("node"))
            {
                // Nodes are independent: parse lat/lon on the node pool.
                pool->enqueue([&vertices,&mutex,n]()
                {
                    const double lat = std::strtod(get_attribute(n, "lat").c_str(), nullptr);
                    const double lon = std::strtod(get_attribute(n, "lon").c_str(), nullptr);
                    std::string id = get_attribute(n, "id");
                    std::unique_lock<std::mutex> lock(mutex);
                    vertices.emplace(id, geoma::core::vertex::make(lat, lon));
                });
            }
            else if (n->name() == std::string("way"))
            {
                // Switch to the way pool from the first <way> onward.
                if (pool)
                    pool = nullptr;
                if (not pool2)
                    pool2 = std::make_shared<geoma::misc::thread_pool>();
                pool2->enqueue([&vertices,&mutex,n]()
                {
                    bool is_highway = false;
                    // Consecutive (ref, ref) pairs of the way's <nd> children.
                    std::list<std::pair<std::string, std::string>> links;
                    rapidxml::xml_node<>* lsn = nullptr;
                    for (auto sn = n->first_node(); sn; sn = sn->next_sibling())
                    {
                        if (sn->name() == std::string("nd"))
                        {
                            if (lsn != nullptr)
                            {
                                auto lid = get_attribute(lsn, "ref");
                                auto id = get_attribute(sn, "ref");
                                links.emplace_back(lid, id);
                            }
                            lsn = sn;
                        }
                        else if (sn->name() == std::string("tag") && get_attribute(sn, "k") == "highway")
                        {
                            is_highway = true;
                        }
                    }

                    // Highways only: link every consecutive node pair in both
                    // directions so the graph models two-way roads.
                    if (is_highway)
                    {
                        for (auto& [fst, snd] : links)
                        {
                            std::shared_ptr<geoma::core::vertex> a;
                            std::shared_ptr<geoma::core::vertex> b;
                            {
                                std::unique_lock<std::mutex> lock(mutex);
                                a = vertices.at(fst);
                                b = vertices.at(snd);
                            }
                            if (a != nullptr && b != nullptr)
                            {
                                geoma::core::vertex::join(a, b);
                                geoma::core::vertex::join(b, a);
                            }
                        }
                    }
                });
            }
        }
        n = n->next_sibling();
    }
    // Releasing pool2 joins its workers: all way-linking tasks are done here.
    pool2 = nullptr;
    // Drop vertices that no highway ever referenced.
    std::list<std::shared_ptr<geoma::core::vertex>> valid_vertices;
    for (auto& [fst, snd] : vertices)
    {
        if (not snd->neighbors.empty())
        {
            valid_vertices.push_back(snd);
        }
    }
    const auto all = vertices.size();
    vertices.clear();
    const auto valid = valid_vertices.size();
    std::cout << '[' << __FUNCTION__ << " " << valid << " vertices loaded; " << all - valid << " discarded]" <<
        std::endl;
    return std::move(valid_vertices);
};
