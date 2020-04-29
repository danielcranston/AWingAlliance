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

constexpr float FOV_Y = glm::radians(45.0f);
uint SCREEN_W, SCREEN_H;

std::chrono::system_clock::time_point t;
const std::chrono::system_clock::duration dt = std::chrono::milliseconds(10);
std::chrono::system_clock::time_point currentTime;
std::chrono::system_clock::duration accumulator;

std::unique_ptr<ScenarioParser> parser;
std::unique_ptr<Renderer> renderer;
std::unique_ptr<GameState> game_state;

void init()
{
    std::cout << "init" << '\n';

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
    game_state->GetCamera().attach_to(
        dynamic_cast<actor::Ship*>(game_state->GetShips().at(parser->player).get()));

    renderer->register_terrain(game_state->GetTerrain(), parser->terrain->textures);
    renderer->list_textures();

    currentTime = std::chrono::system_clock::now();
    accumulator = std::chrono::milliseconds(0);

    std::cout << "Finished Init" << '\n';
}

void onIdle()
{
    glutPostRedisplay();
}

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

    game_state->GetCamera().SetProjMatrix(
        glm::perspective(FOV_Y, 1.0f * SCREEN_W / SCREEN_H, 0.1f, 8192.0f));
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 2);
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);  // GLUT_DOUBLE
    SCREEN_W = 1200;
    SCREEN_H = 900;
    glutInitWindowSize(SCREEN_W, SCREEN_H);
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
    // glutTimerFunc(1000/FPS, &onTimer, 0);
    glutMainLoop();

    return 0;
}