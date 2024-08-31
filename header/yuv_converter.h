//
// Created by leo on 24-8-31.
//

#ifndef YUV_CONVERTER_H
#define YUV_CONVERTER_H
#include <cstdint>
#include <string_view>

#include "media_type.h"

class YuvConverter {
private:
  bool formatPrinted;
  uint32_t pktCount;

  void decodeVideo(AVCodecContext* codecCtx, AVPacket* pkt, AVFrame* frame, std::ofstream& ofs);
  static void printPixelFormat(const AVFrame* frame);
public:
  // constructor
  YuvConverter();
  void videoToYuv(VideoType type, const std::string_view& videoPath, const std::string_view& yuvPath);
};
#endif //YUV_CONVERTER_H
