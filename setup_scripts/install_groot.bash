#! /usr/bin/bash

#
# OSU UWRT Groot (behaviortree editor) installation script
# Usage: ./install_groot.bash. Do not use sudo.
#

sudo apt install qtbase5-dev libqt5svg5-dev libzmq3-dev libdw-dev
cd
mkdir groot
cd groot
mkdir src
cd src
git clone https://github.com/osu-uwrt/Groot
cd Groot
git submodule update --init --recursive
cd ~/groot
colcon build

# make launch script for groot
rm -f launch_groot.sh #remove if exists

cat >> launch_groot.bash << EOF
#!/usr/bin/bash
. ~/groot/install/setup.bash
ros2 run groot Groot \$*

EOF

chmod +x launch_groot.bash
sudo ln launch_groot.bash /usr/bin/groot
