#include <string>
#include <iostream>

#include <GL/glew.h>

#include "rendering/texture.h"

namespace rendering
{
Texture::Texture(const resources::TextureData& data)
  : Texture(data.name, data.data, data.width, data.height, data.comp)
{
}

Texture::Texture(const std::string& name,
                 const std::vector<unsigned char>& data,
                 const int width,
                 const int height,
                 const int comp)
  : name(name), width(width), height(height), comp(comp), type(Type::TEXTURE)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (comp != 3 && comp != 4)
        throw std::runtime_error("Unsupported texture composition for " + name);

    const int format = comp == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data.data());

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    // std::cout << "Texture \"" << name << "\" (id " << texture_id << ") being constructed"
    //           << std::endl;
}

// from https://learnopengl.com/Advanced-OpenGL/Cubemaps
Texture::Texture(const std::string& name, const std::array<resources::TextureData, 6>& data_vec)
  : name(name),
    width(data_vec[0].width),
    height(data_vec[0].height),
    comp(data_vec[0].comp),
    type(Type::CUBEMAP)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    for (int i = 0; i < data_vec.size(); ++i)
    {
        const auto& texdata = data_vec[i];

        if (texdata.comp != 3 && texdata.comp != 4)
            throw std::runtime_error("Unsupported texture composition for " + texdata.name);

        const int format = texdata.comp == 3 ? GL_RGB : GL_RGBA;

        // std::cout << "  buffering CUBEMAP texture: " << texdata.name << ", w = " << texdata.width
        //           << ", h = " << texdata.height << ", comp = " << comp << std::endl;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     format,
                     texdata.width,
                     texdata.height,
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     texdata.data.data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    // std::cout << "Texture (cubemap) \"" << name << "\" (id " << texture_id << ") being
    // constructed"
    //           << std::endl;
}

Texture::Texture(Texture&& other)
  : name(std::move(other.name)),
    width(other.width),
    height(other.height),
    comp(other.comp),
    texture_id(other.texture_id),
    type(other.type)
{
    // std::cout << "Texture \"" << name << "\" (id " << texture_id << ") being moved" << std::endl;

    // End others ownership of the texture, preventing it from releasing the vao/vbos in its
    // destructor.
    other.is_owning = false;
}

Texture::Texture(const Texture&& other)
  : name(std::move(other.name)),
    width(other.width),
    height(other.height),
    comp(other.comp),
    texture_id(other.texture_id),
    type(other.type)
{
    // std::cout << "Texture \"" << name << "\" (id " << texture_id << ") being moved from const"
    //           << std::endl;

    // End others ownership of the texture, preventing it from releasing the vao/vbos in its
    // destructor.
    other.is_owning = false;
}

Texture& Texture::operator=(Texture&& other)
{
    // std::cout << "Texture \"" << other.name << "\" (id " << other.texture_id
    //           << ") move assignment operator" << std::endl;
    name = other.name;
    width = other.width;
    height = other.height;
    comp = other.comp;
    texture_id = other.texture_id;
    type = other.type;

    // End others ownership of the texture, preventing it from releasing the vao/vbos in its
    // destructor.
    other.is_owning = false;

    return *this;
}

Texture& Texture::operator=(const Texture&& other)
{
    // std::cout << "Texture \"" << other.name << "\" (id " << other.texture_id
    //           << ") const move assignment operator" << std::endl;
    name = other.name;
    width = other.width;
    height = other.height;
    comp = other.comp;
    texture_id = other.texture_id;
    type = other.type;

    // End others ownership of the texture, preventing it from releasing the vao/vbos in its
    // destructor.
    other.is_owning = false;

    return *this;
}

Texture::~Texture()
{
    if (is_owning)
    {
        // std::cout << "Texture " << name << " (id " << texture_id << ") being cleaned up"
        //           << std::endl;
        glDeleteTextures(1, &texture_id);  // TODO: check if cleanup is different for cubemap
    }
    else
    {
        // std::cout << "  (Moved-from Texture is safely discarded without freeing any resources)"
        //           << std::endl;
    }
}
}  // namespace rendering
