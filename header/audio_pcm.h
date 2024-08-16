//
// Created by leo on 24-8-16.
//

#ifndef AAC_PCM_H
#define AAC_PCM_H
#include <string_view>

#include "media_type.h"
void audioToPcm(AudioType type, const std::string_view& aacPath, const std::string_view& pcmPath);
void decodeAudio(AVCodecContext* codecCtx, AVPacket* pkt, AVFrame* decodedFrame, std::ofstream& ofs);
void printSampleFormat(const AVFrame* frame);
#endif //AAC_PCM_H
