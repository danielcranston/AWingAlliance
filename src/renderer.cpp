#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GL/glew.h>

#include "renderer.h"
#include "tinyobj_helper.h"

Renderer::Renderer()
{
    Textures[""] = 0;
}

std::unique_ptr<Renderer> Renderer::Create()
{
    return std::make_unique<Renderer>(Renderer());
}

void Renderer::register_shader(const std::string& name,
                               const std::string& vertex_source,
                               const std::string& frag_source)
{
    Shaders.insert(std::make_pair(name, compileShaders(name, vertex_source, frag_source)));
}

void Renderer::register_terrain(const Terrain* terrain,
                                const std::vector<std::string>& texture_names)
{
    std::vector<uint> texture_ids;
    for (const auto& tex_name : texture_names)
    {
        load_texture(tex_name);
        texture_ids.push_back(Textures.at(tex_name));
    }

    const auto& buffers = terrain->CreateBuffers();
    terrain_model = std::make_unique<TerrainModel>(texture_ids,
                                                   buffers.first,
                                                   buffers.second,
                                                   terrain->max_height,
                                                   Shaders.at("terrain")->GetProgram());
}

void Renderer::register_skybox(const std::string& textures_folder)
{
    std::cout << "Loading skybox" << std::endl;

    if (Models.find("cube") == Models.end())
        load_model("cube");

    if (Textures.find(textures_folder) == Textures.end())
        load_texture_cubemap(textures_folder);

    skybox_texture = textures_folder;
}

void Renderer::load_models(const std::set<std::string>& model_names)
{
    for (const auto& model_name : model_names)
    {
        load_model(model_name);
    }
}

void Renderer::load_model(const std::string& name)
{
    // Ignore if model is already loaded.
    if (Models.find(name) != Models.end())
    {
        std::cout << "[" << name << "] is already loaded. Skipping.\n";
        return;
    }

    std::cout << "Loading [" << name << "]\n";

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    bool ret = load_obj(&attrib, &shapes, &materials, name);

    std::vector<uint> texture_ids;
    for (const auto& material : materials)
    {
        // Will skip if texture is already loaded
        load_texture(material.diffuse_texname);
    }

    const auto res = tinobj_helper::parse_obj(attrib, shapes, materials, Textures);
    Models.insert(
        std::make_pair(name, Model::Create(name, std::move(res.groups), res.bounding_box)));
}

void Renderer::load_textures(const std::vector<std::string>& texture_names)
{
    for (const auto& filename : texture_names)
    {
        load_texture(filename);
    }
}

void Renderer::load_texture(const std::string& filename)
{
    int w, h;
    int comp;

    // Some materials have no texture (ie only color)
    if (filename == "")
    {
        std::cout << "  Skipping material with no diffuse texture" << std::endl;
        return;
    }

    // Skip if already loaded
    if (Textures.find(filename) != Textures.end())
    {
        std::cout << "[" << filename << "] is already loaded. Skipping.\n";
        return;
    }

    std::string texture_path = "Textures/" + filename;
    if (!FileExists(texture_path))
    {
        std::cerr << "Unable to find file: " << texture_path << std::endl;
        throw std::runtime_error("Unable to find texture file");
    }
    // Load image data into CPU memory
    unsigned char* image = stbi_load(texture_path.c_str(), &w, &h, &comp, STBI_default);
    if (!image)
    {
        std::cerr << "Unable to load texture: " << texture_path << std::endl;
        throw std::runtime_error("Unable to load texture");
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
    Textures.insert(std::make_pair(filename, texture_id));
}

// from https://learnopengl.com/Advanced-OpenGL/Cubemaps
void Renderer::load_texture_cubemap(const std::string& textures_folder)
{
    std::vector<std::string> faces = {
        "Textures/" + textures_folder + "/right.png", "Textures/" + textures_folder + "/left.png",
        "Textures/" + textures_folder + "/top.png",   "Textures/" + textures_folder + "/bot.png",
        "Textures/" + textures_folder + "/front.png", "Textures/" + textures_folder + "/back.png"
    };

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

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

    Textures.insert(std::make_pair(textures_folder, texture_id));
}

void Renderer::UseProgram(const std::string& name)
{
    assert(Shaders.find(name) != Shaders.end());
    Shaders.at(name)->Use();
}

const ShaderProgram* Renderer::GetShaderProgram(const std::string& name)
{
    assert(Shaders.find(name) != Shaders.end());
    return Shaders.at(name).get();
}

const Model* Renderer::GetModel(const std::string& name)
{
    assert(Models.find(name) != Models.end());
    return Models.at(name).get();
}

const std::map<std::string, std::unique_ptr<Model>>* Renderer::GetModels()
{
    return &Models;
}

void Renderer::list_textures()
{
    std::cout << "Loaded Textures: (texID : name)" << std::endl;
    for (auto const& texture : Textures)
        std::cout << "  " << texture.second << " : " << texture.first << std::endl;
}

void Renderer::render(const GameState* game_state)
{
    render_skybox(game_state->GetCamera().GetProjMatrix(), game_state->GetCamera().GetCamMatrix());

    const glm::mat4 projCamMatrix =
        game_state->GetCamera().GetProjMatrix() * game_state->GetCamera().GetCamMatrix();
    render_terrain(projCamMatrix);

    UseProgram("program");
    for (const auto& ship : game_state->GetShips())
    {
        render_ship(*ship.second, projCamMatrix);
    }
    for (const auto& laser : game_state->GetLasers())
    {
        render_laser(laser, projCamMatrix);
    }
}

void Renderer::render_ship(const actor::Ship& ship, const glm::mat4& camera_pose)
{
    // Construct Model Matrix from Position and Viewing Direction
    glm::mat4 mvp = camera_pose * ship.GetPose();
    ship.GetModel()->Draw(mvp, ship.GetColor(), Shaders.at("program")->GetProgram());

    mvp = mvp * ship.GetModel()->GetBoundingBox().pose;
    Shaders.at("program")->SetUniformMatrix4fv("mvp", mvp);
    Shaders.at("program")->SetUniform1i("bUseColor", true);
    Shaders.at("program")->SetUniform3fv("uniform_color", { 0.0f, 1.0f, 0.0f });

    Models.at("cube")->DrawWireframe();
}

void Renderer::render_laser(const Laser& laser, const glm::mat4& camera_pose)
{
    glm::mat4 mvp = camera_pose * laser.GetPose();

    ShaderProgram* program = Shaders.at("program").get();
    program->Use();
    program->SetUniform3fv("uniform_color", laser.color);
    program->SetUniform1i("bUseColor", 1);
    program->SetUniformMatrix4fv("mvp", mvp);
    Models.at("cube")->Draw();
}

void Renderer::render_terrain(const glm::mat4& camera_pose)
{
    if (terrain_model)
    {
        Shaders.at("terrain")->Use();
        terrain_model->Draw(camera_pose, Shaders.at("terrain")->GetProgram());
    }
}

void Renderer::render_skybox(const glm::mat4& proj_matrix, glm::mat4 cam_matrix)
{
    if (skybox_texture != "")
    {
        cam_matrix[3] = glm::vec4(0, 0, 0, 1);
        const auto mvp = proj_matrix * cam_matrix;

        ShaderProgram* sky_program = Shaders.at("sky").get();
        sky_program->Use();
        sky_program->SetUniform1i("tex", 0);
        sky_program->SetUniformMatrix4fv("mvp", mvp);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Textures.at(skybox_texture));

        glDisable(GL_CULL_FACE);
        Models.at("cube")->Draw();
        glEnable(GL_CULL_FACE);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}
