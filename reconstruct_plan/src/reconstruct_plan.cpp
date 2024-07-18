// References:
// [1] https://docs.ros.org/en/kinetic/api/moveit_ros_planning_interface/html/classmoveit_1_1planning__interface_1_1MoveGroupInterface.html

#include <math.h>
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>

int main(int argc, char* argv[])
{
  // Initialize ROS and create the Node
  rclcpp::init(argc, argv);
  auto const node = std::make_shared<rclcpp::Node>(
      "reconstruct_plan_node", rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

  // Create a ROS logger
  auto const logger = rclcpp::get_logger("reconstruct_plan_node");

  // Create the MoveIt MoveGroup Interface
  using moveit::planning_interface::MoveGroupInterface;
  auto move_group_interface = MoveGroupInterface(node, "mh5_arm");
  // move_group_interface.allowReplanning(true);
  // move_group_interface.setNumPlanningAttempts(10);



  // Set a target Pose
  // auto const target_pose = [] {
  //   geometry_msgs::msg::Pose msg;
  //   msg.orientation.w = 1.0;
  //   msg.position.x = 0.28;
  //   msg.position.y = -0.2;
  //   msg.position.z = 0.5;
  //   return msg;
  // }();
  // move_group_interface.setPoseTarget(target_pose);

  std::vector<std::vector<double>> scan_poses_joint_values = {
    {-34.0, 3.0, 31.0, 34.0, -115.0, 142.0},  //left_1
    // {-92.0, -36.0, 16.0, -1.0, -114.0, 181.0},  //front_1
    {-91.0, -28.0, 23.0, -1.0, -112.0, 181.0},  // top_1
    // {-90.0, 19.0, 57.0, 0.0, 125.0, 185.0}, //back_1
    {-139.0, -8.0, 33.0, -30.0, -115.0, 214.0}, //right_1
    // {-34.0, 3.0, 31.0, 34.0, -115.0, 142.0},
    // {-34.0, 3.0, 31.0, 34.0, -115.0, 142.0},
  };

  for (int i = 0; i < scan_poses_joint_values.size(); i++) {

    // std::vector<double> joint_values = {-34.0, 3.0, 31.0, 34.0, -115.0, 142.0};
    for (int j = 0; j < scan_poses_joint_values[i].size(); j++) {
        scan_poses_joint_values[i][j] *= M_PI / 180.0;
    }
    move_group_interface.setStartStateToCurrentState();
    move_group_interface.setJointValueTarget(scan_poses_joint_values[i]);
    // move_group_interface.setMaxVelocityScalingFactor(0.0001);


    // const moveit::core::JointModelGroup* joint_model_group = move_group_interface.getCurrentState()->getJointModelGroup("mh5_arm");
    // moveit::core::RobotStatePtr current_state = move_group_interface.getCurrentState();

    // move_group_interface.setMaxVelocityScalingFactor(0.05);
    // move_group_interface.setMaxAccelerationScalingFactor(0.05);

    // Create a plan to that target pose
    auto const[success, plan] = [&move_group_interface] {
      moveit::planning_interface::MoveGroupInterface::Plan msg;
      auto const ok = static_cast<bool>(move_group_interface.plan(msg));
      return std::make_pair(ok, msg);
    }();

    // Execute the plan
    if (success)
    {
      RCLCPP_INFO(logger, "Planning %d start!", i);
      move_group_interface.execute(plan);
      RCLCPP_INFO(logger, "Planning %d End!", i);
      // Sleep for 0.5 second
      rclcpp::sleep_for(std::chrono::seconds(0.5));
    }
    else
    {
      RCLCPP_ERROR(logger, "Planning failed!");
    }
  }

  // Shutdown ROS
  rclcpp::shutdown();
  return 0;
}