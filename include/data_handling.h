#pragma once

#include <fstream>
#include <filesystem>

namespace data_handling
{
constexpr const char* ROOT_PATH = "/home/daniel/Documents/new/data/";
constexpr const char* SHADERS_PATH = "/home/daniel/Documents/new/data/shaders/";
constexpr const char* MODELS_PATH = "/home/daniel/Documents/new/data/models/";
constexpr const char* TEXTURES_PATH = "/home/daniel/Documents/new/data/textures/";
constexpr const char* SOUNDS_PATH = "/home/daniel/Documents/new/data/sounds/";
constexpr const char* URDF_PATH = "/home/daniel/Documents/new/data/urdf/";

inline std::string load_textfile(const std::string& filename,
                                 const std::string folder_path = ROOT_PATH)
{
    std::string full_path = folder_path + filename;
    std::ifstream ifs(full_path);
    if (!ifs.good())
        throw std::runtime_error("Could not open " + full_path);

    return { (std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()) };
}

}  // namespace data_handling
