//
// Created by leo on 24-8-31.
//

#include <array>
#include <memory>
#include <filesystem>
#include <fstream>

#ifdef __cplusplus
extern "C" {
#include <libavutil/avutil.h>
}
#else
#include <libavutil/avutil.h>
#endif

#include "../header/ff_util.h"
#include "../header/media_type.h"
#include "../header/pcm_converter.h"

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
#define INVALID_DATA_ERR (-1094995529)
#define INVALID_DATA_ERR2 (-1163346256)

namespace fs = std::filesystem;

PcmConverter::PcmConverter(): formatPrinted(false), pktCount(0) {}

void PcmConverter::audioToPcm(AudioType type, const std::string_view& aacPath, const std::string_view& pcmPath) {
  // ensure file is exist and can be read
  if(!fs::exists(aacPath)){
    printAVError(nullptr,-1,0,"source file not found");
    return;
  }
  std::ifstream ifs(aacPath.data(),std::ios::binary);
  std::ofstream ofs(pcmPath.data(),std::ios::binary);
  if(!ifs.is_open()){
    printAVError(nullptr,-1,0,"source file open failed");
    return;
  }
  if(!ofs.is_open()){
    printAVError(nullptr,-1,0,"target file open failed");
    return;
  }

  size_t len=0;
  int ret=0;
  std::array<char,AV_ERROR_MAX_STRING_SIZE+1> errBuf{};
  std::array<uint8_t,(AUDIO_INBUF_SIZE+AV_INPUT_BUFFER_PADDING_SIZE)> inbuf{};
  std::unique_ptr<AVFrame,AVFrameDeleter>decodedFrame(av_frame_alloc());

  // alloc pkt
  std::unique_ptr<AVPacket,AVPacketDeleter>pkt(av_packet_alloc());
  // find decodercoe
  std::unique_ptr<const AVCodec, AVCodecDeleter> codec(avcodec_find_decoder(static_cast<AVCodecID>(type)));
  if(!codec){
    av_strerror(AVERROR_DECODER_NOT_FOUND,errBuf.data(),AV_ERROR_MAX_STRING_SIZE);
    printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,-1,"avcodec_find_decoder failed");
    return;
  }
  // find parser, probably won't go wrong
  std::unique_ptr<AVCodecParserContext,AVParserContextDeleter>parserCtx(av_parser_init(codec->id));
  // find codec context, probably won't go wrong
  std::unique_ptr<AVCodecContext,AVCodecContextDeleter>codecCtx(avcodec_alloc_context3(codec.get()));
  // relate decoder and codec context
  if((ret = avcodec_open2(codecCtx.get(),codec.get(),nullptr)) < 0 ){
    av_strerror(ret,errBuf.data(),AV_ERROR_MAX_STRING_SIZE);
    printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,ret,"avcodec_open2 failed");
    return;
  }

  uint8_t* data=inbuf.data();
  size_t dataSize=0;
  // begin to read file
  while(true) {
    len = ifs.read(reinterpret_cast<char*>(data),AUDIO_INBUF_SIZE - dataSize).gcount();
    if(len<=0){
      //TODO: handle eof
      break;
    }
    dataSize += len;
    // parse data, get pkt
    while(dataSize>=AUDIO_REFILL_THRESH){
      ret = av_parser_parse2(parserCtx.get(),codecCtx.get(),&pkt->data,&pkt->size,data,dataSize,AV_NOPTS_VALUE,AV_NOPTS_VALUE,0);
      if(ret < 0){
        av_strerror(ret,errBuf.data(),AV_ERROR_MAX_STRING_SIZE);
        printAVError(errBuf.data(),AV_ERROR_MAX_STRING_SIZE,ret,"av_parser_parse2 failed");
        return;
      }
      ++pktCount;
      data += ret;
      dataSize -= ret;
      if(pkt->size){
        decodeAudio(codecCtx.get(),pkt.get(),decodedFrame.get(),ofs);
      }else break;
    }
    // 将剩余数据移到缓冲区头部
    if (dataSize > 0)
      memmove(inbuf.data(),data,dataSize);
    data = inbuf.data();
  }
  pkt->data = nullptr;
  pkt->size = 0;
  decodeAudio(codecCtx.get(),pkt.get(),decodedFrame.get(),ofs);

  ofs.close();
  ifs.close();

  // av structs will be freed automatically
  printf("audio to pcm done\n");
}


void PcmConverter::decodeAudio(AVCodecContext* codecCtx, AVPacket* pkt, AVFrame* decodedFrame, std::ofstream& ofs) {
  int ret;
  int dataSize;
  ret = avcodec_send_packet(codecCtx,pkt);

  bool toReceive = true;
  if (ret == AVERROR(EAGAIN)) {
    printAVError(nullptr,-1,ret,"get EAGAIN when trying to send packet");
  } else if(ret == INVALID_DATA_ERR) {
    printAVError(nullptr,-1,ret,"get invalid data error when trying to send packet");
  } else if (ret == INVALID_DATA_ERR2) {
    printAVError(nullptr,-1,ret,"get invalid data error2 when trying to send packet");
  }
  else if (ret < 0 && ret != AVERROR_EOF) {
    toReceive = false;
    throw std::runtime_error("avcodec_send_packet failed with ret: "+std::to_string(ret));
  }

  while(toReceive) {
    ret=avcodec_receive_frame(codecCtx,decodedFrame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      if (ret == AVERROR(EAGAIN))
        printAVError(nullptr,-1,ret,"get EAGIN when trying to receive frame");
      break;
    }else if (ret < 0)
      throw std::runtime_error("avcodec_receive_frame failed");

    dataSize = av_get_bytes_per_sample(codecCtx->sample_fmt);
    if (dataSize < 0) {
      // This should not occur, checking just for paranoia
      throw std::runtime_error("Failed to calculate data size");
    }
    if(!formatPrinted){
      printSampleFormat(decodedFrame);
      formatPrinted = true;
    }
    // 交错模式
    // decodedFrame->nb_samples就是每个channel都有这么多sample
    for(uint32_t i=0;i<decodedFrame->nb_samples;++i) {
      for(uint32_t ch=0; ch<codecCtx->ch_layout.nb_channels;++ch) {
        ofs.write(reinterpret_cast<const char*>((decodedFrame->data)[ch]+dataSize*i),dataSize);
      }
    }
  }
}

/*
* sample rate: 44100
channel layout: 2
sample format enum: 8
sample format: fltp
 */
// ffplay -f f32le -ar 44100 -ac 2 -i the1.pcm
void PcmConverter::printSampleFormat(const AVFrame* frame) {
  printf("sample rate: %d\n",frame->sample_rate);
  printf("channel layout: %d\n",frame->ch_layout.nb_channels);
  printf("sample format enum: %d\n",frame->format);
  printf("sample format: %s\n",av_get_sample_fmt_name(static_cast<AVSampleFormat>(frame->format)));
}
