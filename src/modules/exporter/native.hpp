//
// Created by dte on 10/22/2025.
//

#pragma once
#include <list>
#include <memory>
#include <string>
#include "modules/core/vertex.hpp"

/**
 * @namespace geoma::exporter::native
 * @brief Serializes worlds to geoma's compact binary format.
 */
namespace geoma::exporter::native
{
    /**
     * @brief Writes the world to a binary file.
     *
     * The layout is: an 8-byte vertex count, then the @c x/@c y doubles of
     * every vertex in list order, then one 8-byte index pair per directed
     * edge, and finally the vertex count again as a footer sentinel.
     *
     * @param world The vertices to serialize.
     * @param filename Path of the binary file to write.
     * @return True on success, false when the world is empty, the file
     *         cannot be opened, or a "ghost" vertex (neighbor outside the
     *         world list) is detected.
     */
    bool export_(const std::list<std::shared_ptr<geoma::core::vertex>> & world,const std::string & filename);
}
