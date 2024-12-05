%
% UWRT Matlab installer toolbox checking script
%

fprintf("Checking for missing toolboxes...");
EXPECTED_TOOLBOXES = [
    "MATLAB Coder";
    "Robotics System Toolbox";
    "Simulink Coder";
    "HDL Coder";
    "Fixed-Point Designer";
    "Aerospace Toolbox";
    "Embedded Coder";
    "Control System Toolbox";
    "Simulink";
    "Aerospace Blockset";
    "ROS Toolbox";
];

installed_toolboxes_table = matlab.addons.installedAddons;
installed_toolbox_names = installed_toolboxes_table{:, 1};
missing_toolboxes = setdiff(EXPECTED_TOOLBOXES, installed_toolbox_names);

if ~isempty(missing_toolboxes)
    fprintf("MISSING TOOLBOXES DETECTED!\nToolboxes:\n");
    for i = 1 : length(missing_toolboxes)
        fprintf("%s\n", missing_toolboxes(i));
    end
    fprintf("\n");
    error("Found missing toolboxes, listed above. Please install them and re-run the matlab configure script (configure_matlab.bash).")
end

fprintf("No missing toolboxes found.\n");
