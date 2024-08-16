//
// Created by leo on 24-8-9.
//
#include "header/ff_demux.h"
#include "header/ff_util.h"
extern "C" {
  #include <libavutil/avutil.h>
  #include <libavcodec/avcodec.h>
}

#include <filesystem>
#include <memory>
namespace fs = std::filesystem;

void test_ff_demux() {
  const char* vPath="/home/leo/CLionProjects/untitled1/resources/mp4/the1.mp4";
  if(!fs::exists(vPath)){
    fprintf(stderr,"pcm file not found: %s\n", vPath);
    return;
  }
  AVFormatContext* fmtCtx = nullptr;
  int ret = avformat_open_input(&fmtCtx, vPath, nullptr, nullptr);
  if(ret != 0){
    std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(AV_ERROR_MAX_STRING_SIZE+1);
    av_strerror(ret,errBuf.get(),AV_ERROR_MAX_STRING_SIZE);
    fprintf(stderr,"avformat_open_input failed: %s\n", errBuf.get());
    return;
  }
  std::unique_ptr<AVFormatContext, AVFormatContextDeleter> fmtCtxPtr(fmtCtx);
  ret = avformat_find_stream_info(fmtCtx, nullptr);
  if(ret < 0){
    std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(AV_ERROR_MAX_STRING_SIZE+1);
    av_strerror(ret,errBuf.get(),AV_ERROR_MAX_STRING_SIZE);
    fprintf(stderr,"avformat_find_stream_info failed: %s\n", errBuf.get());
    return;
  }
  printf("==== av_dump_format for file:%s =====\n", vPath);
  av_dump_format(fmtCtxPtr.get(),0,vPath,0);
  printf("==== av_dump_format end =====\n");
  printf("media url: %s\n", fmtCtxPtr->url);
  printf("stream number: %d\n", fmtCtxPtr->nb_streams);
  printf("media average ratio:%lld \n", fmtCtxPtr->bit_rate);

  // timing
  uint64_t totalSec = fmtCtxPtr->duration/AV_TIME_BASE;
  uint32_t hour = totalSec/3600;
  uint32_t min = (totalSec%3600)/60;
  uint32_t sec = totalSec%60;
  printf("media duration:%02d:%02d:%02d\n", hour, min, sec);

  for(uint32_t i=0;i<fmtCtxPtr->nb_streams;++i) {
    printf("========= stream index: %d =========\n", i);
    AVStream* stream = fmtCtxPtr->streams[i];
    switch (stream->codecpar->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
      printf("audio info:\n");
      printf("index: %d\n",stream->index);
      printf("codec_id: %d\n",stream->codecpar->codec_id);
      printf("sample_rate: %d\n",stream->codecpar->sample_rate);
      printf("channels: %d\n",stream->codecpar->ch_layout.nb_channels);
      printf("format: %d\n",stream->codecpar->format);
      if(stream->duration!=AV_NOPTS_VALUE){
        printf("duration: %lld\n",static_cast<uint64_t>(stream->duration * av_q2d(stream->time_base)));
      }else printf("duration: unknown\n");
      break;
    case AVMEDIA_TYPE_VIDEO:
      printf("video info:\n");
      printf("index: %d\n",stream->index);
      printf("fps: %lf\n",av_q2d(stream->avg_frame_rate));
      printf("codec_id: %d\n",stream->codecpar->codec_id);
      printf("width: %d\n",stream->codecpar->width);
      printf("height: %d\n",stream->codecpar->height);
      printf("format: %d\n",stream->codecpar->format);
      if(stream->duration!=AV_NOPTS_VALUE){
        printf("duration: %lld\n",static_cast<uint64_t>(stream->duration * av_q2d(stream->time_base)));
      }else printf("duration: unknown\n");
      break;
    default:
      printf("stream type that untested\n");
      break;
    }
  }
  printf("========= stream end =========\n");

  // begin to analyse packets
  uint32_t pacCount=0;
  uint32_t maxCount=10;

  AVPacket* pkt = av_packet_alloc();

  while(true) {
    ret = av_read_frame(fmtCtxPtr.get(), pkt);
    if(ret != 0) {
      printf("av_read_frame end of file\n");
      break;
    }
    if(pacCount++ == maxCount-1) {
      printf("av_read_frame break\n");
      break;
    }
    printf("========== packet index: %d\n", pacCount);
    switch (pkt->stream_index) {
    case AVMEDIA_TYPE_AUDIO:
      printf("audio pts:%lld\n",pkt->pts);
      printf("audio dts:%lld\n",pkt->dts);
      printf("audio duration:%lf\n",pkt->duration * av_q2d(fmtCtxPtr->streams[pkt->stream_index]->time_base));
      printf("audio size:%d\n",pkt->size);
      break;
    case AVMEDIA_TYPE_VIDEO:
      printf("video pts:%lld\n",pkt->pts);
      printf("video dts:%lld\n",pkt->dts);
      printf("video duration:%lf\n",pkt->duration * av_q2d(fmtCtxPtr->streams[pkt->stream_index]->time_base));
      printf("video size:%d\n",pkt->size);
      printf("video pos:%lld\n",pkt->pos);
      break;
    }
    av_packet_unref(pkt);
  }
  av_packet_free(&pkt);
}
