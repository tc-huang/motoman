""" Static transform publisher acquired via MoveIt 2 hand-eye calibration """
""" EYE-IN-HAND: link_t -> camera_color_optical_frame """
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
                "link_t",
                "--child-frame-id",
                "camera_color_optical_frame",
                "--x",
                "2.06982e-05",
                "--y",
                "-0.0505135",
                "--z",
                "0.186187",
                "--qx",
                "4.70777e-18",
                "--qy",
                "3.44107e-18",
                "--qz",
                "6.16893e-18",
                "--qw",
                "1",
                # "--roll",
                # "9.41553e-18",
                # "--pitch",
                # "6.88213e-18",
                # "--yaw",
                # "1.23379e-17",
            ],
        ),
    ]
    return LaunchDescription(nodes)
