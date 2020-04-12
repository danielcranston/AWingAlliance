#pragma once

#include <string>
#include <map>
#include <set>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <model.h>
#include <actor.h>
#include <fighter.h>
#include <terrain.h>
#include <skybox.h>
#include <parser.h>
#include <shaders.h>

namespace loaders
{
unsigned int load_texture(const std::string& filename);
unsigned int load_texture_cubemap(const std::string& textures_folder);
std::unique_ptr<Model> load_model(std::map<std::string, uint>* textures,
                                  const std::string& model_name,
                                  const ShaderProgram* shader);
std::unique_ptr<Terrain> load_terrain(std::map<std::string, uint>* textures,
                                      const ScenarioParser::TerrainEntry& terrainentry,
                                      const uint program);
std::unique_ptr<Skybox> load_skybox(std::map<std::string, std::unique_ptr<Model>>* models,
                                    std::map<std::string, uint>* textures,
                                    const std::string& textures_folder,
                                    const ShaderProgram* shader);

void load_textures(std::map<std::string, uint>* textures,
                   const std::vector<std::string>& filenames);
void load_models(std::map<std::string, std::unique_ptr<Model>>* models,
                 std::map<std::string, uint>* textures,
                 const std::set<std::string>& model_names,
                 const ShaderProgram* shader);
void load_actors(std::map<std::string, std::unique_ptr<actor::Actor>>* actors,
                 std::map<std::string, std::unique_ptr<Model>>* models,
                 const std::map<std::string, ScenarioParser::ActorEntry>& actor_entries);
}  // namespace loaders