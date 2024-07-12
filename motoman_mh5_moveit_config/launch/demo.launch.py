from moveit_configs_utils import MoveItConfigsBuilder
# from moveit_configs_utils.launches import generate_demo_launch
from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    IncludeLaunchDescription,
)
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration

from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue

from srdfdom.srdf import SRDF

from moveit_configs_utils.launch_utils import (
    add_debuggable_node,
    DeclareBooleanLaunchArg,
)

import launch
from launch.substitutions import (
    Command,
    FindExecutable,
    LaunchConfiguration,
    PathJoinSubstitution,
)
from launch.conditions import IfCondition
import launch_ros
import os

def generate_launch_description():
    moveit_config = MoveItConfigsBuilder("motoman_mh5", package_name="motoman_mh5_moveit_config").to_moveit_configs()
#     return generate_demo_launch(moveit_config)
    """
    Launches a self contained demo

    launch_package_path is optional to use different launch and config packages

    Includes
     * static_virtual_joint_tfs
     * robot_state_publisher
     * move_group
     * moveit_rviz
     * warehouse_db (optional)
     * ros2_control_node + controller spawners
    """
    launch_package_path = None
    if launch_package_path == None:
        launch_package_path = moveit_config.package_path

    ld = LaunchDescription()

    # https://github.com/tc-huang/ros2_robotiq_gripper/blob/main/robotiq_description/launch/robotiq_control.launch.py
    description_pkg_share = launch_ros.substitutions.FindPackageShare(
        package="robotiq_description"
    ).find("robotiq_description")

    default_model_path = os.path.join(
        description_pkg_share, "urdf", "robotiq_2f_85_gripper.urdf.xacro"
    )

    ld.add_action(
        launch.actions.DeclareLaunchArgument(
            name="model",
            default_value=default_model_path,
            description="Absolute path to gripper URDF file",
        )
    )

    # arm_description_pkg_share = launch_ros.substitutions.FindPackageShare(
    #     package="motoman_mh5_moveit_config"
    # ).find("motoman_mh5_moveit_config")

    # arm_default_model_path = os.path.join(
    #     arm_description_pkg_share, "config", "motoman_mh5.urdf.xacro"
    # )

    # ld.add_action(
    #     launch.actions.DeclareLaunchArgument(
    #         name="model",
    #         default_value=arm_default_model_path,
    #         description="Absolute path to gripper URDF file",
    #     )
    # )


    robot_description_content = Command(
        [
            PathJoinSubstitution([FindExecutable(name="xacro")]),
            " ",
            LaunchConfiguration("model"),
            " ",
            "use_fake_hardware:=false",
        ]
    )


    robot_description_param = {
        "robot_description": launch_ros.parameter_descriptions.ParameterValue(
            robot_description_content, value_type=str
        )
    }

    update_rate_config_file = PathJoinSubstitution(
        [
            description_pkg_share,
            "config",
            "robotiq_update_rate.yaml",
        ]
    )

    controllers_file = "robotiq_controllers.yaml"
    initial_joint_controllers = PathJoinSubstitution(
        [description_pkg_share, "config", controllers_file]
    )
    """
    """
    ld.add_action(
        Node(
            package="controller_manager",
            executable="ros2_control_node",
            parameters=[
                robot_description_param,
                update_rate_config_file,
                initial_joint_controllers,
            ],
            # parameters=[
            #     # str(moveit_config.package_path / "config/ros2_controllers.yaml"),
            #     initial_joint_controllers
            # ],
            # remappings=[
            #     ("/controller_manager/robot_description", "/gripper_description"),
            # ],
        )
    )

    ld.add_action(
        Node(
            package="robot_state_publisher",
            executable="robot_state_publisher",
            parameters=[robot_description_param],
        )
    )

    ld.add_action(
        Node(
            package="controller_manager",
            executable="spawner",
            arguments=[
                "gripper_joint_state_broadcaster",
                "--controller-manager",
                "/controller_manager",
            ],
        )
    )

    ld.add_action(
        Node(
            package="controller_manager",
            executable="spawner",
            arguments=["robotiq_gripper_controller", "-c", "/controller_manager"],
        )
    )

    ld.add_action(
        Node(
            package="controller_manager",
            executable="spawner",
            arguments=["robotiq_activation_controller", "-c", "/controller_manager"],
        )
    )
    ################################################################################################3
    ld.add_action(
        DeclareBooleanLaunchArg(
            "db",
            default_value=False,
            description="By default, we do not start a database (it can be large)",
        )
    )
    ld.add_action(
        DeclareBooleanLaunchArg(
            "debug",
            default_value=False,
            description="By default, we are not in debug mode",
        )
    )
    ld.add_action(DeclareBooleanLaunchArg("use_rviz", default_value=True))
    # If there are virtual joints, broadcast static tf by including virtual_joints launch
    virtual_joints_launch = (
        launch_package_path / "launch/static_virtual_joint_tfs.launch.py"
    )

    if virtual_joints_launch.exists():
        ld.add_action(
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(str(virtual_joints_launch)),
            )
        )

    # Given the published joint states, publish tf for the robot links
    ld.add_action(
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                str(launch_package_path / "launch/rsp.launch.py")
            ),
        )
    )

    ld.add_action(
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                str(launch_package_path / "launch/move_group.launch.py")
            ),
        )
    )

    """
    """
    # Run Rviz and load the default config to see the state of the move_group node
    ld.add_action(
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                str(launch_package_path / "launch/moveit_rviz.launch.py")
            ),
            condition=IfCondition(LaunchConfiguration("use_rviz")),
        )
    )

    # If database loading was enabled, start mongodb as well
    # ld.add_action(
    #     IncludeLaunchDescription(
    #         PythonLaunchDescriptionSource(
    #             str(launch_package_path / "launch/warehouse_db.launch.py")
    #         ),
    #         condition=IfCondition(LaunchConfiguration("db")),
    #     )
    # )

    # Fake joint driver
    ld.add_action(
        Node(
            package="controller_manager",
            executable="spawner",
            arguments=[
                "joint_state_broadcaster",
                "--controller-manager",
                "/controller_manager",
            ],
        )
    )

    ld.add_action(
        Node(
            package="controller_manager",
            executable="spawner",
            arguments=["robotiq_gripper_controller", "-c", "/controller_manager"],
        )
    )
    ld.add_action(
        Node(
            package="controller_manager",
            executable="spawner",
            arguments=["robotiq_activation_controller", "-c", "/controller_manager"],
            remappings=[
                    ("/controller_manager/robot_description", "/robot_description"),
                ],
        )
    )

    # ld.add_action(
    #     IncludeLaunchDescription(
    #         PythonLaunchDescriptionSource(
    #             str(launch_package_path / "launch/spawn_controllers.launch.py")
    #         ),
    #     )
    # )
    ld.add_action(
        Node(
            package="controller_manager",
            executable="spawner",
            arguments=["mh5_arm_controller", "-c", "/controller_manager"],
        )
    )



    return ld


