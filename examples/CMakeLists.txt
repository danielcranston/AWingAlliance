add_executable(hyperspace_example hyperspace_example.cpp)
target_link_libraries(hyperspace_example rendering resources audio ${SDL2_LIBRARIES} Eigen3::Eigen)

add_executable(fire_control_example fire_control_example.cpp)
target_link_libraries(fire_control_example actors resources audio ${SDL2_LIBRARIES})

add_executable(control_example control_example.cpp)
target_link_libraries(control_example rendering actors resources ${SDL2_LIBRARIES} Eigen3::Eigen)
