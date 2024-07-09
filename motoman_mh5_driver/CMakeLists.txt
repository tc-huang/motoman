# References:
# [1] https://github.com/ros-controls/ros2_control_demos/blob/eb8733c588aecca58d491c8389c1553880942bda/example_1/CMakeLists.txt

cmake_minimum_required(VERSION 3.8)
project(motoman_mh5_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
set(THIS_PACKAGE_INCLUDE_DEPENDS
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
)
find_package(ament_cmake REQUIRED)
foreach(Dependency IN ITEMS ${THIS_PACKAGE_INCLUDE_DEPENDS})
  find_package(${Dependency} REQUIRED)
endforeach()
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

## COMPILE
add_library(
  motoman_mh5_driver
  SHARED
  src/motoman_mh5_driver.cpp
  src/control.cpp
)
target_compile_features(motoman_mh5_driver PUBLIC cxx_std_17)
target_include_directories(motoman_mh5_driver PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include/motoman_mh5_driver>
)
ament_target_dependencies(
  motoman_mh5_driver PUBLIC
  ${THIS_PACKAGE_INCLUDE_DEPENDS}
)

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME} PRIVATE "MOTOMAN_MH5_DRIVER_BUILDING_DLL")

# Export hardware plugins
pluginlib_export_plugin_description_file(hardware_interface motoman_mh5_driver.xml)

# INSTALL
install(
  DIRECTORY include/
  DESTINATION include/motoman_mh5_driver
)
install(
  DIRECTORY include/
  DESTINATION include/control
)
# install(
#   DIRECTORY description/launch description/ros2_control description/urdf
#   DESTINATION share/motoman_mh5_driver
# )
# install(
#   DIRECTORY bringup/launch bringup/config
#   DESTINATION share/motoman_mh5_driver
# )
install(TARGETS motoman_mh5_driver
  EXPORT export_motoman_mh5_driver
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

## EXPORTS
ament_export_targets(export_motoman_mh5_driver HAS_LIBRARY_TARGET)
ament_export_dependencies(${THIS_PACKAGE_INCLUDE_DEPENDS})
ament_package()
