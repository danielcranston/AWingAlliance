#include <iostream>
#include <fstream>
#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>

#include <loaders.h>
#include <loader_utils.h>

namespace loaders
{
void load_models(std::map<std::string, Model>* models,
                 std::map<std::string, uint>* textures,
                 const std::set<std::string>& model_names)
{
    for (const auto& model_name : model_names)
    {
        if (!load_model(models, textures, model_name))
        {
            throw std::runtime_error("  Failed to load model [" + model_name + "]!");
        }
    }
}

bool load_model(std::map<std::string, Model>* models,
                std::map<std::string, uint>* textures,
                const std::string& model_name)
{
    // Ignore if model is already loaded.
    std::cout << "Loading [" << model_name << "]\n";
    if (models->find(model_name) == models->end())
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        bool ret = load_obj(&attrib, &shapes, &materials, model_name);

        for (const auto& material : materials)
        {
            // Only load texture if it hasn't already been loaded
            if (textures->find(material.diffuse_texname) == textures->end())
            {
                const std::string& tex_name = material.diffuse_texname;

                const unsigned int tex_id = load_texture(tex_name);
                textures->insert(std::make_pair(tex_name, tex_id));
            }
        }

        Model model =
            create_model_from_drawobjects(attrib, shapes, materials, textures, model_name);
        models->insert(std::make_pair(model_name, model));
    }
    else
    {
        std::cout << "  model already exists. skipping..." << '\n';
    }
    return true;
}

void load_textures(std::map<std::string, uint>* textures, const std::vector<std::string>& filenames)
{
    for (const auto& tex_name : filenames)
    {
        unsigned int tex_id = load_texture(tex_name);
        textures->insert(std::make_pair(tex_name, tex_id));
    }
}

unsigned int load_texture(const std::string& filename)
{
    int w, h;
    int comp;

    // Some materials have no texture (ie only color)
    if (filename != "")
    {
        std::string texture_path = "Textures/" + filename;
        if (!FileExists(texture_path))
        {
            std::cerr << "Unable to find file: " << texture_path << std::endl;
            exit(1);
        }
        // Load image data into CPU memory
        unsigned char* image = stbi_load(texture_path.c_str(), &w, &h, &comp, STBI_default);
        if (!image)
        {
            std::cerr << "Unable to load texture: " << texture_path << std::endl;
            exit(1);
        }
        std::cout << "  Loaded texture: " << texture_path << ", w=" << w << ", h=" << h
                  << ", comp=" << comp << std::endl;

        // Upload buffered image to GPU
        unsigned int texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (comp == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        }
        else if (comp == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }
        else
        {
            throw std::runtime_error("Unsupported texture composition for " + filename);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);
        return texture_id;
    }
    else
    {
        std::cout << "  Skipping material with no diffuse texture" << std::endl;
        return 0;
    }
}

std::unique_ptr<Terrain> load_terrain(std::map<std::string, uint>* textures,
                                      const ScenarioParser::TerrainEntry& terrainentry,
                                      const uint program)
{
    std::cout << "Loading terrain\n";
    // First load required textures into the almighty map
    for (const auto& tex_name : terrainentry.textures)
    {
        // Only load texture if it hasn't already been loaded
        if (textures->find(tex_name) == textures->end())
        {
            unsigned int tex_id = load_texture(tex_name);
            textures->insert(std::make_pair(tex_name, tex_id));
        }
    }

    return std::make_unique<Terrain>(terrainentry, textures, program);
}

std::unique_ptr<Skybox> load_skybox(std::map<std::string, Model>* models,
                                    std::map<std::string, uint>* textures,
                                    const std::string& textures_folder,
                                    const uint program)
{
    std::cout << "Loading skybox\n";

    if (!loaders::load_model(models, textures, "cube"))
        throw std::runtime_error("  Failed to load model [cube]!");

    // Only load texture if it hasn't already been loaded
    if (textures->find(textures_folder) == textures->end())
    {
        unsigned int tex_id = load_texture_cubemap(textures_folder);
        textures->insert(std::make_pair(textures_folder, tex_id));
    }
    uint tex_id = textures->find(textures_folder)->second;
    Model* cube = &models->operator[]("cube");
    return std::make_unique<Skybox>(cube, tex_id, program);
}

// from https://learnopengl.com/Advanced-OpenGL/Cubemaps
unsigned int load_texture_cubemap(const std::string& textures_folder)
{
    std::vector<std::string> faces = {
        "Textures/" + textures_folder + "/right.png", "Textures/" + textures_folder + "/left.png",
        "Textures/" + textures_folder + "/top.png",   "Textures/" + textures_folder + "/bot.png",
        "Textures/" + textures_folder + "/front.png", "Textures/" + textures_folder + "/back.png"
    };

    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    int w, h, comp;
    for (std::size_t i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &comp, 0);
        if (data)
        {
            std::cout << "  Loaded CUBEMAP texture: " << faces[i] << ", w = " << w << ", h = " << h
                      << ", comp = " << comp << std::endl;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGB,
                         w,
                         h,
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        }
        else
        {
            stbi_image_free(data);
            throw std::runtime_error("    Cubemap texture failed to load at path: " + faces[i]);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

    // Not needed. Also, if put after glGenTextures the cubemap becomes all black. Not sure why...
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    return texture_id;
}

void load_actors(std::map<std::string, std::unique_ptr<actor::Actor>>* actors,
                 std::map<std::string, Model>* models,
                 const std::map<std::string, ScenarioParser::ActorEntry>& actor_entries)
{
    for (const auto& actorentry : actor_entries)
    {
        Model* model_ptr = &models->operator[](actorentry.second.type);
        // Check type of actor and create appropriate derived class
        if (actorentry.second.type != "hangar")
        {
            actor::Fighter a = actor::Fighter(
                actorentry.second.pos,
                actorentry.second.dir,
                { std::make_pair(model_ptr, glm::mat4(1.0f)) });
            actors->insert(std::make_pair(actorentry.first, std::make_unique<actor::Fighter>(a)));
        }
        else
        {
            actor::Actor a = { actorentry.second.pos,
                               actorentry.second.dir,
                               { std::make_pair(model_ptr, glm::mat4(1.0f)) } };
            actors->insert(std::make_pair(actorentry.first, std::make_unique<actor::Actor>(a)));
        }
    }
}
}  // namespace loaders
