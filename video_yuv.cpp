//
// Created by leo on 24-8-16.
//
#include <array>
#include <cassert>
#include <memory>
#include <filesystem>
#include <fstream>

#include "header/ff_util.h"

#ifdef __cplusplus
  extern "C" {
  #include <libavutil/avutil.h>
  #include <libavutil/pixdesc.h>
}
#endif

#include "header/video_yuv.h"

#define VIDEO_INBUF_SIZE (1024*128) // 128KB
#define VIDEO_REFILL_THRESH (32*1024) // 32KB

namespace fs = std::filesystem;

static bool formatPrinted = false;
static uint32_t pktCount = 0;

void videoToYuv(VideoType type, const std::string_view& videoPath, const std::string_view& yuvPath) {
  if(!fs::exists(videoPath)){
    printAVError(nullptr,-1,0,"[ERROR] source file not found");
    return;
  }
  std::ifstream ifs(videoPath.data(),std::ios::binary);
  std::ofstream ofs(yuvPath.data(),std::ios::binary);
  if(!ifs.is_open()){
    printAVError(nullptr,-1,0,"[ERROR] source file open failed");
    return;
  }
  if(!ofs.is_open()){
    printAVError(nullptr,-1,0,"[ERROR] target file open failed");
    return;
  }

  size_t len;
  int ret;
  constexpr uint32_t errBufLen = AV_ERROR_MAX_STRING_SIZE+1;

  std::array<char,errBufLen> errBuf{};
  std::array<uint8_t,(VIDEO_INBUF_SIZE+AV_INPUT_BUFFER_PADDING_SIZE)> inbuf{};
  std::unique_ptr<AVFrame,AVFrameDeleter> frame(av_frame_alloc());
  std::unique_ptr<AVPacket,AVPacketDeleter>pkt(av_packet_alloc());

  // find decoder
  std::unique_ptr<const AVCodec, AVCodecDeleter> codec(avcodec_find_decoder(static_cast<AVCodecID>(type)));
  if (!codec) {
    av_strerror(AVERROR_DECODER_NOT_FOUND,errBuf.data(),errBufLen-1);
    printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,-1,"[ERROR] avcodec_find_decoder failed");
    return;
  }
  // probably won't go wrong
  std::unique_ptr<AVCodecParserContext,AVParserContextDeleter>parserCtx(av_parser_init(codec->id));
  // probably won't go wrong
  std::unique_ptr<AVCodecContext,AVCodecContextDeleter>codecCtx(avcodec_alloc_context3(codec.get()));

  ret = avcodec_open2(codecCtx.get(),codec.get(),nullptr);
  if(ret < 0){
    av_strerror(ret,errBuf.data(),errBufLen-1);
    printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,-1,"[ERROR] avcodec_open2 failed");
    return;
  }

  uint8_t* data= inbuf.data();
  size_t dataSize=0;

  // begin to read file
  while(true) {
    len = ifs.read(reinterpret_cast<char*>(data),VIDEO_INBUF_SIZE - dataSize).gcount();
    if (len <= 0) {
      break;// 是需要在while之后在最后解析一次，以耗尽剩余数据
    }
    dataSize += len;
    // parse data, get pkt
    while (dataSize>=VIDEO_REFILL_THRESH) {
      ret = av_parser_parse2(parserCtx.get(),codecCtx.get(),&pkt->data,&pkt->size,data,dataSize,AV_NOPTS_VALUE,AV_NOPTS_VALUE,0);
      if (ret < 0) {
        av_strerror(ret,errBuf.data(),errBufLen-1);
        printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,ret,"[ERROR] av_parser_parse2 failed");
        return;
      }
      ++pktCount;
      data += ret;
      dataSize -= ret;
      if (pkt->size) {
        decodeVideo(codecCtx.get(),pkt.get(),frame.get(),ofs);
      } else break;
    }
    // 说明buf中数据过少
    if (dataSize > 0)
      memmove(inbuf.data(),data,dataSize);
    data = inbuf.data();
  }
  // 最后一次解析
  while(dataSize>0){
    ret = av_parser_parse2(parserCtx.get(),codecCtx.get(),&pkt->data,&pkt->size,data,dataSize,AV_NOPTS_VALUE,AV_NOPTS_VALUE,0);
    if(ret==AVERROR_EOF) {
      printf("[INFO] av_parser_parse2: EOF\n");
      break;
    }
    if (ret < 0) {
      av_strerror(ret,errBuf.data(),errBufLen-1);
      printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,ret,"av_parser_parse2 failed");
      break;
    }
    ++pktCount;
    data += ret;
    dataSize -= ret;
    decodeVideo(codecCtx.get(),pkt.get(),frame.get(),ofs);
  }
  ifs.close();
  ofs.close();
  // struct will be freed automatically
  printf("[DONE] video to yuv done\n");
}

void decodeVideo(AVCodecContext* codecCtx, AVPacket* pkt, AVFrame* frame, std::ofstream& ofs) {
  int ret;
  bool toReceive = true;

  ret=avcodec_send_packet(codecCtx,pkt);

  if (ret == AVERROR(EAGAIN)) {
    printAVError(nullptr,-1,ret,"[WARN] get EAGAIN when trying to send packet");
  } else if(ret == AVERROR_EOF) {
    printAVError(nullptr,-1,ret,"[INFO] get EOF when trying to send packet");
  } else if(ret < 0) {
    // maybe there is other error decoding packet
    printAVError(nullptr,-1,ret,"avcodec_send_packet failed");
  }

  while(toReceive) {
    ret = avcodec_receive_frame(codecCtx, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      if (ret == AVERROR(EAGAIN))
        printAVError(nullptr,-1,ret,"[WARN] get EAGAIN when trying to receive frame");
      break;
    } else if (ret < 0) {
      printAVError(nullptr,-1,ret,"[ERROR] avcodec_receive_frame failed");
      throw std::runtime_error("avcodec_receive_frame failed");
    }

    if (!formatPrinted) {
      printPixelFormat(frame);
      assert(frame->format == AV_PIX_FMT_YUV420P);
      formatPrinted = true;
    }
    // H.264 一般强制为YUV420，这里就只写着一种格式的文件写入方法，其他的先不管了
    // Y, U, V分量的写入
    uint32_t planeSize = frame->width * frame->height;
    ofs.write(reinterpret_cast<char*>(frame->data[0]),planeSize);
    ofs.write(reinterpret_cast<char*>(frame->data[1]),planeSize/4);
    ofs.write(reinterpret_cast<char*>(frame->data[2]),planeSize/4);
    // for (int i = 0; i < frame->height; i++) {
    //   ofs.write(reinterpret_cast<char*>(frame->data[0] + i * frame->linesize[0]), frame->width);
    // }
    // for (int i = 0; i < frame->height / 2; i++) {
    //   ofs.write(reinterpret_cast<char*>(frame->data[1] + i * frame->linesize[1]), frame->width / 2);
    // }
    // for (int i = 0; i < frame->height / 2; i++) {
    //   ofs.write(reinterpret_cast<char*>(frame->data[2] + i * frame->linesize[2]), frame->width / 2);
    // }

  }
}

void printPixelFormat(const AVFrame* frame) {
  printf("width: %d, height: %d\n", frame->width, frame->height);
  printf("format: %d\n",frame->format);
  // format name
  printf("format name: %s\n",av_get_pix_fmt_name(static_cast<AVPixelFormat>(frame->format)));
}
