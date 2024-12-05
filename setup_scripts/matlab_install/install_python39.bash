#!/usr/bin/bash

#
# MATLAB install helper; installs python 3.9, needed for venv to add custom msg support
# usage: ./install_python39.bash <directory>
#

echo
echo "[INFO] Installing Python 3.9"
echo

cd $1

# this downloads source archive and build it
wget https://www.python.org/ftp/python/3.9.0/Python-3.9.0.tgz
tar -xf Python-3.9.0.tgz
cd Python-3.9.0
./configure
echo "Building Python3.9"
make --quiet -j4
echo "Installing Python3.9"
sudo make --quiet altinstall

