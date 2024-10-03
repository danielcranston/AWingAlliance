add_executable(hyperspace_fsm_example hyperspace_fsm_example.cpp)
target_link_libraries(hyperspace_fsm_example rendering resources audio
                      ${SDL2_LIBRARIES} Eigen3::Eigen ecs)

add_executable(motionstate_control_example motionstate_control_example.cpp)
target_link_libraries(motionstate_control_example ecs urdf rendering resources
                      control)

find_package(ompl REQUIRED)

add_executable(ompl_example ompl_example.cpp)
target_include_directories(ompl_example PUBLIC ${OMPL_INCLUDE_DIRS})
target_link_libraries(ompl_example ${OMPL_LIBRARIES})
