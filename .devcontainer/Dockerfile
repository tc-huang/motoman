FROM ros:humble-ros-core-jammy

RUN apt-get update && apt-get install -y build-essential python3-vcstool git software-properties-common python3-rosdep python3-pip python3-colcon-common-extensions ros-dev-tools


# WORKDIR /motoman

# RUN vcs import src < my_dependencies_exact.repos


# RUN apt-get update && apt-get upgrade -y

RUN apt-get install -y ros-humble-rqt-console ros-humble-rqt-graph

RUN git clone https://github.com/tc-huang/ros2_robotiq_gripper.git -b main
WORKDIR /ros2_robotiq_gripper
RUN vcs import . < ros2_robotiq_gripper.humble.repos
RUN rosdep init && rosdep update && rosdep install --from-paths . -iry 

RUN . /opt/ros/humble/setup.sh && colcon build

RUN git clone https://github.com/tc-huang/motoman.git -b feature/ros2-mh5
RUN rosdep install --from-paths . -iry --skip-keys warehouse_ros_mongo

RUN apt update && apt install -y ros-humble-librealsense2* ros-humble-realsense2-*

WORKDIR /motoman


RUN echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc

# CMD ["bash", "-c", "source /opt/ros/humble/setup.bash && source /motoman/install/setup.bash"]