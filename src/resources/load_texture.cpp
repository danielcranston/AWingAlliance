#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

#include "resources/locator.h"
#include "resources/load_texture.h"
#include "resources/texture_data.h"

using resources::locator::TEXTURES_PATH;

namespace resources
{
TextureData load_texture(const std::string& filename)
{
    int w, h;
    int comp;

    const auto texture_path = std::string(TEXTURES_PATH) + filename;
    unsigned char* image = stbi_load(texture_path.c_str(), &w, &h, &comp, STBI_default);
    if (!image)
    {
        std::cerr << "Unable to load texture: " << texture_path << std::endl;
        throw std::runtime_error("Unable to load texture");
    }
    // std::cout << "Loading \"" << filename << "\", w=" << w << ", h=" << h << ", comp=" << comp
    //           << std::endl;

    // Wasteful to copy, but rather that than dealing with raw unowned pointers
    const std::vector<unsigned char> buffer{ image, image + w * h * comp };

    stbi_image_free(image);

    return TextureData(filename, std::move(buffer), w, h, comp);
}

std::array<TextureData, 6> load_cubemap_texture(const std::string& folder_name)
{
    std::array<std::string, 6> filenames = { "/right.png", "/left.png",  "/top.png",
                                             "/bot.png",   "/front.png", "/back.png" };
    return { load_texture(folder_name + filenames[0]), load_texture(folder_name + filenames[1]),
             load_texture(folder_name + filenames[2]), load_texture(folder_name + filenames[3]),
             load_texture(folder_name + filenames[4]), load_texture(folder_name + filenames[5]) };
}
}  // namespace resources
