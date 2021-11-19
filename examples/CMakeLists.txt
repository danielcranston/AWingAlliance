add_executable(hyperspace_example EXCLUDE_FROM_ALL hyperspace_example.cpp)
target_link_libraries(hyperspace_example rendering resources audio ${SDL2_LIBRARIES} Eigen3::Eigen)

add_executable(fire_control_example EXCLUDE_FROM_ALL fire_control_example.cpp)
target_link_libraries(fire_control_example actors resources audio ${SDL2_LIBRARIES})

add_executable(control_example EXCLUDE_FROM_ALL control_example.cpp)
target_link_libraries(control_example rendering actors resources ${SDL2_LIBRARIES} Eigen3::Eigen)

add_executable(scribbles EXCLUDE_FROM_ALL scribbles.cpp)
target_link_libraries(scribbles Eigen3::Eigen ${ASSIMP_LIBRARIES})
