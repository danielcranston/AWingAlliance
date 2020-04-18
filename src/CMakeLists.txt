set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_executable(awing main.cpp actor.cpp model.cpp keyboard.cpp shaders.cpp terrain.cpp spline.cpp parser.cpp renderer.cpp fighter.cpp)

target_link_libraries(awing ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
target_link_libraries(awing -lglut)
