#include <iostream>
#include <math.h>
#include <algorithm>
#include <array>
#include <vector>
#include <memory>
#include <map>
#include <list>

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
#include <spline.h>
#include <parser.h>
#include <renderer.h>
#include <game_state.h>

constexpr float FOV_Y = glm::radians(45.0f);
uint SCREEN_W, SCREEN_H;

float t;
const float dt = 0.001666;  // length of one update step
float currentTime;
float accumulator;

std::unique_ptr<ScenarioParser> parser;
std::unique_ptr<Renderer> renderer;
std::unique_ptr<GameState> game_state;

void init()
{
    std::cout << "init" << '\n';

    parser = std::make_unique<ScenarioParser>("scenario1.json");
    parser->Parse();

    renderer = Renderer::Create(SCREEN_W, SCREEN_H);
    renderer->load_models(parser->required_models);
    renderer->register_skybox(parser->skybox);

    game_state = GameState::Create(renderer->GetModels());
    game_state->register_ships(parser->actors);
    game_state->register_terrain(parser->terrain.get());
    game_state->register_player(parser->player);

    const actor::Ship* player_ptr = game_state->GetShips().at(parser->player).get();
    game_state->SetCameraPlacementFunc([player_ptr]() -> std::pair<glm::vec3, glm::vec3> {
        const glm::vec3 pos = player_ptr->GetPosition() + 5.0f * player_ptr->GetDesiredDir() -
                              25.0f * player_ptr->GetDirection() + 5.0f * glm::vec3(0.0, 1.0, 0.0);
        const glm::vec3 dir = player_ptr->GetPosition() + 20.0f * player_ptr->GetDirection();
        return { pos, dir };
    });

    renderer->register_terrain(game_state->GetTerrain(), parser->terrain->textures);
    renderer->list_textures();

    t = 0.0f;
    currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    accumulator = 0.0f;

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
    float newTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float frameTime = newTime - currentTime;
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
    renderer->SetResolution(width, height);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 2);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);  // GLUT_DOUBLE
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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // glutTimerFunc(1000/FPS, &onTimer, 0);
    glutMainLoop();

    return 0;
}