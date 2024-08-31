//
// Created by leo on 24-9-1.
//

#ifndef AAC_EXTRACTOR_H
#define AAC_EXTRACTOR_H
#include <array>
#include <cstdint>
#include <string_view>

#define SAMPLING_ARRAY_LEN 13
#define PROTECTION_ABSENT 1

class AACExtractor {
private:
  static const std::array<uint32_t,SAMPLING_ARRAY_LEN> samplingRateTable;
  static int adtsHeaderSet( char* pAdtsHeader,//可以改变指向空间的内容，不允许改变指向空间的地址
                int len,
                int profile,
                int sampleRate,
                int channels);
public:
  static void extractAAC(const std::string_view& vPath, const std::string_view& aacPath);
};
#endif //AAC_EXTRACTOR_H
