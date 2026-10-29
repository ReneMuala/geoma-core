//
// Created by dte on 11/26/2026.
//

#pragma once
#include <list>
#include <memory>
#include <string>
#include "modules/core/vertex.hpp"

/**
 * @namespace geoma::importer::native
 * @brief Deserializes geoma's compact binary to world format.
 */
namespace geoma::importer::native
{
    std::list<std::shared_ptr<geoma::core::vertex>> import(const std::string & filename);
}
