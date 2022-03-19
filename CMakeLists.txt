cmake_minimum_required(VERSION 3.12.4)
project(AWingAlliance LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_EXAMPLES "Build examples" OFF)

find_package(Eigen3 3.3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

find_package(assimp REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(yaml-cpp REQUIRED)
include_directories(${YAML_INCLUDE_DIRS})

option(ALURE_BUILD_EXAMPLES OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/alure)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/thirdparty/entt/src
  ${PROJECT_SOURCE_DIR}/thirdparty/alure/include
  ${PROJECT_SOURCE_DIR}/thirdparty/imgui
  ${PROJECT_SOURCE_DIR}/thirdparty/imgui/backends
  ${PROJECT_SOURCE_DIR}/thirdparty/implot
  ${PROJECT_SOURCE_DIR}/thirdparty)

# Create the absolute location header to locate resources
get_filename_component(CMAKE_ABSOLUTE_DATA_DIR "${CMAKE_SOURCE_DIR}/data"
                       ABSOLUTE)
configure_file("include/resources/locator.h.in"
               "${CMAKE_SOURCE_DIR}/include/resources/locator.h")

add_library(
  resources src/resources/load_model.cpp src/resources/load_texture.cpp
            src/resources/load_textfile.cpp src/resources/load_geometry.cpp)
target_link_libraries(resources ${ASSIMP_LIBRARIES} ${YAML_CPP_LIBRARIES})

add_library(
  imgui
  thirdparty/imgui/imgui.cpp
  thirdparty/imgui/imgui_demo.cpp
  thirdparty/imgui/imgui_draw.cpp
  thirdparty/imgui/imgui_tables.cpp
  thirdparty/imgui/imgui_widgets.cpp
  thirdparty/imgui/backends/imgui_impl_opengl3.cpp
  thirdparty/imgui/backends/imgui_impl_sdl.cpp
  thirdparty/implot/implot.cpp
  thirdparty/implot/implot_items.cpp
  thirdparty/implot/implot_demo.cpp)
target_link_libraries(imgui ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})

add_library(
  rendering
  src/rendering/mesh.cpp
  src/rendering/texture.cpp
  src/rendering/shader_program.cpp
  src/rendering/context_manager.cpp
  src/rendering/compile_shader_program.cpp
  src/rendering/model.cpp
  src/rendering/primitives.cpp
  src/rendering/draw.cpp)
target_link_libraries(rendering imgui Eigen3::Eigen ${OPENGL_LIBRARIES}
                      ${GLEW_LIBRARIES})
target_compile_options(rendering PRIVATE -Wall -Wextra -pedantic -Werror)

add_library(audio src/audio/audio_manager.cpp)
target_link_libraries(audio alure2)
target_include_directories(audio PUBLIC ${OPENAL_INCLUDE_DIR})

add_library(geometry SHARED src/geometry/geometry.cpp
                            src/geometry/collision.cpp)
target_link_libraries(geometry Eigen3::Eigen)
target_compile_options(geometry PRIVATE -Wall -Wextra -pedantic -Werror)

add_library(
  control
  src/control/motion_model.cpp
  src/control/lqr.cpp
  src/control/position_controller.cpp
  src/control/velocity_controller.cpp
  src/control/orientation_controller.cpp
  src/control/camera_controller.cpp
  src/control/ship_controller.cpp)
target_link_libraries(control geometry Eigen3::Eigen)
target_compile_options(control PRIVATE -Wall -Wextra -pedantic -Werror)

add_library(urdf thirdparty/external/tinyxml2.cpp src/urdf/parsing.cpp
                 src/urdf/parsing_utils src/urdf/fighter_input.cpp)
target_link_libraries(urdf Eigen3::Eigen)
target_compile_options(urdf PRIVATE -Wall -Wextra -pedantic -Werror)

add_library(
  ecs src/ecs/scene.cpp src/ecs/scene_factory.cpp src/ecs/resource_manager.cpp
      src/ecs/components.cpp src/ecs/systems.cpp)
target_link_libraries(ecs urdf rendering)
target_compile_options(ecs PRIVATE -Wall -Wextra -pedantic -Werror)

if("${BUILD_EXAMPLES}")
  add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif()

add_executable(awing src/main2.cpp)

target_link_libraries(
  awing
  ecs
  urdf
  rendering
  resources
  audio
  control
  ${SDL2_LIBRARIES}
  Eigen3::Eigen)
