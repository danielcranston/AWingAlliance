set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_library(actors actor/actor.cpp
                   actor/camera.cpp
                   actor/ship.cpp
                   actor/laser.cpp
                   actor/billboard.cpp)

add_executable(awing main.cpp
                     model.cpp
                     keyboard.cpp
                     shaders.cpp
                     terrain.cpp
                     spline.cpp
                     parser.cpp
                     renderer.cpp
                     game_state.cpp
                     fbo.cpp
                     behavior/ship_controller.cpp
                     behavior/nodes.cpp)

target_link_libraries(awing actors
                            ${OPENGL_LIBRARIES}
                            ${GLEW_LIBRARIES}
                            ${BEHAVIOR_TREE_LIBRARY})

target_link_libraries(awing -lglut)
