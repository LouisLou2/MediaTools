//
// Created by leo on 24-8-31.
//

#ifndef MEDIA_TRANS_H
#define MEDIA_TRANS_H
#include <string_view>

#include "media_type.h"


class MediaInfo {
private:
public:
  static void printVideoDemuxInfo(const std::string_view& path);
};
#endif //MEDIA_TRANS_H
