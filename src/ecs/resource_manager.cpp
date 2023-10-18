#include "ecs/resource_manager.h"

namespace
{
}  // namespace

namespace ecs
{
ResourceManager::ResourceManager()
{
    load_primitive("box");
    load_primitive("quad");
}

void ResourceManager::load_model(const std::string& uri)
{
    if (auto uri_hash = entt::hashed_string(uri.data()); !model_cache.contains(uri_hash))
    {
        model_cache.load(uri_hash, uri);

        for (const auto& mesh : model_cache[uri_hash]->get_meshes())
        {
            if (mesh.get_texture_name().size())
            {
                load_texture(mesh.get_texture_name());
            }
        }
    }
}

void ResourceManager::load_primitive(const std::string& name)
{
    auto name_hash = entt::hashed_string(name.c_str());

    if (model_cache.contains(name_hash))
    {
        return;
    }

    if (name == "box")
    {
        model_cache.load(entt::hashed_string(name.c_str()), &rendering::primitives::box);
    }
    else if (name == "quad")
    {
        model_cache.load(entt::hashed_string(name.c_str()), &rendering::primitives::quad);
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
        texture_cache.load(texture_uri, uri, as_cubemap);
    }
}

void ResourceManager::load_skybox(const std::string& uri)
{
    load_texture(uri, true);
}

void ResourceManager::load_shader(const std::string& uri,
                                  const std::string& vert_filename,
                                  const std::string& frag_filename,
                                  const std::optional<std::string>& geom_filename)
{
    shader_cache.load(
        entt::hashed_string(uri.c_str()), uri, vert_filename, frag_filename, geom_filename);
}

void ResourceManager::load_fighter_model(const std::string& uri)
{
    if (auto uri_hash = entt::hashed_string(uri.data()); !fighter_model_cache.contains(uri_hash))
    {
        fighter_model_cache.load(uri_hash, uri);

        if (const auto& visual_name = fighter_model_cache[uri_hash]->visual_name;
            visual_name.empty())
        {
            load_model(visual_name);
        }
    }
}

void ResourceManager::load_sound(const std::string& uri)
{
    if (auto uri_hash = entt::hashed_string(uri.data()); !sound_cache.contains(uri_hash))
    {
        sound_cache.load(uri_hash, uri);
    }
}

void ResourceManager::update_shaders(
    std::function<void(const entt::resource<rendering::ShaderProgram>&)> fn)
{
    for (const auto& program : shader_cache)
    {
        fn(program.second);
    }
    // shader_cache.each([&fn](entt::id_type, entt::resource<const rendering::ShaderProgram>
    // program) {
    //     fn(program);
    // });
}

entt::resource<const rendering::Model> ResourceManager::get_model(const std::string& uri) const
{
    return model_cache[entt::hashed_string(uri.c_str())];
}

entt::resource<const rendering::Texture> ResourceManager::get_texture(const std::string& uri) const
{
    return texture_cache[entt::hashed_string(uri.c_str())];
}

entt::resource<const rendering::ShaderProgram>
ResourceManager::get_shader(const std::string& uri) const
{
    return shader_cache[entt::hashed_string(uri.c_str())];
}

entt::resource<const urdf::FighterModel>
ResourceManager::get_fighter_model(const std::string& uri) const
{
    return fighter_model_cache[entt::hashed_string(uri.c_str())];
}

entt::resource<const audio::AudioBuffer> ResourceManager::get_sound(const std::string& uri) const
{
    return sound_cache[entt::hashed_string(uri.c_str())];
}
}  // namespace ecs
