. /opt/ros/foxy/setup.bash

rm -rf log build install
colcon build
. install/setup.bash

ros2 launch collision_avoidance turtle.launch.py
