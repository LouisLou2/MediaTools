//
// Created by leo on 24-8-16.
//

#ifndef MEDIA_TYPE_H
#define MEDIA_TYPE_H
extern "C" {
#include <libavcodec/avcodec.h>
}

enum class AudioType {
  AAC=AV_CODEC_ID_AAC,
  MP3=AV_CODEC_ID_MP3,
  FLAC=AV_CODEC_ID_FLAC,
};

enum class VideoType {
  H264=AV_CODEC_ID_H264,
  HEVC=AV_CODEC_ID_MPEG2VIDEO,
};
#endif //MEDIA_TYPE_H
