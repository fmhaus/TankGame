#pragma once

#include "engine/Types.h"

#include <memory>
#include <fstream>

namespace FileUtil
{
    /// Loads a binary file into a u8[] buffer; throws exception on error
    static std::unique_ptr<u8[]> load_file(const char* file_name, u64& file_size)
    {
        std::ifstream file(file_name, std::ios::binary | std::ios::ate);
        if (!file)
            throw std::runtime_error("Failed to open file: " + std::string(file_name));

        file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        auto buffer = std::make_unique<u8[]>(file_size);
        file.read(reinterpret_cast<char*>(buffer.get()), file_size);
        return buffer;
    }
}