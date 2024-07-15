# build:
# 	@echo "Building..."
# 	. /opt/ros/humble/setup.sh && colcon build --cmake-args -DTESSERACT_BUILD_FCL=OFF -DBUILD_RENDERING=OFF

SETUP=. /opt/ros/humble/setup.sh && . /ros2_robotiq_gripper/install/setup.sh &&  . install/setup.sh
# SETUP=. /opt/ros/humble/setup.sh &&  . install/setup.sh
PROJECT_ROOT := $(shell pwd)

install:
	@echo "Installing..."
	pip install opencv-python
	apt-get update && $(SETUP) && rosdep install --from-paths . -iry --skip-keys warehouse_ros_mongo && apt-get install -y ros-humble-image-pipeline
# 	# sudo apt-get install -y gazebo libgazebo-dev
# 	# sudo apt-get install -y ros-humble-octomap
# 	rosdep install --from-paths src -iry --skip-keys libvtk --skip-keys fcl --skip-keys taskflow

clean:
	@echo "Cleaning..."
	rm -rf install log build

setup_assistant:
	@echo "Setup Assistant..."
	$(SETUP) && ros2 launch moveit_setup_assistant setup_assistant.launch.py

camera_calibration:
# 	https://wiki.ros.org/camera_calibration
#	https://docs.ros.org/en/rolling/p/camera_calibration/tutorial_mono.html
	@echo "Camera calibraion..."
#	108mm
	ros2 run camera_calibration cameracalibrator --size 11x8 --square 0.02 image:=/camera/camera/color/image_raw camera:=/cmaera
	cp /tmp/calibrationdata.tar.gz /motoman
	mkdir calibration
	tar -xzvf calibrationdata.tar.gz -C /motoman/calibration
	
camera_calibration_verify:
	ros2 run camera_calibration cameracheck.py --size 11x8 monocular:=/forearm image:=image_rect

image_proc:
# 	https://docs.ros.org/en/rolling/p/image_proc/index.html
# 	ros2 run image_proc image_proc --ros-args --remap namespace:=my_camera -r __ns:=/my_camera
	ros2 run image_proc image_proc --ros-args -r __ns:=/camera/camera/color --remap namespace:=camera/camera/color
#	https://github.com/IntelRealSense/realsense-ros/issues/855
# 	ROS_NAMESPACE=camera/camera/color ros2 run image_proc image_proc

camera_info:
	# echo
#	sensor_msgs/CameraInfo.msg
# 	python3 yaml_to_dict.py --file /motoman/calibration/camera_info.yaml --output /motoman/calibration/camera_info.txt
# 	ros2 topic pub /my_camera_info sensor_msgs/CameraInfo "$(cat /motoman/calibration/camera_info.txt)"
	$(SETUP) && ros2 run camera_info camera_info_publisher --ros-args -r __ns:=/my_camera

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
	git clone https://github.com/RIF-Robotics/moveit2_calibration.git
	git clone https://github.com/ros-industrial/industrial_reconstruction.git
	git clone https://github.com/moveit/warehouse_ros_mongo.git -b ros2
	git clone https://github.com/moveit/warehouse_ros_sqlite.git

sql:
	apt-get udpate && apt-get install sqlite3
	sqlite3 my_database.db

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
	$(SETUP) &&  ros2 launch motoman_mh5_moveit_config demo.launch.py db:=true

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
	ros2 launch realsense2_camera rs_launch.py enable_rgbd:=true enable_sync:=true align_depth.enable:=true enable_color:=true enable_depth:=true pointcloud.enable:=true
#  ros2 run realsense2_camera realsense2_camera_node
#  --ros-args --remap align_depth:=true --remap pointcloud:=true --remap colorizer:=true 
#  --ros-args -r /camera/camera/color/image_raw:=/camera/camera/color/image
#	--ros-args -r /camera/camera/color/image_raw:=/my_camera/image
camera_pose:
	$(SETUP) &&  ros2 launch motoman_mh5_moveit_config calibration_camera_pose.launch.py

reconstruct_launch:
	$(SETUP) &&  ros2 launch industrial_reconstruction reconstruction.launch.xml depth_image_topic:=/camera/camera/aligned_depth_to_color/image_raw color_image_topic:=/camera/camera/color/image_raw camera_info_topic:=/camera/camera/color/camera_info

reconstruct_start:
	$(SETUP) &&  ros2 service call /start_reconstruction industrial_reconstruction_msgs/srv/StartReconstruction "$(shell cat reconstruct_start_config.txt)"

reconstruct_stop:
	ros2 service call /stop_reconstruction industrial_reconstruction_msgs/srv/StopReconstruction "$(shell cat reconstruct_stop_config.txt)"


.PHONY: build clean install import launch graph console docker_build docker_exec test dep camera_calibration image_proc camera_info camera_pose