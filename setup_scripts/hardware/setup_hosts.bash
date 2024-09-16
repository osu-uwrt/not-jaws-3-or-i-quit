#!/bin/bash

# Update /etc/hosts File
# Add appropriate lines to the hosts if they do not exist
if ! grep -Fq "xavier" /etc/hosts; then
    echo -e "192.168.1.69\txavier\n$(cat /etc/hosts)" > /etc/hosts
fi

if ! grep -Fq "orin" /etc/hosts; then
    echo -e "192.168.1.23\torin\n$(cat /etc/hosts)" > /etc/hosts
fi

if ! grep -Fq "uwrt-dvl" /etc/hosts; then
    echo -e "192.168.1.212\tuwrt-dvl\n$(cat /etc/hosts)" > /etc/hosts
fi
