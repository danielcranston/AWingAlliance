project(AWingAlliance)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

# find_path : finds header files
# find_library : finds libraries
# find_package : finds some predefined file that does a bunch of the two above

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(    ${PROJECT_SOURCE_DIR}/include
                        ${PROJECT_SOURCE_DIR}/include/external)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)


# add_library(mylib SHARED IMPORTED)
# set_property(TARGET mylib
#               PROPERTY IMPORTED_LOCATION
#               "${LIBRARY_OUTPUT_PATH}/mylib.so")

