#include <iostream>
#include "header/ff_demux.h"
#include "header/extract_aac.h"
#include "header/h264_annexb.h"
#include "header/audio_pcm.h"
#include "header/video_yuv.h"
int main(){
    std::cout << "Hello, World!" << std::endl;
    //test_ff_demux();
    //extract_aac("/home/leo/CLionProjects/ff_test/resource/the1.mp4","/home/leo/CLionProjects/ff_test/resource/the1.aac");
    //test_h264_mp4_annexb("/home/leo/CLionProjects/ff_test/resource/the1.mp4","/home/leo/CLionProjects/ff_test/resource/the1_annexb.h264");
    //audioToPcm(AudioType::AAC,"/home/leo/CLionProjects/ff_test/resource/the1.aac","/home/leo/CLionProjects/ff_test/resource/the1.pcm");
    videoToYuv(VideoType::H264,"/home/leo/CLionProjects/ff_test/resource/bejeweled_6s.h264","/home/leo/CLionProjects/ff_test/resource/bejeweled_6s.yuv");
    return 0;
}