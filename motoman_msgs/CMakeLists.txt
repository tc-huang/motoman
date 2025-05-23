cmake_minimum_required(VERSION 3.0.2)
project(motoman_msgs)

find_package(catkin
  REQUIRED COMPONENTS
    industrial_msgs
    message_generation
    std_msgs
    trajectory_msgs
)

add_message_files(
  DIRECTORY
    msg
  FILES
    DynamicJointPoint.msg
    DynamicJointState.msg
    DynamicJointTrajectory.msg
    DynamicJointTrajectoryFeedback.msg
    DynamicJointsGroup.msg
)

add_service_files(
  DIRECTORY
    srv
  FILES
    CmdJointTrajectoryEx.srv
    ReadGroupIO.srv
    ReadMRegister.srv
    ReadSingleIO.srv
    SelectTool.srv
    WriteGroupIO.srv
    WriteMRegister.srv
    WriteSingleIO.srv
)

generate_messages(
  DEPENDENCIES
    industrial_msgs
    std_msgs
    trajectory_msgs
)

catkin_package(
    CATKIN_DEPENDS
      industrial_msgs
      message_runtime
      std_msgs
      trajectory_msgs
)
