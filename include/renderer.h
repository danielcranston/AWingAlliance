#pragma once

#include <map>
#include <string>
#include <memory>
#include <set>

#include "shaders.h"
#include "model.h"
#include "terrain.h"

class Renderer
{
  public:
    static std::unique_ptr<Renderer> Create();

    void register_shader(const std::string& name,
                         const std::string& vertex_source,
                         const std::string& frag_source);
    void load_models(const std::set<std::string>& model_names);
    void load_textures(const std::vector<std::string>& texture_names);
    void load_model(const std::string& name);
    void load_texture(const std::string& filename);
    void load_terrain();
    void list_textures();

    void UseProgram(const std::string& name);
    const ShaderProgram* GetShaderProgram(const std::string& name);
    const Model* GetModel(const std::string& name);

    void render(const glm::vec3& pos,
                const glm::vec3& dir,
                const Model* model,
                const glm::mat4 camera_pose);

  private:
    explicit Renderer();
    std::map<std::string, std::unique_ptr<Model>> Models;
    std::map<std::string, uint> Textures;
    std::map<std::string, std::unique_ptr<ShaderProgram>> Shaders;
};
