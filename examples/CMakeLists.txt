add_executable(hyperspace_example hyperspace_example.cpp)
target_link_libraries(hyperspace_example rendering resources audio
                      ${SDL2_LIBRARIES} Eigen3::Eigen)

add_executable(motionstate_control_example motionstate_control_example.cpp)
target_link_libraries(motionstate_control_example ecs urdf rendering resources
                      control)
