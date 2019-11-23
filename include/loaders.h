#pragma once

#include <string>
#include <map>
#include <set>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <model.h>
#include <terrain.h>
#include <skybox.h>
#include <parser.h>

namespace loaders{
	void load_textures(std::map<std::string, uint> *textures, const std::vector<std::string> filenames);
	unsigned int load_texture(std::string filename);
	unsigned int load_texture_cubemap(const std::string &textures_folder);
	void loadModels(std::map<std::string, Model>& Models, std::vector<std::string> model_names);
	bool load_models(std::map<std::string, Model> *models, std::map<std::string, uint> *textures, const std::set<std::string> &model_names);
	bool load_model(std::map<std::string, Model> *models, std::map<std::string, uint> *textures, const std::string &model_name);
	Terrain load_terrain(std::map<std::string, uint> *textures, const ScenarioParser::TerrainEntry &terrainentry, uint program);
	Skybox load_skybox(std::map<std::string, Model> *models, std::map<std::string, uint> *textures, const std::string &textures_folder, uint program);

} // namespace loaders