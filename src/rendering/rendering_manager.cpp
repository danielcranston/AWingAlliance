#include <iostream>

#include "resources/load_texture.h"
#include "resources/load_model.h"
#include "rendering/primitives.h"
#include "rendering/rendering_manager.h"

#include "rendering/compile_shader_program.h"

namespace rendering
{
RenderingManager::RenderingManager()
{
}

RenderingManager::~RenderingManager()
{
}

const rendering::Texture& RenderingManager::get_texture(const std::string& filename)
{
    if (textures.find(filename) != textures.end())
    {
        return textures.at(filename);
    }
    else
    {
        throw std::runtime_error("get_texture: requested texture is not loaded: " + filename);
    }
}

const rendering::Model& RenderingManager::get_model(const std::string& filename)
{
    if (models.find(filename) != models.end())
    {
        return models.at(filename);
    }
    else
    {
        throw std::runtime_error("get_model: requested model is not loaded: " + filename);
    }
}
const ShaderProgram& RenderingManager::get_shader_program(const std::string& filename)
{
    if (programs.find(filename) != programs.end())
    {
        return *programs.at(filename).get();
    }
    else
    {
        throw std::runtime_error("get_shader_program: requested program is not loaded: " +
                                 filename);
    }
}

void RenderingManager::register_shader_program(const std::string& name,
                                               const std::string& vertex_filename,
                                               const std::string& fragment_filename,
                                               std::function<void(ShaderProgram* program)> setup_fn)
{
    programs.insert(std::make_pair(name, compileShaders(name, vertex_filename, fragment_filename)));
    setup_fn(programs.at(name).get());
}

void RenderingManager::register_models(
    const std::vector<std::string>& model_filenames,
    std::function<std::vector<resources::MeshData>(const std::string&)> loader_fn)
{
    for (const auto& filename : model_filenames)
    {
        auto meshdata = loader_fn(filename);
        register_model(filename, std::move(meshdata));
    }
}

void RenderingManager::register_model(const std::string& filename,
                                      std::vector<resources::MeshData>&& meshdata)
{
    if (models.find(filename) == models.end())
    {
        std::vector<rendering::Mesh> meshes;
        for (const auto md : meshdata)
        {
            if (md.diffuse_texname != "")
            {
                unloaded_textures.emplace_back(md.diffuse_texname);
                // register_texture(md.diffuse_texname);
            }
            meshes.emplace_back(rendering::Mesh(md));
        }

        register_model(rendering::Model(filename, std::move(meshes)));
    }
}

void RenderingManager::register_model(Model&& model)
{
    if (models.find(model.get_name()) == models.end())
    {
        models.insert(std::make_pair(model.get_name(), std::move(model)));
    }
}

void RenderingManager::register_skybox(const std::string& folder_name,
                                       const std::array<resources::TextureData, 6>& data)
{
    if (textures.find(folder_name) == textures.end())
    {
        textures.insert({ folder_name, rendering::Texture(folder_name, data) });
    };

    if (models.find("box") == models.end())
    {
        register_model(rendering::primitives::box());
    }
}

void RenderingManager::register_texture(const std::string& filename, resources::TextureData&& data)
{
    if (textures.find(filename) == textures.end())
    {
        textures.insert({ filename, std::move(data) });
    }
}

bool RenderingManager::has_unloaded_textures() const
{
    return unloaded_textures.size();
}

void RenderingManager::register_unloaded_textures(
    std::function<resources::TextureData(const std::string&)> loader_fn)
{
    std::cout << "Loading required textures:" << std::endl;
    for (const auto& filename : unloaded_textures)
    {
        if (textures.find(filename) == textures.end())
        {
            register_texture(filename, loader_fn(filename));
        }
    }

    unloaded_textures.clear();
}

const std::map<std::string, rendering::Texture>& RenderingManager::get_textures() const
{
    return textures;
}

}  // namespace rendering
