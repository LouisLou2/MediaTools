//
// Created by leo on 24-9-2.
//

#ifndef MEDIA_TRANS_CODING_H
#define MEDIA_TRANS_CODING_H
#include <Eigen/Core>

using rawPicM = Eigen::Matrix<uint8_t,Eigen::Dynamic,Eigen::Dynamic>;
using PicM = Eigen::Matrix<int8_t,Eigen::Dynamic,Eigen::Dynamic>;

class MediaTransCoder {
public:
  static void test();
  static Eigen::MatrixXd plainDCT(const Eigen::MatrixXi& x);
  static Eigen::MatrixXi imgDCT(const rawPicM& x);
};
#endif //MEDIA_TRANS_CODING_H
