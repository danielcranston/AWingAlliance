#include "ecs/resource_manager.h"

#include "resources/load_model.h"
#include "resources/load_texture.h"
#include "resources/load_geometry.h"
#include "rendering/compile_shader_program.h"
#include "rendering/primitives.h"

namespace
{
struct model_loader final : entt::resource_loader<model_loader, rendering::Model>
{
    std::shared_ptr<rendering::Model> load(const std::string& uri) const
    {
        auto meshes = std::vector<rendering::Mesh>();
        for (const auto& mesh_data : resources::load_model(uri))
        {
            meshes.emplace_back(mesh_data);
        }

        return std::make_shared<rendering::Model>(uri, std::move(meshes));
    }
};

struct primitive_loader final : entt::resource_loader<primitive_loader, rendering::Model>
{
    std::shared_ptr<rendering::Model> load(std::function<rendering::Model()> load_fn) const
    {
        return std::make_shared<rendering::Model>(load_fn());
    }
};

struct texture_loader final : entt::resource_loader<texture_loader, rendering::Texture>
{
    std::shared_ptr<rendering::Texture> load(const std::string& uri, const bool as_cubemap) const
    {
        if (as_cubemap)
        {
            return std::make_shared<rendering::Texture>(uri, resources::load_cubemap_texture(uri));
        }
        else
        {
            return std::make_shared<rendering::Texture>(resources::load_texture(uri));
        }
    }
};

struct shader_loader final : entt::resource_loader<shader_loader, rendering::ShaderProgram>
{
    std::shared_ptr<rendering::ShaderProgram> load(const std::string& uri,
                                                   const std::string& vert_filename,
                                                   const std::string& frag_filename) const
    {
        return rendering::compileShaders(uri, vert_filename, frag_filename);
    }
};

}  // namespace

void ResourceManager::load_model(const std::string& uri)
{
    auto uri_hash = entt::hashed_string(uri.data());
    model_cache.load<model_loader>(uri_hash, uri);

    for (const auto& mesh : model_cache.handle(uri_hash)->get_meshes())
    {
        if (mesh.get_texture_name().size())
        {
            load_texture(mesh.get_texture_name());
        }
    }
}

void ResourceManager::load_primitive(const std::string& name)
{
    auto name_hash = entt::hashed_string(name.c_str());

    if (name == "box")
    {
        if (!model_cache.contains(name_hash))
        {
            model_cache.load<primitive_loader>(entt::hashed_string(name.c_str()),
                                               &rendering::primitives::box);
        }
    }
    else
    {
        throw std::runtime_error(std::string("unrecognized primitive: " + name));
    }
}

void ResourceManager::load_texture(const std::string& uri, const bool as_cubemap)
{
    if (auto texture_uri = entt::hashed_string(uri.c_str()); !texture_cache.contains(texture_uri))
    {
        texture_cache.load<texture_loader>(texture_uri, uri, as_cubemap);
    }
}

void ResourceManager::load_skybox(const std::string& uri)
{
    load_primitive("box");
    load_texture(uri, true);
}

void ResourceManager::load_shader(const std::string& uri,
                                  const std::string& vert_filename,
                                  const std::string& frag_filename)
{
    shader_cache.load<shader_loader>(
        entt::hashed_string(uri.c_str()), uri, vert_filename, frag_filename);
}

void ResourceManager::update_shaders(
    std::function<void(entt::resource_handle<const rendering::ShaderProgram>)> fn)
{
    shader_cache.each(
        [&fn](entt::id_type, entt::resource_handle<const rendering::ShaderProgram> program) {
            fn(program);
        });
}

entt::resource_handle<const rendering::Model>
ResourceManager::get_model(const std::string& uri) const
{
    return model_cache.handle(entt::hashed_string(uri.c_str()));
}

entt::resource_handle<const rendering::Texture>
ResourceManager::get_texture(const std::string& uri) const
{
    return texture_cache.handle(entt::hashed_string(uri.c_str()));
}

entt::resource_handle<const rendering::ShaderProgram>
ResourceManager::get_shader(const std::string& uri) const
{
    return shader_cache.handle(entt::hashed_string(uri.c_str()));
}
