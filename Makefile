# build:
# 	@echo "Building..."
# 	. /opt/ros/humble/setup.sh && colcon build --cmake-args -DTESSERACT_BUILD_FCL=OFF -DBUILD_RENDERING=OFF

SETUP=. /opt/ros/humble/setup.sh && . /ros2_robotiq_gripper/install/setup.sh &&  . install/setup.sh
# SETUP=. /opt/ros/humble/setup.sh &&  . install/setup.sh
PROJECT_ROOT := $(shell pwd)

install:
	@echo "Installing..."
	apt-get update && rosdep install --from-paths . -iry --skip-keys warehouse_ros_mongo
# 	# sudo apt-get install -y gazebo libgazebo-dev
# 	# sudo apt-get install -y ros-humble-octomap
# 	rosdep install --from-paths src -iry --skip-keys libvtk --skip-keys fcl --skip-keys taskflow

clean:
	@echo "Cleaning..."
	rm -rf install log build

setup_assistant:
	@echo "Setup Assistant..."
	$(SETUP) && ros2 launch moveit_setup_assistant setup_assistant.launch.py

# import:
# 	@echo "Import..."
# 	# Old
# 	# vcs import src < dependencies.repos
# 	# vcs import src < src/scan_n_plan_workshop/dependencies.repos
# 	# vcs import src < src/scan_n_plan_workshop/dependencies_tesseract.repos
	
# 	vcs import src < my_dependencies_exact.repos

# 	# taskflow (from the ROS-I PPA)
# 	# sudo add-apt-repository ppa:ros-industrial/ppa -y
# 	# sudo apt-get update
# 	# sudo apt-get install taskflow -y
# 	add-apt-repository ppa:ros-industrial/ppa -y
# 	apt-get update
# 	apt-get install taskflow -y

# launch:
# 	# source install/setup.bash
# 	ros2 launch snp_automate_2023 start.launch.xml sim_robot:=true sim_vision:=true
# 	# ros2 launch snp_automate_2023 start.launch.xml sim_robot:=false sim_vision:=flase

# exp_dep:
# 	vcs export --exact src > my_dependencies_exact.repos
# 	vcs export --exact-with-tags src > my_dependencies_exact_with_tags.repos
dep:
	@echo "Git clone..."
	git clone https://github.com/tc-huang/ros2_robotiq_gripper.git

urdf:
	@echo "URDF..."
	$(SETUP) && xacro /motoman/motoman_mh5_support/urdf/mh5_2f85.xacro > /motoman/motoman_mh5_support/urdf/mh5_2f85.urdf

graph:
	@echo "Graph..."
	$(SETUP) && ros2 run rqt_graph rqt_graph

console:
	@echo "Console..."
	$(SETUP) && ros2 run rqt_console rqt_console

source:
	@echo "Sourcing..."
	$(SETUP)

test:
	@echo "Building..."
	. /opt/ros/humble/setup.sh && . /ros2_robotiq_gripper/install/setup.sh && colcon build --packages-skip-regex ros2_control
	@echo "Demo..."
	# $(SETUP) &&  ros2 launch motoman_mh5_2f85_moveit_config demo.launch.py
	$(SETUP) &&  ros2 launch motoman_mh5_moveit_config demo.launch.py

build:
	@echo "Building..."
	. /opt/ros/humble/setup.sh && . /ros2_robotiq_gripper/install/setup.sh && colcon build --packages-skip-regex ros2_control

demo:
	@echo "Demo..."
	# $(SETUP) &&  ros2 launch motoman_mh5_2f85_moveit_config demo.launch.py
	$(SETUP) &&  ros2 launch motoman_mh5_moveit_config demo.launch.py

list_node:
	@echo "List Node..."
	$(SETUP) && ros2 node list

list_pkg:
	@echo "List Package..."
	$(SETUP) && ros2 pkg list

docker_build:
	@echo "Docker compose build"
	sudo docker compose -f "$(PROJECT_ROOT)/docker/docker-compose-gui-nvidia.yml" up --build

docker_exec:
	sudo docker exec -it ros2_humble_docker /bin/bash

node_camera:
	sudo docker exec -it ros2_humble_docker /bin/bash &&\
	ros2 run realsense2_camera realsense2_camera_node

.PHONY: build clean install import launch graph console docker_build docker_exec test dep