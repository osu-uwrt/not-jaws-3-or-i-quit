#!/bin/bash

if [[ $UID = 0 ]] ; then
  echo "Please dont use sudo with this script"
  exit
fi

if [[ $REINSTALL == 1 ]] ; then
  echo "Forcing reinstall of ROS2"
fi

if [[ -z "$ROS_DISTRO" || $REINSTALL == 1 ]] ; then
    if type lsb_release >/dev/null 2>&1; then
        VER=$(lsb_release -sr)
        if [ $VER == "22.04" ]; then
            export ROS_DISTRO="humble"
        else
            echo "Linux version not recognized"
            exit
        fi
        echo "Ros distribution $ROS_DISTRO selected"
    else
        echo "Linux distro not recognized"
        exit
    fi
fi

# Install ros
if [[ ! -d "/opt/ros/$ROS_DISTRO" || $REINSTALL == 1 ]] ; then
    ~/osu-uwrt/riptide_setup/setup_scripts/install_ros.bash
    printf "\n\n\n"
fi

# set up development installation
~/osu-uwrt/riptide_setup/setup_scripts/dev_install/install_dev.bash

# Prompt the user about installing the release setup
printf "\n\n\n"
echo "If you intend to work directly with deploying software to the robot"
echo "answer the following prompt with a y"
read -p "Install release setup? (default is no) " CHOICE
case "$CHOICE" in
    [yY]*) 
        echo "Installing release setup"
        ~/osu-uwrt/riptide_setup/setup_scripts/release_install/setup_release.bash
        ;;

    *) 
        echo "Skipping release setup"
        ;;
esac
printf "\n\n\n"

read -p "Install MATLAB? This will take approximately 15 additional minutes and use approximately 10 GB (default is no) " CHOICE
case "$CHOICE" in
    [yY]*) 
        echo "Running MATLAB setup"
        ~/osu-uwrt/riptide_setup/setup_scripts/matlab_install/install_matlab.bash
        ;;

    *) 
        echo "Skipping MATLAB setup"
        ;;
esac
printf "\n\n\n"

# install controller packages
echo "Installing most recent controller packages"
# source ros first
. /opt/ros/$ROS_DISTRO/setup.bash
. ~/osu-uwrt/development/dependencies/install/setup.bash
. ~/osu-uwrt/development/software/install/setup.bash
ros2 run riptide_controllers2 model_manager.py -y clean_workspace
ros2 run riptide_controllers2 model_manager.py -y download_packages --build

#setup groot
echo "Installing Groot"
~/osu-uwrt/riptide_setup/setup_scripts/install_groot.bash

# setup hosts and add hardware udev rules
echo "Setting up hardware and hosts files"
sudo ~/osu-uwrt/riptide_setup/setup_scripts/hardware/setup_hosts.bash
sudo ~/osu-uwrt/riptide_setup/setup_scripts/hardware/add_rule.bash

# Setup ~/.bashrc
echo "Setting bashrc for development"
~/osu-uwrt/riptide_setup/setup_scripts/setup_bashrc.bash

printf "\n\n\n"
echo "If no errors occurred during compilation, then everything was setup correctly"
echo "Please reboot your computer for final changes to take effect"
