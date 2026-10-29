//
// Created by dte on 10/22/2025.
//
#pragma once
#include <list>
#include <memory>
#include <string>
#include "modules/core/vertex.hpp"

/**
 * @namespace geoma::exporter::links
 * @brief Dumps worlds as human-readable edge lists.
 */
namespace geoma::exporter::links
{
    /**
     * @brief Writes every directed edge of the world as a text line.
     *
     * Each line has the form @c "{ x = .., y = .. } -> { x = .., y = .. }",
     * one per entry in a vertex's neighbors list. Handy for debugging,
     * diffing and feeding the graph to other tools.
     *
     * @param world The vertices to dump.
     * @param filename Path of the text file to write.
     * @return True on success, false when the world is empty or the file
     *         cannot be opened (the dump is skipped in that case).
     */
    bool export_(const std::list<std::shared_ptr<geoma::core::vertex>> & world,const std::string & filename);
}
