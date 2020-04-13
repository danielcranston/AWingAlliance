#include <iostream>
#include <math.h>
#include <algorithm>
#include <array>
#include <vector>
#include <memory>
#include <bitset>
#include <map>

/* External Dependencies*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <actor.h>
#include <fighter.h>
#include <keyboard.h>
#include <terrain.h>
#include <skybox.h>
#include <fbo.h>
#include <spline.h>
#include <parser.h>
#include <renderer.h>

uint SCREEN_W, SCREEN_H;
uint timeNow, timeOfLastUpdate, timeOfLastSplineChange;
std::bitset<8> keyboardInfo = 0;

glm::mat4 projCamMatrix, camMatrix, projMatrix;

FBO fbo;

std::unique_ptr<Renderer> renderer;
std::map<std::string, std::unique_ptr<actor::Actor>> Actors;
std::string player_name;

std::unique_ptr<ScenarioParser> parser;

std::unique_ptr<Skybox> skybox;
std::unique_ptr<Terrain> terrain;

void init()
{
    std::cout << "init" << '\n';

    SCREEN_W = 1200;
    SCREEN_H = 900;
    timeNow = 0;
    timeOfLastUpdate = 0;
    timeOfLastSplineChange = 0;

    // PARSE SCENARIO
    parser = std::make_unique<ScenarioParser>("scenario1.json");
    parser->Parse();

    renderer = Renderer::Create();
    renderer->register_shader("program", "Shaders/object.vert", "Shaders/object.frag");
    renderer->register_shader("sky", "Shaders/sky.vert", "Shaders/sky.frag");
    renderer->register_shader("terrain", "Shaders/terrain.vert", "Shaders/terrain.frag");
    renderer->load_models(parser->required_models);

    // terrain = std::make_unique<Terrain>(parser->terrain.get());
    // skybox = loaders::load_skybox(&Models, &Textures, parser->skybox, Shaders.at("sky").get());
    renderer->list_textures();

    // ACTOR STUFF
    for (const auto& actorentry : parser->actors)
    {
        const Model* model_ptr = renderer->GetModel(actorentry.second.type);
        // Check type of actor and create appropriate derived class
        if (actorentry.second.type == "hangar")
        {
            Actors.insert(std::make_pair(
                actorentry.first,
                actor::Actor::Create(actorentry.second.pos, actorentry.second.dir, model_ptr)));
        }
        else
        {
            Actors.insert(std::make_pair(
                actorentry.first,
                actor::Fighter::Create(actorentry.second.pos, actorentry.second.dir, model_ptr)));
        }
    }
    player_name = parser->player;
    dynamic_cast<actor::Fighter*>(Actors["tie1"].get())->bDrawSpline = true;

    fbo.Init(800, 600);
    fbo.SetupQuad();
    glActiveTexture(GL_TEXTURE0);
    std::cout << "Finished Init" << '\n';
}

void onIdle()
{
    glutPostRedisplay();
}

// void DummyCompute()
// {
//     glUseProgram(computeProgram);
//     glBindImageTexture(0, fbo.texid1, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
//     glBindImageTexture(1, fbo.texid2, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
//     // Dispatch compute shaders in Y direction and write it as x component
//     int nGroupsX = std::ceil(fbo.width / 32.0);
//     int nGroupsY = std::ceil(fbo.height / 32.0);
//     glDispatchCompute(nGroupsX, nGroupsY, 1);

//     glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//     glUseProgram(0);
// }

void onDisplay()
{
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ProcessKeyboardInput(keyboardInfo);
    timeNow = glutGet(GLUT_ELAPSED_TIME);
    if (timeNow - timeOfLastUpdate > 5)
    {
        float dt = (timeNow - timeOfLastUpdate) / 1000.0;

        dynamic_cast<actor::Fighter*>(Actors["awing1"].get())->Update(keyboardInfo, dt);
        dynamic_cast<actor::Fighter*>(Actors["tie1"].get())->Update_Roaming(timeNow / 1000.0f);
        timeOfLastUpdate = glutGet(GLUT_ELAPSED_TIME);
    }

    camMatrix = glm::lookAt(Actors[player_name]->pos - (20.0f * Actors[player_name]->dir) +
                                5.0f * glm::vec3(0.0, 1.0, 0.0),
                            Actors[player_name]->pos,
                            glm::vec3(0.0, 1.0, 0.0));
    projCamMatrix = projMatrix * camMatrix;

    // Draw to FBO
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
    // glBindTexture(GL_TEXTURE_2D, fbo.texid1);
    // glViewport(0, 0, fbo.width, fbo.height);
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);
    // glClear(GL_DEPTH_BUFFER_BIT);
    // if (skybox)
    // {
    //     skybox->Draw(projMatrix, camMatrix);
    //     utils::CheckErrors("draw sky");
    // }

    // glUseProgram(program);
    // for (auto& actor : Actors)
    // {
    //     actor.second->Draw(projCamMatrix);
    // }

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // DummyCompute();
    // glClearColor(0.8, 0.8, 0.8, 1.0);
    // glViewport(0, 0, SCREEN_W, SCREEN_H);

    // Draw billboard of FBO contents
    // glUseProgram(fboProgram);
    // glBindVertexArray(fbo.vao);
    // glDisable(GL_CULL_FACE);
    // glBindTexture(GL_TEXTURE_2D, fbo.texid1);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // glEnable(GL_CULL_FACE);
    // utils::CheckErrors("draw quad");

    renderer->UseProgram("program");
    for (auto& actor : Actors)
    {
        renderer->render(actor.second->pos, actor.second->dir, actor.second->model, projCamMatrix);
    }

    glutSwapBuffers();
}

void onReshape(int width, int height)
{
    std::cout << "onReshape" << '\n';
    SCREEN_W = width;
    SCREEN_H = height;
    glViewport(0, 0, SCREEN_W, SCREEN_H);

    projMatrix = glm::perspective(45.0f, 1.0f * SCREEN_W / SCREEN_H, 0.1f, 8192.0f);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 2);
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);  // GLUT_DOUBLE
    glutInitWindowSize(1200, 900);
    glutCreateWindow("awing");

    // glewExperimental = GL_TRUE;

    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return EXIT_FAILURE;
    }
    else
    {
        fprintf(stderr, "GLEW Initialized: %s\n", glewGetErrorString(glew_status));
    }

    init();

    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
    glutReshapeFunc(onReshape);
    glutIdleFunc(onIdle);
    glutDisplayFunc(onDisplay);
    glutKeyboardFunc(onKeyDown);
    glutKeyboardUpFunc(onKeyUp);
    glutSpecialFunc(onSpecialDown);
    glutSpecialUpFunc(onSpecialUp);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // glEnable(GL_PRIMITIVE_RESTART);
    glutMainLoop();

    return 0;
}