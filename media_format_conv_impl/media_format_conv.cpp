//
// Created by leo on 24-9-1.
//
#include "../header/media_format_conv.h"
#include "../header/ff_util.h"

#ifdef __cplusplus
extern "C" {
#include <libavfilter/avfilter.h>
#include <libavcodec/avcodec.h>
}
#else
#include <libavfilter/avfilter.h>
#include <libavcodec/avcodec.h>
#endif

#include <filesystem>
#include <memory>
#include <fstream>
#include <array>

namespace fs = std::filesystem;

void MediaFormatConv::h264_mp4_annexb(const std::string_view& srcPath, const std::string_view& dstPath) {
  int ret=0;
  std::array<char,AV_ERROR_MAX_STRING_SIZE+1> errBuf{};
  if(!fs::exists(srcPath)){
    fprintf(stderr,"source file not found: %s\n", srcPath.data());
    return;
  }
  AVFormatContext* fmtCtx = nullptr;
  ret = avformat_open_input(&fmtCtx, srcPath.data(), nullptr, nullptr);
  if(ret != 0){
    printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,ret,"avformat_open_input failed");
    return;
  }
  std::unique_ptr<AVFormatContext, AVFormatContextDeleter> fmtCtxPtr(fmtCtx);
  ret = avformat_find_stream_info(fmtCtxPtr.get(), nullptr);
  if(ret < 0){
    printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,ret,"avformat_find_stream_info failed");
    return;
  }

  printf("==== av_dump_format for file:%s =====\n", srcPath.data());
  av_dump_format(fmtCtxPtr.get(),0,srcPath.data(),0);
  printf("==== av_dump_format end =====\n");

  int vInd = av_find_best_stream(fmtCtxPtr.get(), AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if(vInd < 0){
    printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,vInd,"av_find_best_stream failed");
    return;
  }
  const AVStream* vStream = fmtCtxPtr->streams[vInd];
  if(vStream->codecpar->codec_id != AV_CODEC_ID_H264){
    fprintf(stderr,"video codec is not h264\n");
    return;
  }

  const AVBitStreamFilter* bsf = av_bsf_get_by_name("h264_mp4toannexb");
  AVBSFContext* bsfCtx = nullptr; // 不要使用raw ptr
  av_bsf_alloc(bsf, &bsfCtx);
  std::unique_ptr<AVBSFContext, AVBSFContextDeleter> bsfCtxPtr(bsfCtx);
  avcodec_parameters_copy(bsfCtxPtr->par_in, vStream->codecpar);
  av_bsf_init(bsfCtxPtr.get());

  std::unique_ptr<AVPacket,AVPacketDeleter> pkt(av_packet_alloc());

  std::ofstream ofs(dstPath.data(),std::ios::binary);
  if(!ofs.is_open()){
    fprintf(stderr,"open file failed: %s\n", dstPath.data());
    return;
  }

  bool keepReading = true;
  int outputCount=0;
  while(keepReading) {
    if((ret = av_read_frame(fmtCtxPtr.get(),pkt.get())) < 0) {
      keepReading = false;
      if(ret == AVERROR_EOF) {
        fprintf(stderr,"read frame eof\n");
      } else {
        printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,ret,"av_read_frame failed");
      }
    }
    if(pkt->stream_index != vInd) continue;
    // 开始将pkt送入bsf
    if(av_bsf_send_packet(bsfCtxPtr.get(),pkt.get())) {
      // 没有送成功， 继续送
      av_packet_unref(pkt.get());
      continue;
    }
    av_packet_unref(pkt.get());
    while(!av_bsf_receive_packet(bsfCtxPtr.get(),pkt.get())) {
      ++outputCount;
      ofs.write(reinterpret_cast<const char*>(pkt->data),pkt->size);
      av_packet_unref(pkt.get());
    }
  }
  ofs.close();
  printf("output pkt count: %d\n", outputCount);
}
