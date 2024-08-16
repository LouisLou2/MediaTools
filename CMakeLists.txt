cmake_minimum_required(VERSION 3.28)
project(ff_test)

set(CMAKE_CXX_STANDARD 20)

set(SYS_INCLUDE_DIR "/usr/include")
set(SYS_X64_INCLUDE_DIR "${SYS_INCLUDE_DIR}/x86_64-linux-gnu")
set(SYS_LIB_DIR "/usr/lib/x86_64-linux-gnu")

include_directories(
    ${SYS_X64_INCLUDE_DIR}
)

add_executable(
    ff_test
    main.cpp
    ff_demux.cpp
    extract_aac.cpp
    ff_util.cpp
    h264_annexb.cpp
    audio_pcm.cpp
    video_yuv.cpp
)

add_compile_definitions(__STDC_CONSTANT_MACROS)

find_package(PkgConfig REQUIRED)

pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(AVFILTER REQUIRED IMPORTED_TARGET libavfilter)

target_link_libraries(ff_test
    PkgConfig::AVCODEC
    PkgConfig::AVFORMAT
    PkgConfig::AVUTIL
    PkgConfig::AVFILTER
)