set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_library(actors actor/actor.cpp
                   actor/camera.cpp
                   actor/ship.cpp
                   actor/laser.cpp
                   actor/billboard.cpp)

add_library(behavior behavior/ship_controller.cpp
                     behavior/nodes.cpp
                     behavior/nodes/face_target.cpp
                     behavior/nodes/is_alive.cpp
                     behavior/nodes/roam_towards_destination.cpp
                     behavior/nodes/runnable_action_node.cpp
                     behavior/nodes/set_roaming_destination.cpp
                     behavior/nodes/set_target.cpp
                     behavior/nodes/tumble.cpp)


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
                     geometry.cpp)

target_link_libraries(awing actors
                            behavior
                            ${OPENGL_LIBRARIES}
                            ${GLEW_LIBRARIES}
                            ${BEHAVIOR_TREE_LIBRARY})

target_link_libraries(awing -lglut)
