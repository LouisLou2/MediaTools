//
// Created by leo on 24-9-1.
//

#ifndef MEDIA_FORMAT_CONV_H
#define MEDIA_FORMAT_CONV_H
#include <string_view>

class MediaFormatConv {
private:
public:
  void h264_mp4_annexb(const std::string_view& srcPath, const std::string_view& dstPath);
};
#endif //MEDIA_FORMAT_CONV_H
