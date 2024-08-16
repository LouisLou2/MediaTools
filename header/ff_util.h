//
// Created by leo on 24-8-10.
//

#ifndef FF_UTIL_H
#define FF_UTIL_H

extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/bsf.h>
  #include <libavcodec/avcodec.h>
}

// deleter for AVFormatContext
struct AVFormatContextDeleter {
  void operator()(AVFormatContext* fmtCtx) const {
    avformat_close_input(&fmtCtx);
  }
};

// deleter for AVPacket
struct AVPacketDeleter {
  void operator()(AVPacket* pkt) const {
    av_packet_free(&pkt);
  }
};

// deleter for AVBSFContext
struct AVBSFContextDeleter {
  void operator()(AVBSFContext* bsfCtx) const {
    av_bsf_free(&bsfCtx);
  }
};

struct AVCodecContextDeleter {
  void operator()(AVCodecContext* codecCtx) const {
    avcodec_free_context(&codecCtx);
  }
};

struct AVFrameDeleter {
  void operator()(AVFrame* frame) const {
    av_frame_free(&frame);
  }
};

struct AVParserContextDeleter {
  void operator()(AVCodecParserContext* parserCtx) const {
    av_parser_close(parserCtx);
  }
};

struct AVCodecDeleter {
  void operator()(const AVCodec* codec) const {
    // maybe AVCodecContext will free this as well
  }
};
void printAVError(char* errBuf, int bufSize, int ret, const char* spec);
#endif //FF_UTIL_H
