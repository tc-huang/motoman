cmake_minimum_required(VERSION 3.0.2)

project(motoman_gp25_support)

find_package(catkin REQUIRED)

catkin_package()

if (CATKIN_ENABLE_TESTING)
  find_package(roslaunch REQUIRED)
  roslaunch_add_file_check(test/roslaunch_test_gp25.xml)
  roslaunch_add_file_check(test/roslaunch_test_gp25_12.xml)
endif()

install(DIRECTORY config launch meshes urdf
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})
