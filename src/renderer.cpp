#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GL/glew.h>

#include "renderer.h"
#include "tinyobj_helper.h"

Renderer::Renderer()
{
    Textures[""] = 0;
}

std::unique_ptr<Renderer> Renderer::Create()
{
    Renderer r{};
    return std::make_unique<Renderer>(std::move(r));
}

void Renderer::register_shader(const std::string& name,
                               const std::string& vertex_source,
                               const std::string& frag_source)
{
    Shaders.insert(std::make_pair("program", compileShaders(name, vertex_source, frag_source)));
}

void Renderer::load_models(const std::set<std::string>& model_names)
{
    for (const auto& model_name : model_names)
    {
        load_model(model_name);
    }
}

void Renderer::load_model(const std::string& name)
{
    // Ignore if model is already loaded.
    if (Models.find(name) != Models.end())
    {
        std::cout << "[" << name << "] is already loaded. Skipping.\n";
        return;
    }

    std::cout << "Loading [" << name << "]\n";

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    bool ret = load_obj(&attrib, &shapes, &materials, name);

    std::vector<uint> texture_ids;
    for (const auto& material : materials)
    {
        // Will skip if texture is already loaded
        load_texture(material.diffuse_texname);
    }

    const auto res = tinobj_helper::parse_obj(attrib, shapes, materials, Textures);
    Models.insert(
        std::make_pair(name, Model::Create(name, std::move(res.groups), res.bounding_box)));
}

void Renderer::load_textures(const std::vector<std::string>& texture_names)
{
    for (const auto& filename : texture_names)
    {
        load_texture(filename);
    }
}

void Renderer::load_texture(const std::string& filename)
{
    int w, h;
    int comp;

    // Some materials have no texture (ie only color)
    if (filename == "")
    {
        std::cout << "  Skipping material with no diffuse texture" << std::endl;
        return;
    }

    // Skip if already loaded
    if (Textures.find(filename) != Textures.end())
    {
        std::cout << "[" << filename << "] is already loaded. Skipping.\n";
        return;
    }

    std::string texture_path = "Textures/" + filename;
    if (!FileExists(texture_path))
    {
        std::cerr << "Unable to find file: " << texture_path << std::endl;
        throw std::runtime_error("Unable to find texture file");
    }
    // Load image data into CPU memory
    unsigned char* image = stbi_load(texture_path.c_str(), &w, &h, &comp, STBI_default);
    if (!image)
    {
        std::cerr << "Unable to load texture: " << texture_path << std::endl;
        throw std::runtime_error("Unable to load texture");
    }
    std::cout << "  Loaded texture: " << texture_path << ", w=" << w << ", h=" << h
              << ", comp=" << comp << std::endl;

    // Upload buffered image to GPU
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (comp == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    else if (comp == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    else
    {
        throw std::runtime_error("Unsupported texture composition for " + filename);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);
    Textures.insert(std::make_pair(filename, texture_id));
}

void Renderer::UseProgram(const std::string& name)
{
    assert(Shaders.find(name) != Shaders.end());
    Shaders.at(name)->Use();
}

const ShaderProgram* Renderer::GetShaderProgram(const std::string& name)
{
    assert(Shaders.find(name) != Shaders.end());
    return Shaders.at(name).get();
}

const Model* Renderer::GetModel(const std::string& name)
{
    assert(Models.find(name) != Models.end());
    return Models.at(name).get();
}

void Renderer::list_textures()
{
    std::cout << "Loaded Textures: (texID : name)" << std::endl;
    for (auto const& texture : Textures)
        std::cout << "  " << texture.second << " : " << texture.first << std::endl;
}
