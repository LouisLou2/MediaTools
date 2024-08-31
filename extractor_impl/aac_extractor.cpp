//
// Created by leo on 24-9-1.
//
#include "../header/aac_extractor.h"

#include "../header/ff_util.h"

#ifdef __cplusplus
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
}
#else
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#endif

#include <filesystem>
#include <memory>
#include <fstream>
#include <array>

namespace fs = std::filesystem;

const std::array<uint32_t,SAMPLING_ARRAY_LEN> AACExtractor::samplingRateTable  = {
  96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350
};

int AACExtractor::adtsHeaderSet(char*const pAdtsHeader, const int len, const int profile, const int sampleRate, const int channels) {
  uint16_t samplingRateInd;
  uint32_t adtslen = len+7;
  uint16_t i;
  for(i=0; i<SAMPLING_ARRAY_LEN; ++i){
    if(sampleRate==samplingRateTable[i]){
      samplingRateInd = i;
      break;
    }
  }
  if(i==SAMPLING_ARRAY_LEN){
    fprintf(stderr,"invalid sample rate: %d\n", sampleRate);
    return -1;
  }
  pAdtsHeader[0] = 0xff; // syncword: 0xff            高8bits

  pAdtsHeader[1] = 0xf0; // syncword: 0xf0            低4bits
  pAdtsHeader[1] |= (0<<3); // ID: 0  MPEG-4 :1       1bit
  pAdtsHeader[1] |= (0<<2); // layer: '00'            2bits
  pAdtsHeader[1] |= 1; // protection_absent: 1        1bit

  pAdtsHeader[2] = (profile<<6); // profile:     2bits
  pAdtsHeader[2] |= (samplingRateInd & 0x0f)<<2; // sampling frequency index:  4bits
  pAdtsHeader[2] |= (0<<1); // private_bit: 0          1bit
  pAdtsHeader[2] |= (channels & 0x04)>>2; // channel configuration:  高1bits(共3bits)

  pAdtsHeader[3] = (channels & 0x03)<<6; // channel configuration:  2bits(共3bits)
  pAdtsHeader[3] |= (0<<5); // original/copy: 0        1bit
  pAdtsHeader[3] |= (0<<4); // home: 0                 1bit
  pAdtsHeader[3] |= (0<<3); // copyright: 0             1bit
  pAdtsHeader[3] |= (0<<2); // copyright: 0             1bit
  pAdtsHeader[3] |= (adtslen & 0x1800) >> 11; // frame length: 先2bits(共13bits)

  pAdtsHeader[4] = (adtslen & 0x07f8) >> 3; // frame length: 中8bits(共13bits)

  pAdtsHeader[5] = (adtslen & 0x7) << 5; // frame length: 后3bits(共13bits)
  pAdtsHeader[5] |= 0x1f;

  pAdtsHeader[6] = 0xfc;
  // number_of_raw_data_blocks_in_frame: 0b0000
  return 0;
}

void AACExtractor::extractAAC(const std::string_view& vPath, const std::string_view& aacPath) {
  int ret=0;
  char errors[AV_ERROR_MAX_STRING_SIZE+1];
  if(!fs::exists(vPath)){
    fprintf(stderr,"source file not found: %s\n", vPath);
    return;
  }
  AVFormatContext* fmtCtx = nullptr;
  ret = avformat_open_input(&fmtCtx, vPath.data(), nullptr, nullptr);
  if(ret != 0){
    av_strerror(ret,errors,AV_ERROR_MAX_STRING_SIZE);
    printAVError(errors,AV_ERROR_MAX_STRING_SIZE,ret,"avformat_open_input failed");
    return;
  }
  std::unique_ptr<AVFormatContext, AVFormatContextDeleter> fmtCtxPtr(fmtCtx);
  ret = avformat_find_stream_info(fmtCtxPtr.get(), nullptr);
  if(ret < 0){
    av_strerror(ret,errors,AV_ERROR_MAX_STRING_SIZE);
    printAVError(errors,AV_ERROR_MAX_STRING_SIZE,ret,"avformat_find_stream_info failed");
    return;
  }

  printf("==== av_dump_format for file:%s =====\n", vPath);
  av_dump_format(fmtCtxPtr.get(),0,vPath.data(),0);
  printf("==== av_dump_format end =====\n");

  std::unique_ptr<AVPacket,AVPacketDeleter> pkt(av_packet_alloc());
  int audioInd=av_find_best_stream(fmtCtxPtr.get(),AVMEDIA_TYPE_AUDIO,-1,-1,nullptr,0);
  if(audioInd<0){
    printAVError(errors,AV_ERROR_MAX_STRING_SIZE,audioInd,"av_find_best_stream failed");
    return;
  }
  const AVStream* aStream=fmtCtxPtr->streams[audioInd];
  // print aac level
  printf("audio profile: %d\nFF_PROFILE_AAC_LOW: %d\n",aStream->codecpar->profile,FF_PROFILE_AAC_LOW);

  if(aStream->codecpar->codec_id!=AV_CODEC_ID_AAC){
    fprintf(stderr,"audio codec is not aac\n");
    return;
  }

  char adtsHeader[7];
  std::ofstream ofs(aacPath.data(),std::ios::binary);
  if(!ofs.is_open()){
    fprintf(stderr,"open file failed: %s\n", aacPath);
    return;
  }
  while(av_read_frame(fmtCtxPtr.get(),pkt.get())>=0){
    if(pkt->stream_index!=audioInd){
      continue;
    }
    auto theStream = fmtCtxPtr->streams[pkt->stream_index];
    adtsHeaderSet(adtsHeader,pkt->size,theStream->codecpar->profile,theStream->codecpar->sample_rate,theStream->codecpar->ch_layout.nb_channels);
    ofs.write(adtsHeader,7);// 写header
    ofs.write(reinterpret_cast<const char*>(pkt->data),pkt->size);// 写数据
    av_packet_unref(pkt.get());
  }
  ofs.close();
  printf("extract aac done\n");
}
