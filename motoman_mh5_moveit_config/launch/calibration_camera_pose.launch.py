""" Static transform publisher acquired via MoveIt 2 hand-eye calibration """
""" EYE-IN-HAND: robotiq_85_base_link -> camera_color_optical_frame """
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description() -> LaunchDescription:
    nodes = [
        Node(
            package="tf2_ros",
            executable="static_transform_publisher",
            output="log",
            arguments=[
                "--frame-id",
                "robotiq_85_base_link",
                "--child-frame-id",
                "camera_color_optical_frame",
                "--x",
                "0.0205807",
                "--y",
                "-0.0437127",
                "--z",
                "-0.0400177",
                "--qx",
                "0.0167113",
                "--qy",
                "0.0207555",
                "--qz",
                "0.99632",
                "--qw",
                "0.0814677",
                # "--roll",
                # "3.10292",
                # "--pitch",
                # "3.1049",
                # "--yaw",
                # "-0.162465",
            ],
        ),
    ]
    return LaunchDescription(nodes)
