#!/bin/bash

s1="source /opt/ros/$ROS_DISTRO/setup.bash"
s2="source ~/osu-uwrt/development/dependencies/install/setup.bash"
s3="source ~/osu-uwrt/development/software/install/setup.bash"

if [ -d ~/osu-uwrt/release ]; then

    cat >> ~/.bashrc << EOF
if [ ! -f /tmp/source_this_boot ]; then
    read -p "Setup bashrc for Release (R) or Development (D)? (default Development) " -r
    echo ""

    touch /tmp/source_this_boot
    if [[ \$REPLY =~ ^[Rr]$ ]]; then
        touch /tmp/source_release
    else
        rm -f /tmp/source_release
    fi
fi



if [ -f /tmp/source_release ]; then
    echo "Sourcing Release"
    source ~/osu-uwrt/release/install/setup.bash
else
    echo "Sourcing Development"
    source ~/osu-uwrt/development/dependencies/install/setup.bash
    source ~/osu-uwrt/development/software/install/setup.bash
fi

EOF
else
    # Add appropriate lines to the bashrc if they do not exist
    if ! grep -q "$s1" ~/.bashrc || ! grep -q "$s2" ~/.bashrc || ! grep -q "$s3" ~/.bashrc; then
        # sed -i "/setup.bash/d" ~/.bashrc
        echo "$s1" >> ~/.bashrc
        echo "$s2" >> ~/.bashrc
        echo "$s3" >> ~/.bashrc
    fi
fi
