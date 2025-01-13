#include <string>
#include <iostream>
#include <array>

#include <GL/glew.h>
#include "rendering/texture.h"
#include "data_handling.h"

#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

namespace rendering
{
namespace
{
struct LoadedTextureData
{
    /**
     * @brief RAII wrapper around stbi_load and stbi_image_free
     */
    LoadedTextureData(const std::string& uri)
    {
        std::cout << "Loading \"" << uri << "\" ..." << std::endl;

        const auto full_path = std::string(data_handling::TEXTURES_PATH) + uri;
        data = stbi_load(full_path.c_str(), &width, &height, &comp, STBI_default);

        if (!data)
        {
            std::cerr << "Unable to load texture: " << uri << std::endl;
            throw std::runtime_error("Unable to load texture");
        }

        if (comp != 3 && comp != 4)
        {
            throw std::runtime_error("Unsupported texture composition " + std::to_string(comp) +
                                     " for " + uri);
        }
    }

    ~LoadedTextureData()
    {
        stbi_image_free(data);
    }

    std::tuple<int, int, int> width_height_comp() const
    {
        return std::make_tuple(width, height, comp);
    }

    LoadedTextureData(LoadedTextureData&) = delete;
    LoadedTextureData(const LoadedTextureData&) = delete;
    LoadedTextureData& operator=(LoadedTextureData&) = delete;
    LoadedTextureData& operator=(const LoadedTextureData&) = delete;
    LoadedTextureData(LoadedTextureData&&) = delete;
    LoadedTextureData& operator=(LoadedTextureData&&) = delete;
    LoadedTextureData(const LoadedTextureData&&) = delete;
    LoadedTextureData& operator=(const LoadedTextureData&&) = delete;

    unsigned char* data;
    int width;
    int height;
    int comp;
};

unsigned int init_2d_texture(const LoadedTextureData& texture_data)
{
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 texture_data.comp == 3 ? GL_RGB : GL_RGBA,
                 texture_data.width,
                 texture_data.height,
                 0,
                 texture_data.comp == 3 ? GL_RGB : GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 texture_data.data);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture_id;
}

unsigned int init_cubemap_texture(const std::array<LoadedTextureData, 6>& data_vec)
{
    // https://learnopengl.com/Advanced-OpenGL/Cubemaps
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    for (std::size_t i = 0; i < data_vec.size(); ++i)
    {
        const auto& texdata = data_vec[i];

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     texdata.comp == 3 ? GL_RGB : GL_RGBA,
                     texdata.width,
                     texdata.height,
                     0,
                     texdata.comp == 3 ? GL_RGB : GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     texdata.data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture_id;
}

}  // namespace

Texture::Texture(const std::string& uri, const Texture::Type type) : uri(uri), type(type)
{
    if (type == Texture::Type::TEXTURE)
    {
        const auto texture_data = LoadedTextureData(uri);
        std::tie(width, height, comp) = texture_data.width_height_comp();
        texture_id = init_2d_texture(texture_data);
    }
    else if (type == Texture::Type::CUBEMAP)
    {
        std::array<LoadedTextureData, 6> texture_data_vec = {
            LoadedTextureData(uri + "/right.png"), LoadedTextureData(uri + "/left.png"),
            LoadedTextureData(uri + "/top.png"),   LoadedTextureData(uri + "/bot.png"),
            LoadedTextureData(uri + "/front.png"), LoadedTextureData(uri + "/back.png")
        };

        std::tie(width, height, comp) = texture_data_vec[0].width_height_comp();
        texture_id = init_cubemap_texture(texture_data_vec);
    }
    else
    {
        throw std::runtime_error("Unsupported texture type " +
                                 std::to_string(static_cast<int>(type)) + " for " + uri);
    }

    std::cout << "Texture(" << uri << " id=" << texture_id << " width=" << width
              << " height=" << height << " comp=" << comp << ") constructed" << std::endl;
}

Texture::Texture(Texture&& other)
  : uri(std::move(other.uri)),
    type(other.type),
    width(other.width),
    height(other.height),
    comp(other.comp),
    texture_id(other.texture_id)
{
    std::cout << "Texture \"" << uri << "\" (texID " << texture_id << ") being moved" << std::endl;
    other.is_owning = false;
}

Texture& Texture::operator=(Texture&& other)
{
    uri = other.uri;
    type = other.type;
    width = other.width;
    height = other.height;
    comp = other.comp;
    texture_id = other.texture_id;

    std::cout << "Texture \"" << uri << "\" (texID " << texture_id << ") being moved" << std::endl;

    // End others ownership of the texture, preventing it from releasing the vao/vbos in its
    // destructor.
    other.is_owning = false;

    return *this;
}

Texture::~Texture()
{
    if (is_owning)
    {
        std::cout << "Texture " << uri << " (id " << texture_id << ") being cleaned up"
                  << std::endl;
        glDeleteTextures(1, &texture_id);  // TODO: check if cleanup is different for cubemap
    }
    else
    {
        std::cout << "  (Moved-from Texture does not free any resources)" << std::endl;
    }
}
}  // namespace rendering
