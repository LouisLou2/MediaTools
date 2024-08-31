#include <iostream>
#include "header/yuv_converter.h"

int main(){
    std::cout << "Hello, World!" << std::endl;
    //test_ff_demux();
    //extract_aac("/home/leo/CLionProjects/ff_test/resource/the1.mp4","/home/leo/CLionProjects/ff_test/resource/the1.aac");
    //test_h264_mp4_annexb("/home/leo/CLionProjects/ff_test/resource/the1.mp4","/home/leo/CLionProjects/ff_test/resource/the1_annexb.h264");
    //audioToPcm(AudioType::AAC,"/home/leo/CLionProjects/ff_test/resource/the1.aac","/home/leo/CLionProjects/ff_test/resource/the1.pcm");
    YuvConverter cvt;
    cvt.videoToYuv(VideoType::H264,"/home/leo/CLionProjects/ff_test/resource/bejeweled_6s.h264","/home/leo/CLionProjects/ff_test/resource/bejeweled_6s.yuv");
    return 0;
}
