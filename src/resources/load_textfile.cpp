#include <fstream>

#include "resources/load_textfile.h"

namespace resources
{
const std::string load_textfile(const std::string& filename, const std::string folder_path)
{
    std::string full_path = folder_path + filename;
    std::ifstream ifs(full_path);
    if (!ifs.good())
        throw std::runtime_error("Could not open " + full_path);

    return { (std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()) };
}
}  // namespace resources