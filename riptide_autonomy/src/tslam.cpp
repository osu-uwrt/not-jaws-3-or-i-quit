#include "riptide_autonomy/tslam.h"

TSlam::TSlam(BeAutonomous* master) {
  this->master = master;
  duration = 0;
  active_subs.clear();
}

void TSlam::Start() {
  // Calculate heading to point towards next task
  delta_x = master->start_x - master->current_x;
  delta_y = master->start_y - master->current_y;
  angle = atan2(delta_y, delta_x);
  heading = angle - 90;
  if(heading <= -180)
    heading += 360;

  // Publish attitude command
  riptide_msgs::AttitudeCommand attitude_cmd;
  attitude_cmd.roll_active = true;
  attitude_cmd.pitch_active = true;
  attitude_cmd.yaw_active = true;
  attitude_cmd.euler_rpy.x = 0;
  attitude_cmd.euler_rpy.y = 0;
  attitude_cmd.euler_rpy.z = heading;
  master->attitude_pub.publish(attitude_cmd);

  // Publish depth command
  riptide_msgs::DepthCommand depth_cmd;
  depth_cmd.active = true;
  depth_cmd.depth = master->search_depth;
  master->depth_pub.publish(depth_cmd);

  // Calculate distance and ETA
  distance = sqrt(delta_x*delta_x + delta_y*delta_y);
  master->CalcETA(master->search_accel, distance);

  depth_status_sub = master->nh.subscribe<riptide_msgs::ControlStatus>("/status/controls/depth", 1, &TSlam::DepthStatusCB, this);
  active_subs.push_back(depth_status_sub);
}

void TSlam::DepthStatusCB(const riptide_msgs::ControlStatus::ConstPtr& status_msg) {
  if(abs(status_msg->error) < master->depth_thresh) {
    if(duration == 0)
      acceptable_begin = ros::Time::now();
    else
      duration += (ros::Time::now().toSec() - acceptable_begin.toSec());

    if(duration >= master->error_duration_thresh) {
      depth_status_sub.shutdown();
      active_subs.erase(active_subs.end());
      duration = 0;
      attitude_status_sub = master->nh.subscribe<riptide_msgs::ControlStatusAngular>("/status/controls/angular", 1, &TSlam::AttitudeStatusCB, this);
      active_subs.push_back(attitude_status_sub);
    }
  }
  else duration = 0;
}

void TSlam::AttitudeStatusCB(const riptide_msgs::ControlStatusAngular::ConstPtr& status_msg) {
	// Depth is good, now verify heading error
	if(abs(status_msg->yaw.error) < master->yaw_thresh)
	{
    if(duration == 0)
      acceptable_begin = ros::Time::now();
    else
      duration += (ros::Time::now().toSec() - acceptable_begin.toSec());

    if(duration >= master->error_duration_thresh) {
      attitude_status_sub.shutdown();
      active_subs.clear();

  		// Drive forward
  		geometry_msgs::Vector3 msg;
  		msg.x = master->search_accel;
  		msg.y = 0;
  		msg.z = 0;
  		master->linear_accel_pub.publish(msg);
      master->eta_start = ros::Time::now();
      master->timer = master->nh.createTimer(ros::Duration(master->eta), &BeAutonomous::EndTSlamTimer, master);
    }
	}
  else duration = 0;
}

// Shutdown all active subscribers
void TSlam::Abort() {
  if(active_subs.size() > 0) {
    for(int i=0; i<active_subs.size(); i++) {
      active_subs.at(i).shutdown();
    }
    active_subs.clear();
  }
  geometry_msgs::Vector3 msg;
  msg.x = 0;
  msg.y = 0;
  msg.z = 0;
  master->linear_accel_pub.publish(msg);
}
