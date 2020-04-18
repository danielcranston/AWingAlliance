set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_library(actors actor/actor.cpp actor/fighter.cpp)

add_executable(awing main.cpp model.cpp keyboard.cpp shaders.cpp terrain.cpp spline.cpp parser.cpp renderer.cpp)

target_link_libraries(awing actors ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
target_link_libraries(awing -lglut)
