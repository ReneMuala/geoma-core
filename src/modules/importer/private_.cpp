//
// Created by dte on 11/27/2025.
//
#include <fstream>
#include <iostream>
#include <string>
#include <fstream>

/**
 * @brief Shared low-level helpers for the importer module.
 *
 * Declared `extern` in the importer translation units; there is no public
 * header on purpose — nothing outside `geoma::importer` should depend on
 * these.
 */
namespace geoma::importer::private_
{
    /**
     * @brief Reads an entire file into memory.
     * @param file_name Path of the file to read.
     * @return The file contents, or an empty string when the file cannot
     *         be opened.
     * @note Prints the file size and warns when the input exceeds ~1 GB,
     *       since the whole file is held in memory at once.
     */
    std::string read_file(const std::string& file_name)
    {
        std::ifstream file(file_name, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            std::cout << '[' << __FUNCTION__ << " failed to open file = " << file_name << ']' << std::endl;
            return std::string();
        }
        std::streamsize file_size = file.tellg(); // Get file size
        std::cout << '[' << __FUNCTION__ << " filesize = " << file_size << ']' << std::endl;
        if (file_size > static_cast<long long>(1e9))
        {
            std::cout << '[' << __FUNCTION__ <<
                " warning = this is a relatively big file, you should assure that enough memory is available before this operation]"
                << std::endl;
        }
        file.seekg(0, std::ios::beg);

        std::string file_contents;
        file_contents.reserve(file_size);
        file.read(file_contents.data(), file_size);
        return file_contents;
    }
}