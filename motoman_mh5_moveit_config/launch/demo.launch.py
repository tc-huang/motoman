# References:
# [1] https://github.com/moveit/moveit_resources/blob/ros2/panda_moveit_config/launch/demo.launch.py
import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch.conditions import IfCondition
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from ament_index_python.packages import get_package_share_directory
from moveit_configs_utils import MoveItConfigsBuilder
from launch.actions import IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource


def generate_launch_description():
        
    warehouse_ros_config = {
        "warehouse_plugin": "warehouse_ros_sqlite::DatabaseConnection",
        "warehouse_host": "/motoman/my_database.sqlite",
    }
    # Command-line arguments
    rviz_config_arg = DeclareLaunchArgument(
        "rviz_config",
        default_value="moveit.rviz",
        description="RViz configuration file",
    )

    db_arg = DeclareLaunchArgument(
        "db", default_value="True", description="Database flag"
        # "db", default_value="False", description="Database flag"
    )

    # ros2_control_hardware_type = DeclareLaunchArgument(
    #     "ros2_control_hardware_type",
    #     default_value="mock_components",
    #     description="ROS 2 control hardware interface type to use for the launch file -- possible values: [mock_components, isaac]",
    # )

    moveit_config = (
        MoveItConfigsBuilder("motoman_mh5")
        .robot_description(
            file_path="config/motoman_mh5.urdf.xacro",
            # mappings={
            #     "ros2_control_hardware_type": LaunchConfiguration(
            #         "ros2_control_hardware_type"
            #     )
            # },
        )
        .robot_description_semantic(file_path="config/motoman_mh5.srdf")
        # .planning_scene_monitor(
        #     publish_robot_description=True, publish_robot_description_semantic=True
        # )
        # .trajectory_execution(file_path="config/ros2_controllers.yaml")
        # .planning_pipelines(
        #     pipelines=["ompl", "chomp", "pilz_industrial_motion_planner"]#, "stomp"]
        # )
        .to_moveit_configs()
    )

    # Start the actual move_group node/action server
    move_group_node = Node(
        package="moveit_ros_move_group",
        executable="move_group",
        output="screen",
        parameters=[moveit_config.to_dict(), warehouse_ros_config],
        arguments=["--ros-args", "--log-level", "info"],
    )

    # RViz
    rviz_base = LaunchConfiguration("rviz_config")
    rviz_config = PathJoinSubstitution(
        [FindPackageShare("motoman_mh5_moveit_config"), "config", rviz_base]
    )
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="log",
        arguments=["-d", rviz_config],
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.planning_pipelines,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            warehouse_ros_config
        ],
    )

    # Static TF
    static_tf_node = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="static_transform_publisher",
        output="log",
        arguments=["0.0", "0.0", "0.0", "0.0", "0.0", "0.0", "world", "base_link"],
    )

    # Publish TF
    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        output="both",
        parameters=[moveit_config.robot_description],
    )

    # ros2_control using FakeSystem as hardware
    ros2_controllers_path = os.path.join(
        get_package_share_directory("motoman_mh5_moveit_config"),
        "config",
        "ros2_controllers.yaml",
    )

    ros2_control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[ros2_controllers_path],
        remappings=[
            ("/controller_manager/robot_description", "/robot_description"),
        ],
        output="screen",
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_state_broadcaster",
            "--controller-manager",
            "/controller_manager",
        ],
    )

    panda_arm_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["mh5_arm_controller", "-c", "/controller_manager"],
    )

    panda_hand_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["robotiq_gripper_controller", "-c", "/controller_manager"],
    )

    # Warehouse mongodb server
    db_config = LaunchConfiguration("db")
    # mongodb_server_node = Node(
    #     package="warehouse_ros_mongo",
    #     executable="mongo_wrapper_ros.py",
    #     parameters=[
    #         {"warehouse_port": 33829},
    #         {"warehouse_host": "localhost"},
    #         {"warehouse_plugin": "warehouse_ros_mongo::MongoDatabaseConnection"},
    #     ],
    #     output="screen",
    #     condition=IfCondition(db_config),
    # )

    camera_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            os.path.join(
                get_package_share_directory('realsense2_camera'),
                'launch',
                'rs_launch.py'
            )
        ]),
        launch_arguments={
            # 'enable_rgbd': 'true',
            'enable_sync': 'true',
            'align_depth.enable': 'true',
            'enable_color': 'true',
            # 'enable_depth': 'true',
            'rgb_camera.color_profile': '640x480x30',
            'temporal_filter.enable':'true',
            'hole_filling_filter.enable':'true',
        }.items(),
    )

    # disparity_filter - convert depth to disparity before applying other filters and back.
    # spatial_filter - filter the depth image spatially.
    # temporal_filter - filter the depth image temporally.
    # hole_filling_filter - apply hole-filling filter.
    # decimation_filter - reduces depth scene complexity.


    delayed_camera_launch = TimerAction(
        period=5.0,
        actions=[camera_launch]
    )


    camera_pose_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(
        get_package_share_directory('motoman_mh5_moveit_config'), 'launch',
        'calibration_camera_pose.launch.py')
        )
    )
    delayed_camera_pose_launch = TimerAction(
        period=10.0,
        actions=[camera_pose_launch]
    )



    # realsense = Node(
    #     package="realsense2_camera",
    #     executable="realsense2_camera_node",
    #     arguments=[],
    #     namespace="my_camera"
    # )

    return LaunchDescription(
        [
            rviz_config_arg,
            db_arg,
            # ros2_control_hardware_type,
            rviz_node,
            static_tf_node,
            robot_state_publisher,
            move_group_node,
            ros2_control_node,
            joint_state_broadcaster_spawner,
            panda_arm_controller_spawner,
            panda_hand_controller_spawner,
            # mongodb_server_node,
            # camera_pose,
            delayed_camera_launch,
            delayed_camera_pose_launch
            # realsense
        ]
    )