add_executable(hyperspace_example hyperspace_example.cpp)
target_link_libraries(hyperspace_example rendering resources audio ${SDL2_LIBRARIES} Eigen3::Eigen)
