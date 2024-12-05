#!/usr/bin/bash

#
# UWRT MATLAB Configure script
# usage: ./configure_matlab.bash <matlab_install_dir> <python39_install_dir>
#

echo
echo "[INFO] Checking and configuring Matlab"
echo

# ensure that matlab executable exists. This means that matlab installed correctly.
matlabdir=$1
matlabexecutable=$matlabdir/bin/matlab
echo "Checking matlab"
while [ ! -f $matlabexecutable ] || [ "$matlabdir" = "" ]
do
    echo
    echo
    echo "Expected to find a MATLAB executable at $matlabexecutable, however there is none present."
    echo "Please enter the ABSOLUTE location of the matlab install directory: (currently: $matlabdir)"
    read matlabdir
    matlabexecutable=$matlabdir/bin/matlab
done

echo "Checking Python"
pythonexecutable=$2
while [ ! -f $pythonexecutable ] || [ "$pythonexecutable" = "" ]
do
    echo
    echo
    echo "Expected to find a Python3.9 executable at $pythonexecutable, however there is none present."
    echo "Please enter the ABSOLUTE location of the Python executable: (currently: $pythonexecutable)"
    read pythonexecutable
done

#change into scripts directory so we can run matlab scripts
cd ~/osu-uwrt/riptide_setup/setup_scripts/matlab_install

#link matlab executable to /usr/bin so that it can be easily run
#this is done as a shell script because the matlab executable tries 
#to reference files relative to itself.
echo "Linking MATLAB executable to /usr/bin"
rm -f $matlabdir/launch_matlab.bash #this prevents multiple installs from writing to the file a multiple times
cat >> $matlabdir/launch_matlab.bash << EOF
#!/usr/bin/bash
$matlabexecutable \$*

EOF
chmod +x $matlabdir/launch_matlab.bash

#check if the file exists
if [ -f "/usr/bin/matlab" ]
then
    echo "/usr/bin/matlab already exists. Replace it [y/n]?"
    read confirm
    if [ "$confirm" = "y" ] || [ "$confirm" = "Y" ]
    then
        echo "Removing"
        sudo rm -f /usr/bin/matlab
    else
        echo "NOT removing"
    fi
fi

sudo ln $matlabdir/launch_matlab.bash /usr/bin/matlab

#remove stdc++ shared lib that shipped with matlab. We want to use the system one, it works better
echo "Configuring MATLAB to use system stdc++ lib"
oldstdlib=`find $matlabdir/sys/os -maxdepth 2 -name libstdc++.so.6`
newstdlib=$oldstdlib.renamed
if [ "$oldstdlib" = "" ]
then
    echo
    echo "WARNING: The libstdc++.so.6 file (which should have shipped with MATLAB) was not found!"
    echo "Please ensure that this file does not exist ANYWHERE in your MATLAB directory ($matlabdir)!"
    echo "Your install may not work properly if it is not removed."
    echo "Please press enter to continue."
    echo
    read
else
    echo "Found stdc++ shared object file as : $oldstdlib"
    echo "Renaming $oldstdlib to $newstdlib"
    mv $oldstdlib $newstdlib
fi

#check that needed toolboxes are installed

echo "Checking that all required toolboxes are present"
matlab -batch check_toolboxes_installed
check_fail=$?
while [ $check_fail -eq 1 ]
do
    echo
    echo "Missing toolboxes detected. Please install the toolboxes listed above, then press enter to continue."
    read

    echo "Checking that all required toolboxes are present"
    matlab -batch check_toolboxes_installed
    check_fail=$?
done

#set up ros toolbox
echo "Configuring ROS toolbox environment"
matlab -batch "configure_rostoolbox(\"$pythonexecutable\")"

#configure custom msgs
echo "Configuring custom ROS message support"
# source ros first
. /opt/ros/$ROS_DISTRO/setup.bash
. ~/osu-uwrt/development/dependencies/install/setup.bash
. ~/osu-uwrt/development/software/install/setup.bash
ros2 run riptide_controllers2 model_manager.py refresh_custom_msg_support

