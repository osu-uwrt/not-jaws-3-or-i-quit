#ifndef POSE_EDKF_INTERFACE
#define POSE_EDKF_INTERFACE

//#include "riptide_gnc/pose_ekf.h"
#include "ros/ros.h"
#include "eigen3/Eigen/Dense"
#include "math.h"
#include <sstream>

using namespace Eigen;
using namespace std;

class PoseEDKFInterface
{
  private:
  ros::NodeHandle nh;
  Matrix3f mat;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    PoseEDKFInterface();
    void copy(const Ref<const MatrixXf>& m);
};

#endif