//
// Created by leo on 24-8-11.
//
#include "header/ff_util.h"
extern "C"{
  #include <libavutil/avutil.h>
}

void printAVError(char* errBuf, int bufSize, int ret, const char* spec){
  if(!errBuf) {
    fprintf(stderr,"%s: %d\n", spec, ret);
    return;
  }
  av_strerror(ret,errBuf,AV_ERROR_MAX_STRING_SIZE);
  fprintf(stderr,"%s: %s\n", spec, errBuf);
}