#pragma once

#include "rendering/model.h"
#include "rendering/mesh.h"
#include "rendering/texture.h"
#include "rendering/shader_program.h"

#include <entt/entt.hpp>

class ResourceManager
{
  public:
    void load_model(const std::string& uri);
    void load_primitive(const std::string& name);
    void load_texture(const std::string& uri, const bool as_cubemap = false);
    void load_skybox(const std::string& uri);
    void load_shader(const std::string& uri,
                     const std::string& vert_filename,
                     const std::string& frag_filename);

    void
    update_shaders(std::function<void(entt::resource_handle<const rendering::ShaderProgram>)> fn);

    entt::resource_handle<const rendering::Model> get_model(const std::string& uri) const;
    entt::resource_handle<const rendering::Texture> get_texture(const std::string& uri) const;
    entt::resource_handle<const rendering::ShaderProgram> get_shader(const std::string& uri) const;

  private:
    entt::resource_cache<rendering::Model> model_cache;
    entt::resource_cache<rendering::Texture> texture_cache;
    entt::resource_cache<rendering::ShaderProgram> shader_cache;
};
