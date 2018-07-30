// Gold chip, what it does:
// 1. Identify gold chip
// 2. Align to gold chip
// 3. Burn towards gold chip
// 4. Back off gold chip
#include "riptide_autonomy/task_gold_chip.h"

#define AST_CENTER 0
#define AST_BBOX 1

#define BURN_BABY_BURN 0
#define BACK_OFF_MAN 1

GoldChip::GoldChip(BeAutonomous* master) {
  this->master = master;
  GoldChip::Initialize();
}

void GoldChip::Initialize() {
  active_subs.clear();

  mission_state = -1;
  delete chip_detector;
}

void GoldChip::Start() {
  burn_time = master->tasks["tasks"][master->task_id]["burn_time"].as<double>();
  back_off_time = master->tasks["tasks"][master->task_id]["back_off_time"].as<double>();
  burn_accel_msg.data = master->search_accel;
  align_cmd.surge_active = false;
  align_cmd.sway_active = false;
  align_cmd.heave_active = false;
  align_cmd.object_name = master->object_names.at(0);
  align_cmd.alignment_plane = master->alignment_plane;
  align_cmd.bbox_dim = (int)(master->frame_height*0.7);
  align_cmd.bbox_control = rc::CONTROL_BBOX_HEIGHT;
  align_cmd.target_pos.x = 0;
  align_cmd.target_pos.y = 0;
  align_cmd.target_pos.z = 0;
  master->alignment_pub.publish(align_cmd);
  ROS_INFO("GoldChip: Alignment controller disabled. Awaiting detections...");

  task_bbox_sub = master->nh.subscribe<darknet_ros_msgs::BoundingBoxes>("/task/bboxes", 1, &GoldChip::Identify, this);
  active_subs.push_back(task_bbox_sub);
  chip_detector = new DetectionValidator(master->detections_req, master->detection_duration_thresh);
  x_validator = new ErrorValidator(master->align_thresh, master->error_duration_thresh);
  y_validator = new ErrorValidator(master->align_thresh, master->error_duration_thresh);
  bbox_validator = new ErrorValidator(master->bbox_thresh, master->error_duration_thresh);
}

void GoldChip::idToAlignment() {
  task_bbox_sub.shutdown();
  active_subs.erase(active_subs.end());

  align_cmd.surge_active = false;
  align_cmd.sway_active = true;
  align_cmd.heave_active = true;
  alignment_state = AST_CENTER;

  // Take control
  master->alignment_pub.publish(align_cmd);
  alignment_status_sub = master->nh.subscribe<riptide_msgs::ControlStatusLinear>("/status/controls/linear", 1, &GoldChip::AlignmentStatusCB, this);
  active_subs.push_back(alignment_status_sub);
}

void GoldChip::Identify(const darknet_ros_msgs::BoundingBoxes::ConstPtr& bbox_msg) {
  int attempts = chip_detector->GetAttempts();
  if (chip_detector->GetDetections() == 0) {
    ROS_INFO("GoldChip: Beginning target identificaion. Previous attempts: %d", attempts);
    if (attempts == 0)
      master->tslam->Abort(true);
  }

  if (chip_detector->Validate()) {
    ROS_INFO("GoldChip: Identification complete. Identified target after after %d attempts. Aligning to target.", chip_detector->GetAttempts());
    chip_detector->Reset();
    GoldChip::idToAlignment();
  }
}

void GoldChip::AlignmentStatusCB(const riptide_msgs::ControlStatusLinear::ConstPtr& status_msg) {
  if (alignment_state == AST_CENTER) {
    if (x_validator->Validate(status_msg->x.error) && y_validator->Validate(status_msg->y.error)) {
      x_validator->Reset();
      y_validator->Reset();

      // Unsure if this is how we lock in depth, copied from roulette
      align_cmd.heave_active = false;
      align_cmd.surge_active = true;
      master->alignment_pub.publish(align_cmd);
      alignment_state = AST_BBOX;
      ROS_INFO("GoldChip: Aligned to target. Depth locked in. Approaching target.");
    }
  } else if (alignment_state == AST_BBOX) {
    if (bbox_validator->Validate(status_msg->z.error)) {
      ROS_INFO("GoldChip: Target within reach. Beginning push maneuver.");
      GoldChip::StrikeGold();
    }
  }
}

void GoldChip::StrikeGold() {
  mission_state = BURN_BABY_BURN;
  align_cmd.surge_active = false;
  master->alignment_pub.publish(align_cmd);
  master->x_accel_pub.publish(burn_accel_msg);
  ROS_INFO("GoldChip: Push burn start.");
  timer = master->nh.createTimer(ros::Duration(burn_time), &GoldChip::BurnCompleteCB, this, true);
}

void GoldChip::BurnCompleteCB(const ros::TimerEvent &event) {
  if (mission_state == BURN_BABY_BURN) {
    mission_state = BACK_OFF_MAN;
    timer = master->nh.createTimer(ros::Duration(back_off_time), &GoldChip::BurnCompleteCB, this, true);
    ROS_INFO("GoldChip: Push burn complete. Backing off.");
  } else if (mission_state == BACK_OFF_MAN) {
    master->x_accel_pub.publish(burn_accel_msg);
    ROS_INFO("GoldChip: Backed off. Task complete. Ending...");
    GoldChip::Abort();
  }
}

// Shutdown all active subscribers
void GoldChip::Abort() {
  GoldChip::Initialize();

  if(active_subs.size() > 0) {
    for(int i = 0; i < active_subs.size(); i++) {
      active_subs.at(i).shutdown();
    }
    active_subs.clear();
  }

  align_cmd.surge_active = false;
  align_cmd.sway_active = false;
  align_cmd.heave_active = false;
  master->alignment_pub.publish(align_cmd);
  ROS_INFO("GoldChip: Aborting");
}