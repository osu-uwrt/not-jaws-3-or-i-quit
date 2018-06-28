/*********************************************************************************
 *  Copyright (c) 2017, The Underwater Robotics Team
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#ifndef IMU_LOGGER_H
#define IMU_LOGGER_H

#include "ros/ros.h"
#include "imu_3dm_gx4/MagFieldCF.h"
#include "std_msgs/Header.h"
#include "math.h"
#include "stdio.h"
#include "string"
#include "fstream"
#include <boost/lexical_cast.hpp>

class IMULogger
{
private:
  ros::NodeHandle nh;
  ros::Subscriber mag_sub;

  FILE *fid;
  const char *file_name_c;
  double tStart, tNow; //MUST use type-DOUBLE for storing ros::Time
  bool initialized;
public:
  IMULogger(char **argv);
  void magLogger(const imu_3dm_gx4::MagFieldCF::ConstPtr& mag);
  void loop();
  char* convert(const std::string& str);
};

#endif
