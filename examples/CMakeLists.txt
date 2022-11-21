add_executable(hyperspace_fsm_example hyperspace_fsm_example.cpp)
target_link_libraries(hyperspace_fsm_example rendering resources audio
                      ${SDL2_LIBRARIES} Eigen3::Eigen ecs)

add_executable(motionstate_control_example motionstate_control_example.cpp)
target_link_libraries(motionstate_control_example ecs urdf rendering resources
                      control)
