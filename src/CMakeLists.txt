set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_library(loaders 
	TSBK07/loadobj.c
	TSBK07/LoadTGA.c
	TSBK07/GL_utilities.c)

target_link_libraries(loaders ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
target_link_libraries(loaders -lglut)


add_executable(awing main.cpp actor.cpp keyboard.cpp)

target_link_libraries(awing ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
target_link_libraries(awing -lglut)
target_link_libraries(awing loaders)
