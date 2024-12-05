#!/bin/bash

# make all the directories
mkdir -p ~/osu-uwrt/development
cd ~/osu-uwrt/development

echo "Starting ROSDEP"
sudo rosdep init
rosdep update

echo "Importing repositories"
vcs import < ~/osu-uwrt/riptide_setup/setup_scripts/dev_install/riptide.repos . --recursive
vcs pull

if [ -d ~/osu-uwrt/development/titan_firmware ]; then
    echo "Detected firmware repository, inserting COLCON_IGNORE"
    touch ~/osu-uwrt/development/titan_firmware/COLCON_IGNORE
else
    echo "Firmware repo not found after checkout. Make sure the setup script is up to date"
fi

# install child dependencies and build dependencies
echo "Building dependencies"
cd ~/osu-uwrt/development

# test for subfolders named zed and disable if nvidia-smi doesnt run
# this can also work for other packages and platforms
if ! [ -x "$(command -v nvidia-smi)" ]; then
    echo "Nvidia driver not found. Disabling zed packages"
    python3 ~/osu-uwrt/riptide_setup/setup_scripts/dev_install/package_disable.py zed
    python3 ~/osu-uwrt/riptide_setup/setup_scripts/dev_install/package_disable.py tensor_detector
fi

cd ~/osu-uwrt/development/dependencies

rosdep install --from-paths src --ignore-src --rosdistro $ROS_DISTRO -y -r
source /opt/ros/$ROS_DISTRO/setup.bash
colcon build
if [ $? -ne 0 ]; then
    echo "Development dependencies build failed! The script will continue but may have errors going further"
    sleep 10
fi


# add pico compilier tools
echo "Downloading Pico utils"
sudo apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

# build development software (this should pass)
echo "Building riptide_software"
cd ~/osu-uwrt/development/software
rosdep install --from-paths src --ignore-src --rosdistro $ROS_DISTRO -y -r
sudo apt install -y libglfw3-dev #for the sim
pip install transforms3d #for mapping
source /opt/ros/$ROS_DISTRO/setup.bash
source ~/osu-uwrt/development/dependencies/install/setup.bash
colcon build

if [ $? -ne 0 ]; then
    echo "Development software build failed! The script will continue but may have errors going further"
    sleep 10
fi
