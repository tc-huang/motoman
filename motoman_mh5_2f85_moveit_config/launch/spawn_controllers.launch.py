from moveit_configs_utils import MoveItConfigsBuilder
from moveit_configs_utils.launches import generate_spawn_controllers_launch


def generate_launch_description():
    moveit_config = MoveItConfigsBuilder("motoman_mh5_2f85", package_name="motoman_mh5_2f85_moveit_config").to_moveit_configs()
    return generate_spawn_controllers_launch(moveit_config)
