project(AWingAlliance)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(	${PROJECT_SOURCE_DIR}/include
						${PROJECT_SOURCE_DIR}/include/TSBK07)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)


# add_library(mylib SHARED IMPORTED)
# set_property(TARGET mylib
#				PROPERTY IMPORTED_LOCATION
#				"${LIBRARY_OUTPUT_PATH}/mylib.so")

