
rm -rf log build install
colcon build
. install/setup.bash

ros2 launch pega_pega turtle.launch.py

