cmake_minimum_required(VERSION 3.0.2)

project(motoman_mh5_support)

find_package(catkin REQUIRED)

catkin_package()

if (CATKIN_ENABLE_TESTING)
  find_package(roslaunch REQUIRED)
  roslaunch_add_file_check(test/launch_test_mh5.xml)
  roslaunch_add_file_check(test/launch_test_mh5l.xml)
endif()

install(DIRECTORY config launch meshes urdf
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})
