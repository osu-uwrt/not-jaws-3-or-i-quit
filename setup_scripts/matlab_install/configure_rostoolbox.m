%
% UWRT Matlab Installer ROS toolbox configuration script
% Need to set python executable to 3.9 and then manually invoke ROS venv
% setup
%
function configure_rostoolbox(python_location)
    format compact
    
    %save old python executable
    old_pe = pyenv;
    old_python = old_pe.Executable;

    fprintf("Temporarily overriding python executable setting. Currently set to %s\n", old_python);
    
    %activate python 3.9
    pyenv("Version", python_location);
    
    %check that it actually worked
    new_pe = pyenv;
    if new_pe.Executable ~= python_location
        error("Python was not correctly set to 3.9! Cannot continue.\n");
    end

    %init ROS2 venv
    fprintf("Configuring MATLAB ROS2 venv\n");
    ros.ros2.internal.createOrGetLocalPython(true); %the "true" forces reinit
    
    %set back old python executable
    pyenv("Version", old_python);
    new_pe = pyenv;
    if new_pe.Executable == old_python
        fprintf("Python executable successfully set back to %s\n", new_pe.Executable);
    else
        warning("Old python executable NOT successfully restored. It is now %s\n", new_pe.Executable);
    end
end