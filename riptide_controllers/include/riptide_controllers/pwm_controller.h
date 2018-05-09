#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include "ros/ros.h"

#include "riptide_msgs/PwmStamped.h"
#include "riptide_msgs/ThrustStamped.h"
#include "riptide_msgs/SwitchState.h"

class PWMController
{
 private:
  ros::NodeHandle nh;
  ros::Subscriber cmd_sub;
  ros::Subscriber kill_sub;
  ros::Publisher pwm_pub;
  riptide_msgs::PwmStamped msg;
  void PublishZeroPWM();

  int thrust2pwm(double raw_force, int thruster);
  void load_calibration(float &param, std::string name);

  float thrust_config[8][4]; // thrust slopes
  bool dead;
  bool silent;
  ros::Time last_alive_time;
  ros::Duration alive_timeout;

 public:
  PWMController();
  void ThrustCB(const riptide_msgs::ThrustStamped::ConstPtr &thrust);
  void SwitchCB(const riptide_msgs::SwitchState::ConstPtr &state);
  void Loop();
};

#endif
