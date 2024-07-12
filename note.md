1. ros2 launch moveit_setup_assistant setup_assistant.launch.py
2. colcon build --packages-select motoman_mh5_moveit_config && source install/setup.bash
3. colcon build --packages-select motoman_mh5_2f85_moveit_config && source install/setup.bash
3. ros2 launch motoman_mh5_2f85_moveit_config demo.launch.py
4. apt-get install xorg openbox
5. g++ -o call_python call_python.cpp -I/usr/include/python3.10 -lpython3.10
6. rosdep install --from-paths src --ignore-src -r -y
7. ros2 pkg create --build-type ament_cmake --license Apache-2.0 motoman_mh5_ros2_control
8. ros2 control load_controller mh5_arm_controller
9. for node in $(ros2 node list); do
  ros2 lifecycle set $node finalized
done
