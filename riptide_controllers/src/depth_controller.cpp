#include "riptide_controllers/depth_controller.h"

#undef debug
#undef report
#undef progress

int main(int argc, char **argv) {
  ros::init(argc, argv, "depth_controller");
  DepthController dc;
  ros::spin();
}

void DepthController::UpdateError() {
  sample_duration = ros::Time::now() - sample_start;
  dt = sample_duration.toSec();

  depth_error = current_depth - cmd_depth;
  d_error = (depth_error - last_error) / dt;
  last_error = depth_error;

  accel.data = depth_controller_pid.computeCommand(depth_error, d_error, sample_duration);

  cmd_pub.publish(accel);
  sample_start = ros::Time::now();
}


DepthController::DepthController() {
    ros::NodeHandle dcpid("depth_controller");
    cmd_sub = nh.subscribe<riptide_msgs::Depth>("command/depth", 1000, &DepthController::CommandCB, this);
    depth_sub = nh.subscribe<riptide_msgs::Depth>("state/depth", 1000, &DepthController::DepthCB, this);
    kill_sub = nh.subscribe<riptide_msgs::SwitchState>("state/switches", 10, &DepthController::SwitchCB, this);
    depth_controller_pid.init(dcpid, false);

    cmd_pub = nh.advertise<std_msgs::Float64>("command/accel/linear/z", 1);
    sample_start = ros::Time::now();
}

// Subscribe to command/depth
void DepthController::DepthCB(const riptide_msgs::Depth::ConstPtr &depth) {
  current_depth = depth->depth;

  if (!pid_initialized)
    cmd_depth = current_depth;

  DepthController::UpdateError();
}

// Subscribe to state/depth
void DepthController::CommandCB(const riptide_msgs::Depth::ConstPtr &cmd) {
  cmd_depth = cmd->depth;

  if (!pid_initialized)
    pid_initialized = true;

  DepthController::UpdateError();
}

//Subscribe to state/switches
void DepthController::SwitchCB(const riptide_msgs::SwitchState::ConstPtr &state) {
  if (!state->kill) {
    DepthController::ResetController();
  }
}

void DepthController::ResetController() {
  depth_error = 0;
  current_depth = 0;
  cmd_depth = 0;
  d_error = 0;
  last_error = 0;
  dt = 0;

  sample_start = ros::Time::now();
  sample_duration = ros::Duration(0);
  dt = 0;

  pid_initialized = false;
}
