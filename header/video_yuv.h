//
// Created by leo on 24-8-16.
//

#ifndef VIDEO_YUV_H
#define VIDEO_YUV_H
#include <string_view>

#include "media_type.h"

void videoToYuv(VideoType type, const std::string_view& videoPath, const std::string_view& yuvPath);
void decodeVideo(AVCodecContext* codecCtx, AVPacket* pkt, AVFrame* decodedFrame, std::ofstream& ofs);
void printPixelFormat(const AVFrame* frame);

#endif //VIDEO_YUV_H
