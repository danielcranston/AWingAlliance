#pragma once

#include <map>
#include <vector>
#include <string>
#include <functional>

#include "rendering/mesh.h"
#include "rendering/texture.h"
#include "rendering/model.h"
#include "rendering/shader_program.h"

namespace rendering
{
class RenderingManager
{
  public:
    RenderingManager();
    ~RenderingManager();

    const Texture& get_texture(const std::string& filename);
    const Model& get_model(const std::string& filename);
    const ShaderProgram& get_shader_program(const std::string& filename);

    void
    register_models(const std::vector<std::string>& model_filenames,
                    std::function<std::vector<resources::MeshData>(const std::string&)> loader_fn);
    void register_model(const std::string& filename, std::vector<resources::MeshData>&& meshdata);
    void register_model(Model&& model);
    void register_skybox(const std::string& folder_name,
                         const std::array<resources::TextureData, 6>& data);
    void register_texture(const std::string& filename, resources::TextureData&& data);
    void register_shader_program(const std::string& name,
                                 const std::string& vertex_filename,
                                 const std::string& fragment_filename,
                                 std::function<void(ShaderProgram* program)> setup_fn);

    bool has_unloaded_textures() const;
    void
    register_unloaded_textures(std::function<resources::TextureData(const std::string&)> loader_fn);

    const std::map<std::string, Texture>& get_textures() const;

  private:
    std::map<std::string, Texture> textures;
    std::map<std::string, Model> models;
    std::map<std::string, std::unique_ptr<ShaderProgram>> programs;

    std::vector<std::string> unloaded_textures;
};

}  // namespace rendering
