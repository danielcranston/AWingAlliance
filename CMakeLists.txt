cmake_minimum_required(VERSION 3.12.4)
project(AWingAlliance LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(assimp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# Create the absolute location header to locate resources
get_filename_component(
  CMAKE_ABSOLUTE_DATA_DIR
  "${CMAKE_SOURCE_DIR}/data"
  ABSOLUTE
)
configure_file("include/data_handling.h.in" "${CMAKE_SOURCE_DIR}/include/data_handling.h")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(
  imgui
  thirdparty/imgui/imgui.cpp
  thirdparty/imgui/imgui_demo.cpp
  thirdparty/imgui/imgui_draw.cpp
  thirdparty/imgui/imgui_tables.cpp
  thirdparty/imgui/imgui_widgets.cpp
  thirdparty/imgui/backends/imgui_impl_opengl3.cpp
  thirdparty/imgui/backends/imgui_impl_sdl2.cpp
)
target_link_libraries(imgui ${SDL2_LIBRARIES})
target_include_directories(
  imgui SYSTEM
  PUBLIC
    ${PROJECT_SOURCE_DIR}/thirdparty
    ${PROJECT_SOURCE_DIR}/thirdparty/imgui
    ${PROJECT_SOURCE_DIR}/thirdparty/imgui/backends
)

add_library(
  rendering
  src/rendering/model.cpp
  src/rendering/texture.cpp
  src/rendering/shader_program.cpp
  src/rendering/render.cpp
  src/rendering/context_manager.cpp
  src/rendering/global.cpp
)
target_link_libraries(
  rendering
  PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${ASSIMP_LIBRARIES}
  PUBLIC Eigen3::Eigen imgui
)
target_compile_options(
  rendering
  PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror
)

add_executable(awing src/main.cpp)
target_link_libraries(
  awing
  rendering
  Eigen3::Eigen
  ${SDL2_LIBRARIES}
)

# Tests
include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_rendering test/test_rendering.cpp)
target_link_libraries(
  test_rendering
  rendering
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_rendering)
