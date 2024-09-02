#include <iostream>
#include "header/yuv_converter.h"
#include "header/media_trans_coding.h"

void testVidToYuv() {
    std::string_view h264Path = "/home/leo/CLionProjects/ff_test/resource/bejeweled_6s.h264";
    std::string_view yuvPath = "/home/leo/CLionProjects/ff_test/resource/bejeweled_6s.yuv";
    YuvConverter cvt;
    cvt.videoToYuv(VideoType::H264,h264Path,yuvPath);
}

void testDCT() {
    Eigen::MatrixXi x(8,8);
    x <<  89, 101, 114, 125, 126, 115, 105, 96,

          97, 115, 131, 147, 149, 135, 123, 113,

          114, 134, 159, 178, 175, 164, 149, 137,

          121, 143, 177, 196, 201, 189, 165, 150,

          119, 141, 175, 201, 207, 186, 162, 144,

          107, 130, 165, 189, 192, 171, 144, 125,

          97, 119, 149, 171, 172, 145, 117, 96,

          88, 107, 136, 156, 155, 129, 97, 75;
    auto y=MediaTransCoder::plainDCT(x);
    std::cout << "y = " << std::endl << y << std::endl;
}

int main(){
    // std::cout << "Hello, World!" << std::endl;
    //test_ff_demux();
    //extract_aac("/home/leo/CLionProjects/ff_test/resource/the1.mp4","/home/leo/CLionProjects/ff_test/resource/the1.aac");
    //test_h264_mp4_annexb("/home/leo/CLionProjects/ff_test/resource/the1.mp4","/home/leo/CLionProjects/ff_test/resource/the1_annexb.h264");
    //audioToPcm(AudioType::AAC,"/home/leo/CLionProjects/ff_test/resource/the1.aac","/home/leo/CLionProjects/ff_test/resource/the1.pcm");
    // testVidToYuv();
    // MediaTransCoder::test();
    testDCT();
    // return 0;
}
