#pragma once

#include <map>
#include <string>
#include <memory>
#include <set>

#include "shaders.h"
#include "model.h"
#include "terrain.h"
#include "actor/actor.h"
#include "actor/laser.h"
#include "game_state.h"
#include "fbo.h"

class Renderer
{
  public:
    static std::unique_ptr<Renderer> Create(const unsigned int screen_w,
                                            const unsigned int screen_h);

    void register_shader(const std::string& name,
                         const std::string& vertex_source,
                         const std::string& frag_source);
    void register_terrain(const Terrain* terrain, const std::vector<std::string>& texture_names);
    void register_skybox(const std::string& textures_folder);
    void load_models(const std::set<std::string>& model_names);
    void load_textures(const std::vector<std::string>& texture_names);
    void load_model(const std::string& name);
    void load_texture(const std::string& filename);
    void load_texture_cubemap(const std::string& textures_folder);
    void load_terrain();
    void list_textures();

    void UseProgram(const std::string& name);
    const ShaderProgram* GetShaderProgram(const std::string& name);

    void SetResolution(const unsigned int width, const unsigned int height);

    const std::map<std::string, std::unique_ptr<Model>>* GetModels();
    const Model* GetModel(const std::string& name);

    void render(const GameState* game_state);
    void render_ship(const actor::Ship& ship, const glm::mat4& camera_pose);
    void render_bbox(const glm::mat4& mvp, const glm::vec3& color);
    void render_terrain(const glm::mat4& camera_pose);
    void render_skybox(const glm::mat4& proj_matrix, glm::mat4 cam_matrix);
    void render_laser(const actor::Laser& laser, const glm::mat4& camera_pose);
    void render_billboard(const actor::Billboard& billboard, const glm::mat4& camera_pose);

    explicit Renderer(const unsigned int screen_w, const unsigned int screen_h);

  private:
    unsigned int screen_w, screen_h;
    std::map<std::string, std::unique_ptr<Model>> Models;
    std::map<std::string, uint> Textures;
    std::map<std::string, std::unique_ptr<ShaderProgram>> Shaders;
    std::unique_ptr<TerrainModel> terrain_model = nullptr;
    std::string skybox_texture = "";
    FBO fbo;
    actor::Billboard screen_billboard;
};
