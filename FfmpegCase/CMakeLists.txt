cmake_minimum_required(VERSION 3.10)
project(VideoDemo)

set(CMAKE_CXX_STANDARD 11)
include_directories(/usr/local/Cellar/ffmpeg/4.1.3_1/include /usr/local/Cellar/lame/3.100/include)
link_directories(/usr/local/Cellar/ffmpeg/4.1.3_1/lib /usr/local/Cellar/lame/3.100/lib)

add_executable(VideoDemo main.cpp utils.h utils.cpp base_utils.cpp base_utils.h)

target_link_libraries(
        VideoDemo
        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
        mp3lame
)





