#!/bin/bash


cd ~/osu-uwrt/
mkdir -p dependencies
cd dependencies

git -C MYNT pull || git clone https://github.com/slightech/MYNT-EYE-D-SDK.git MYNT
cd MYNT
make init
make ros
cd ..

mkdir src
cd src

if [ $ROS_DISTRO == "noetic" ]; then
    vcs import < ~/osu-uwrt/riptide_setup/scripts/setup_scripts/dependencies_noetic.repos . --recursive
else
    vcs import < ~/osu-uwrt/riptide_setup/scripts/setup_scripts/dependencies.repos . --recursive
fi

cd ..

rosdep install --from-paths src --ignore-src --rosdistro $ROS_DISTRO -y -r

catkin config --install --extend ~/osu-uwrt/dependencies/MYNT/wrappers/ros/devel --cmake-args -DCMAKE_BUILD_TYPE=Release
catkin build
