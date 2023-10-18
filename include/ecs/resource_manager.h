#pragma once

#include <optional>

#include "rendering/model.h"
#include "rendering/texture.h"
#include "rendering/shader_program.h"
#include "urdf/fighter_model.h"
#include "audio/audio.h"
#include "resources/load_model.h"
#include "resources/load_texture.h"
#include "resources/load_geometry.h"
#include "resources/locator.h"
#include "rendering/compile_shader_program.h"
#include "rendering/primitives.h"
#include "urdf/parsing.h"
#include <entt/entt.hpp>

namespace ecs
{
struct model_loader final
{
    using result_type = std::shared_ptr<rendering::Model>;

    result_type operator()(const std::string& uri) const
    {
        auto meshes = std::vector<rendering::Mesh>();
        for (const auto& mesh_data : resources::load_model(uri))
        {
            meshes.emplace_back(mesh_data);
        }

        return std::make_shared<rendering::Model>(uri, std::move(meshes));
    }

    result_type operator()(std::function<rendering::Model()> load_fn) const
    {
        return std::make_shared<rendering::Model>(load_fn());
    }
};

struct primitive_loader final
{
    using result_type = std::shared_ptr<rendering::Model>;

    result_type operator()(std::function<rendering::Model()> load_fn) const
    {
        return std::make_shared<rendering::Model>(load_fn());
    }
};

struct texture_loader final
{
    using result_type = std::shared_ptr<rendering::Texture>;

    result_type operator()(const std::string& uri, const bool as_cubemap) const
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

struct shader_loader final
{
    using result_type = std::shared_ptr<rendering::ShaderProgram>;

    result_type operator()(const std::string& uri,
                           const std::string& vert_filename,
                           const std::string& frag_filename,
                           const std::optional<std::string>& geom_filename) const
    {
        return rendering::compileShaders(uri, vert_filename, frag_filename, geom_filename);
    }
};

struct fighter_model_loader final
{
    using result_type = std::shared_ptr<urdf::FighterModel>;

    result_type operator()(const std::string& uri) const
    {
        return std::make_shared<urdf::FighterModel>(urdf::parse_fighter_urdf(uri));
    }
};

struct sound_loader final
{
    using result_type = std::shared_ptr<audio::AudioBuffer>;

    result_type operator()(const std::string& uri) const
    {
        return std::make_shared<audio::AudioBuffer>(std::string(resources::locator::SOUNDS_PATH) +
                                                    uri);
    }
};

class ResourceManager
{
  public:
    ResourceManager();

    void load_model(const std::string& uri);
    void load_primitive(const std::string& name);
    void load_texture(const std::string& uri, const bool as_cubemap = false);
    void load_skybox(const std::string& uri);
    void load_shader(const std::string& uri,
                     const std::string& vert_filename,
                     const std::string& frag_filename,
                     const std::optional<std::string>& geom_filename = std::nullopt);
    void load_fighter_model(const std::string& uri);
    void load_sound(const std::string& uri);

    void update_shaders(std::function<void(const entt::resource<rendering::ShaderProgram>&)> fn);

    entt::resource<const rendering::Model> get_model(const std::string& uri) const;
    entt::resource<const rendering::Texture> get_texture(const std::string& uri) const;
    entt::resource<const rendering::ShaderProgram> get_shader(const std::string& uri) const;
    entt::resource<const urdf::FighterModel> get_fighter_model(const std::string& uri) const;
    entt::resource<const audio::AudioBuffer> get_sound(const std::string& uri) const;

  private:
    entt::resource_cache<rendering::Model, model_loader> model_cache;
    entt::resource_cache<rendering::Texture, texture_loader> texture_cache;
    entt::resource_cache<rendering::ShaderProgram, shader_loader> shader_cache;
    entt::resource_cache<urdf::FighterModel, fighter_model_loader> fighter_model_cache;
    entt::resource_cache<audio::AudioBuffer, sound_loader> sound_cache;
};
}  // namespace ecs
