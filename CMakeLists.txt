cmake_minimum_required(VERSION 3.12.4)
project(AWingAlliance LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_EXAMPLES "Build examples" OFF)

find_package(Eigen3 3.3 REQUIRED)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

find_package(Alure2 REQUIRED)

find_package(assimp REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(yaml-cpp REQUIRED)
include_directories(${YAML_INCLUDE_DIRS})

add_subdirectory(${PROJECT_SOURCE_DIR}/entt)

include_directories(${PROJECT_SOURCE_DIR}/entt/src ${EIGEN3_INCLUDE_DIR}
                    ${PROJECT_SOURCE_DIR}/include)

# Create the absolute location header to locate resources
get_filename_component(CMAKE_ABSOLUTE_DATA_DIR "${CMAKE_SOURCE_DIR}/data"
                       ABSOLUTE)
configure_file("include/resources/locator.h.in"
               "${CMAKE_SOURCE_DIR}/include/resources/locator.h")

add_library(
  resources
  src/resources/load_model.cpp src/resources/load_texture.cpp
  src/resources/load_textfile.cpp src/resources/load_descriptions.cpp
  src/resources/load_geometry.cpp)
target_link_libraries(resources ${ASSIMP_LIBRARIES} ${YAML_CPP_LIBRARIES})

add_library(
  rendering
  src/rendering/mesh.cpp
  src/rendering/texture.cpp
  src/rendering/shader_program.cpp
  src/rendering/context_manager.cpp
  src/rendering/compile_shader_program.cpp
  src/rendering/model.cpp
  src/rendering/primitives.cpp
  src/rendering/rendering_manager.cpp
  src/rendering/draw.cpp)
target_link_libraries(rendering Eigen3::Eigen ${OPENGL_LIBRARIES}
                      ${GLEW_LIBRARIES})

add_library(audio src/audio/audio_manager.cpp)
target_link_libraries(audio alure2)

add_library(geometry SHARED src/geometry/geometry.cpp
                            src/geometry/collision.cpp)
target_link_libraries(geometry Eigen3::Eigen)

add_library(
  control
  src/control/fire_control.cpp
  src/control/motion_control.cpp
  src/control/motion_model.cpp
  src/control/lqr.cpp
  src/control/position_controller.cpp
  src/control/velocity_controller.cpp
  src/control/orientation_controller.cpp
  src/control/camera_controller.cpp
  src/control/ship_controller.cpp)
target_link_libraries(control geometry Eigen3::Eigen)

add_library(actors src/actor/actor.cpp src/actor/ship.cpp src/actor/laser.cpp
                   src/actor/camera.cpp src/actor/billboard.cpp)
target_link_libraries(actors geometry control Eigen3::Eigen)

add_library(ecs src/scene.cpp src/ecs/resource_manager.cpp src/ecs/systems)

if("${BUILD_EXAMPLES}")
  add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif()

add_executable(awing src/main2.cpp)

target_link_libraries(
  awing
  ecs
  rendering
  resources
  audio
  actors
  ecs
  ${SDL2_LIBRARIES}
  Eigen3::Eigen)
