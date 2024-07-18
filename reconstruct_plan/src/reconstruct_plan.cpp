// References:
// [1] https://docs.ros.org/en/kinetic/api/moveit_ros_planning_interface/html/classmoveit_1_1planning__interface_1_1MoveGroupInterface.html

#include <math.h>
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp/executor.hpp>

#include <moveit/move_group_interface/move_group_interface.h>
#include <industrial_reconstruction_msgs/srv/start_reconstruction.hpp>
#include <industrial_reconstruction_msgs/srv/stop_reconstruction.hpp>
#include <industrial_reconstruction_msgs/srv/flag.hpp>

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
  
  // Create a client to call the /start_reconstruction service
  auto client = node->create_client<industrial_reconstruction_msgs::srv::StartReconstruction>("/start_reconstruction");

  // Wait for the service to be available
  while (!client->wait_for_service(std::chrono::seconds(1))) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(logger, "Interrupted while waiting for the service. Exiting.");
      return 1;
    }
    RCLCPP_INFO(logger, "Service not available, waiting again...");
  }

  auto client_stop = node->create_client<industrial_reconstruction_msgs::srv::StopReconstruction>("/stop_reconstruction");
  // Wait for the service to be available
  while (!client_stop->wait_for_service(std::chrono::seconds(1))) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(logger, "Interrupted while waiting for the service. Exiting.");
      return 1;
    }
    RCLCPP_INFO(logger, "Service not available, waiting again...");
  }

  auto client_flag = node->create_client<industrial_reconstruction_msgs::srv::Flag>("/flag");
  // Wait for the service to be available
  while (!client_flag->wait_for_service(std::chrono::seconds(1))) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(logger, "Interrupted while waiting for the service. Exiting.");
      return 1;
    }
    RCLCPP_INFO(logger, "Service not available, waiting again...");
  }

  // Create a request
  auto request = std::make_shared<industrial_reconstruction_msgs::srv::StartReconstruction::Request>();
  request->tracking_frame = "camera_color_optical_frame";
  request->relative_frame = "world";
  request->translation_distance = 0.1;
  request->rotational_distance = 0.1;
  request->live = true;
  request->tsdf_params.voxel_length = 0.002;
  request->tsdf_params.sdf_trunc = 0.1;
  request->tsdf_params.min_box_values.x = 0;
  request->tsdf_params.min_box_values.y = 0;
  request->tsdf_params.min_box_values.z = 0;
  request->tsdf_params.max_box_values.x = 0;
  request->tsdf_params.max_box_values.y = 0;
  request->tsdf_params.max_box_values.z = 0;
  request->rgbd_params.depth_scale = 1000.0;
  request->rgbd_params.depth_trunc = 0.7;
  request->rgbd_params.convert_rgb_to_intensity = false;

    // Call the service and wait for the response
  auto future = client->async_send_request(request);

  if (rclcpp::spin_until_future_complete(node, future) ==
      rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(logger, "Reconstruction started successfully");
  } else {
    RCLCPP_ERROR(logger, "Failed to start reconstruction");
  }
  

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
      rclcpp::sleep_for(std::chrono::seconds(2));
      // Create a request
      auto request_flag = std::make_shared<industrial_reconstruction_msgs::srv::Flag::Request>();
      request_flag->flag = true;

      // Call the service and wait for the response
      auto future_flag = client_flag->async_send_request(request_flag);

      if (rclcpp::spin_until_future_complete(node, future_flag) == rclcpp::FutureReturnCode::SUCCESS){
        RCLCPP_INFO(logger, "Reconstruction started successfully");
      } else {
        RCLCPP_ERROR(logger, "Failed to start reconstruction");
      }
    }
    else
    {
      RCLCPP_ERROR(logger, "Planning failed!");
    }
  }

  
  // Create a request
  auto request_stop = std::make_shared<industrial_reconstruction_msgs::srv::StopReconstruction::Request>();
  request_stop->archive_directory = "/motoman/industrial_reconstruction_archive/archive";
  request_stop->mesh_filepath = "/motoman/industrial_reconstruction_archive/results_mesh.ply";
  
  // Add normal filters
  industrial_reconstruction_msgs::msg::NormalFilterParams normal_filter;
  normal_filter.normal_direction.x = 0.0;
  normal_filter.normal_direction.y = 0.0;
  normal_filter.normal_direction.z = 1.0;
  normal_filter.angle = 90.0;
  request_stop->normal_filters.push_back(normal_filter);

  request_stop->min_num_faces = 1000;

  // Call the service and wait for the response
  auto future_stop = client_stop->async_send_request(request_stop);

  // Wait for the response
  if (rclcpp::spin_until_future_complete(node, future_stop) ==
    rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(node->get_logger(), "Service call succeeded");
  } else {
    RCLCPP_ERROR(node->get_logger(), "Failed to call service");
  }

  // Shutdown ROS
  rclcpp::shutdown();
  return 0;
}