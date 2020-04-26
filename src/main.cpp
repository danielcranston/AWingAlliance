#include <iostream>
#include <math.h>
#include <algorithm>
#include <array>
#include <vector>
#include <memory>
#include <map>
#include <list>

#include <chrono>
#include <iostream>
#include <unistd.h>

/* External Dependencies*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <actor/ship.h>
#include <keyboard.h>
#include <terrain.h>
#include <fbo.h>
#include <spline.h>
#include <parser.h>
#include <renderer.h>
#include <game_state.h>

uint SCREEN_W, SCREEN_H;

std::chrono::system_clock::time_point t;
const std::chrono::system_clock::duration dt = std::chrono::milliseconds(10);

std::chrono::system_clock::time_point currentTime;
std::chrono::system_clock::duration accumulator;

FBO fbo;

std::unique_ptr<Renderer> renderer;
std::unique_ptr<GameState> game_state;

std::unique_ptr<ScenarioParser> parser;

void init()
{
    std::cout << "init" << '\n';

    SCREEN_W = 1200;
    SCREEN_H = 900;

    // PARSE SCENARIO
    parser = std::make_unique<ScenarioParser>("scenario1.json");
    parser->Parse();

    renderer = Renderer::Create();
    renderer->register_shader("program", "Shaders/object.vert", "Shaders/object.frag");
    renderer->register_shader("sky", "Shaders/sky.vert", "Shaders/sky.frag");
    renderer->register_shader("terrain", "Shaders/terrain.vert", "Shaders/terrain.frag");
    renderer->load_models(parser->required_models);
    renderer->register_skybox(parser->skybox);

    game_state = GameState::Create(renderer->GetModels());
    game_state->register_ships(parser->actors);
    game_state->register_terrain(parser->terrain.get());
    game_state->register_player(parser->player);

    renderer->register_terrain(game_state->GetTerrain(), parser->terrain->textures);
    renderer->list_textures();

    currentTime = std::chrono::system_clock::now();
    accumulator = std::chrono::milliseconds(0);

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

    // https://gafferongames.com/post/fix_your_timestep/
    // " the renderer produces time and the simulation consumes it in discrete dt sized steps."
    using namespace std::chrono;
    auto newTime = system_clock::now();
    auto frameTime = duration_cast<milliseconds>(newTime - currentTime);
    currentTime = newTime;

    accumulator += frameTime;

    while (accumulator >= dt)
    {
        game_state->integrate(t, dt);
        accumulator -= dt;
        t += dt;
    }

    renderer->render(game_state.get());

    glutSwapBuffers();
}

void onReshape(int width, int height)
{
    std::cout << "onReshape" << '\n';
    SCREEN_W = width;
    SCREEN_H = height;
    glViewport(0, 0, SCREEN_W, SCREEN_H);

    game_state->projMatrix = glm::perspective(45.0f, 1.0f * SCREEN_W / SCREEN_H, 0.1f, 8192.0f);
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
    // glutTimerFunc(1000/FPS, &onTimer, 0);
    // Set OpenGL debug message callback.
    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // glDebugMessageControl(
    //     GL_DONT_CARE,
    //     GL_DONT_CARE,
    //     GL_DEBUG_SEVERITY_NOTIFICATION,
    //     0,
    //     0,
    //     GL_FALSE
    // );
    // glDebugMessageCallback(onDebugMessage, 0);
    glutMainLoop();

    return 0;
}