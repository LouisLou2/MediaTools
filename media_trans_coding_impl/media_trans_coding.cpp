//
// Created by leo on 24-9-2.
//
#include "../header/media_trans_coding.h"

#include <iostream>
#include <cmath>

void MediaTransCoder::test() {
  int m=8,n=8;
  // 创建矩阵
  Eigen::MatrixXi data(m,n);
  data.setRandom();
  data*=0;
  // 输出矩阵
  std::cout << "data = " << std::endl << data << std::endl;
}

Eigen::MatrixXd MediaTransCoder::plainDCT(const Eigen::MatrixXi& x) {
  // make sure it is a square matrix，DCT可以处理非方阵，但是为了简单起见，这里只考虑方阵
  assert(x.rows() == x.cols());
  const uint32_t N = x.rows();
  const double ak = std::sqrt(0.5);// ak=√2 when k=0, ak=1 when k!=0
  const double M = std::sqrt(2.0/N); // 系数最后再乘上吧
  // 变换矩阵
  Eigen::MatrixXd A(N,N);
  // k=0特殊，拿出来算
  A.col(0).setConstant(ak);
  // k!=0
  for(uint32_t n=0;n<N;++n) {
    for(uint32_t k=1;k<N;++k) {
      A(n,k) = std::cos((2*n+1)*k*std::numbers::pi/(2*N));
    }
  }
  A*=M;
  // print A
  std::cout << "A = " << std::endl << A << std::endl;
  // 计算
  return A.transpose() * x.cast<double>() * A;
}

Eigen::MatrixXd MediaTransCoder::plainDCT1_same(const Eigen::MatrixXi& x) {
  // make sure it is a square matrix，DCT可以处理非方阵，但是为了简单起见，这里只考虑方阵
  assert(x.rows() == x.cols());
  const uint32_t N = x.rows();
  const double ak = std::sqrt(0.5);// ak=√2 when k=0, ak=1 when k!=0
  const double M = std::sqrt(2.0/N); // 系数最后再乘上吧
  // 变换矩阵
  Eigen::MatrixXd A(N,N);
  // k=0特殊，拿出来算
  A.row(0).setConstant(ak);
  // k!=0
  for(uint32_t k=1;k<N;++k) {
    for(uint32_t n=0;n<N;++n) {
      A(k,n) = std::cos((2*n+1)*k*std::numbers::pi/(2*N));
    }
  }
  A*=M;
  // print A
  std::cout << "A = " << std::endl << A << std::endl;
  // 计算
  return A * x.cast<double>() * A.transpose();
}

Eigen::MatrixXi MediaTransCoder::imgDCT(const rawPicM& x) {
  // make sure it is a square matrix，这里只考虑图像处理中的习惯：只处理方阵
  assert(x.rows() == x.cols());
  // x:0-255 -> x:-128-127
  // eigen不支持矩阵与标量的减法
  const Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> xx = (x.cast<int8_t>() + PicM::Constant(x.rows(),x.cols(),-128)).cast<double>();
  // print xx
  std::cout << "xx = " << std::endl << xx << std::endl;
  const uint32_t N = x.rows();
  double ak=std::sqrt(2);// ak=√2 when k=0, ak=1 when k!=0
  const double M = std::sqrt(2.0/N); // 系数最后再乘上吧
  /// 变换矩阵
  Eigen::MatrixXd A(N,N);
  Eigen::MatrixXi y(N,N);
  // k=0特殊，拿出来算
  A.row(0).setConstant(ak);
  // k!=0
  for(uint32_t k=1;k<N;++k) {
    for(uint32_t n=0;n<N;++n) {
      A(k,n) = std::cos((2*n+1)*k*std::numbers::pi/(2*N));
    }
  }
  A*=M;
  // print A
  std::cout << "A = " << std::endl << A << std::endl;
  // 计算
  y=(A * xx * A.transpose()).cast<int>();
  return y;
}
