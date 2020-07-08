#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GL/glew.h>

#include "renderer.h"
#include "tinyobj_helper.h"

Renderer::Renderer(const unsigned int screen_w, const unsigned int screen_h)
  : screen_w(screen_w),
    screen_h(screen_h),
    fbo(512, 512),
    screen_billboard(glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.0f, 0.0f, -1.0f),
                     actor::Billboard::Type::CAMERA_FACING,
                     0,
                     0.0f)
{
    Textures[""] = 0;
    load_model("billboard");

    register_shader("program", "Shaders/object.vert", "Shaders/object.frag");
    register_shader("sky", "Shaders/sky.vert", "Shaders/sky.frag");
    register_shader("terrain", "Shaders/terrain.vert", "Shaders/terrain.frag");
    register_shader("hyperspace", "Shaders/hyperspace.vert", "Shaders/hyperspace.frag");
    register_shader("spark", "Shaders/object.vert", "Shaders/spark.frag");
    Shaders.at("hyperspace")->Use();
    Shaders.at("hyperspace")->SetUniform2f("resolution", 512.0f, 512.0f);
}

std::unique_ptr<Renderer> Renderer::Create(const unsigned int screen_w, const unsigned int screen_h)
{
    return std::make_unique<Renderer>(screen_w, screen_h);
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

void Renderer::SetResolution(const unsigned int width, const unsigned int height)
{
    screen_w = width;
    screen_h = height;
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

        const auto& bbox = ship.second->GetModel()->GetBoundingBox();
        glm::mat4 bbox_transform = bbox.pose * glm::scale(glm::mat4(1.0f), bbox.scale);
        glm::mat4 mvp = projCamMatrix * ship.second->GetPose() * bbox_transform;

        glm::vec3 color = { 0.0f, 0.0f, 1.0f };
        for (const auto& ship2 : game_state->GetShips())
        {
            if (ship.second.get() != ship2.second.get() &&
                ship.second->IsColliding(*ship2.second.get()))
            {
                color = { 1.0f, 0.0f, 0.0f };
                break;
            }
        }
        render_bbox(mvp, color);
    }

    for (const auto& laser : game_state->GetLasers())
    {
        if (laser.alive)
        {
            render_laser(laser, projCamMatrix);
        }
    }

    glDisable(GL_CULL_FACE);

    // Create hyperspace texture (stored in fbo.texid1)
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
    glViewport(0, 0, fbo.width, fbo.height);
    Shaders.at("hyperspace")->Use();
    Shaders.at("hyperspace")->SetUniform1f("time", game_state->GetCurrentTime());
    Models.at("billboard")->Draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, fbo.texid1);
    glViewport(0, 0, screen_w, screen_h);

    // Use hyperspace texture for something ...

    glDepthMask(false);
    Shaders.at("spark")->Use();
    Shaders.at("spark")->SetUniform1f("time", game_state->GetCurrentTime());
    for (const auto& billboard : game_state->GetBillboards())
    {
        render_billboard(billboard, projCamMatrix);
    }
    glDepthMask(true);

    glEnable(GL_CULL_FACE);
}

void Renderer::render_ship(const actor::Ship& ship, const glm::mat4& camera_pose)
{
    // Construct Model Matrix from Position and Viewing Direction
    glm::mat4 mvp = camera_pose * ship.GetPose();
    ship.GetModel()->Draw(mvp, ship.GetColor(), Shaders.at("program")->GetProgram());
}

void Renderer::render_bbox(const glm::mat4& mvp, const glm::vec3& color)
{
    Shaders.at("program")->SetUniformMatrix4fv("mvp", mvp);
    Shaders.at("program")->SetUniform1i("bUseColor", true);
    Shaders.at("program")->SetUniform3fv("uniform_color", color);

    Models.at("cube")->DrawWireframe();
}

void Renderer::render_laser(const actor::Laser& laser, const glm::mat4& camera_pose)
{
    glm::mat4 mvp = camera_pose * laser.GetPose();

    ShaderProgram* program = Shaders.at("program").get();
    program->Use();
    program->SetUniform3fv("uniform_color", laser.color);
    program->SetUniform1i("bUseColor", 1);
    program->SetUniformMatrix4fv("mvp", mvp);
    Models.at("cube")->Draw();
}

void Renderer::render_billboard(const actor::Billboard& billboard, const glm::mat4& camera_pose)
{
    const auto& scale = billboard.GetScale();
    glm::mat4 mvp = camera_pose * billboard.GetPose() *
                    glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 0.0f));

    Shaders.at("spark")->SetUniform1f("starttime", billboard.GetTimeOfBirth());
    Shaders.at("spark")->SetUniformMatrix4fv("mvp", mvp);
    Models.at("billboard")->Draw();
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
