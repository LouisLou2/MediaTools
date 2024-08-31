//
// Created by leo on 24-8-31.
//

#ifndef PCM_CONVERTER_H
#define PCM_CONVERTER_H
#include <cstdint>
#include <string_view>


#include "media_type.h"


class PcmConverter {
private:
  bool formatPrinted;
  uint64_t pktCount;

  void decodeAudio(AVCodecContext* codecCtx, AVPacket* pkt, AVFrame* decodedFrame, std::ofstream& ofs);
  static void printSampleFormat(const AVFrame* frame);
public:
  PcmConverter();
  void audioToPcm(AudioType type, const std::string_view& aacPath, const std::string_view& pcmPath);
};



#endif //PCM_CONVERTER_H
