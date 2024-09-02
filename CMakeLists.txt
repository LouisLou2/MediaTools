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
    ff_util.cpp
    header/media_info.h
    header/yuv_converter.h
    media_converter_impl/yuv_converter.cpp
    header/media_utils.h
    media_converter_impl/pcm_converter.cpp
    header/pcm_converter.h
    media_info_impl/media_info.cpp
    extractor_impl/aac_extractor.cpp
    header/aac_extractor.h
    header/media_format_conv.h
    media_format_conv_impl/media_format_conv.cpp
    header/media_trans_coding.h
    media_trans_coding_impl/media_trans_coding.cpp
    # simple_dct.cpp
)

add_compile_definitions(__STDC_CONSTANT_MACROS)

find_package(PkgConfig REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(AVFILTER REQUIRED IMPORTED_TARGET libavfilter)

target_link_libraries(ff_test
    PkgConfig::AVCODEC
    PkgConfig::AVFORMAT
    PkgConfig::AVUTIL
    PkgConfig::AVFILTER
    Eigen3::Eigen
)