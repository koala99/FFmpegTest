cmake_minimum_required(VERSION 3.9)
project(FFmpegDemo)

include_directories(/usr/local/Cellar/ffmpeg/3.4/include/ , /usr/local/Cellar/sdl2/2.0.7/include)
link_directories(/usr/local/Cellar/ffmpeg/3.4/lib/ , /usr/local/Cellar/sdl2/2.0.7/lib)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp)
add_executable(FFmpegDemo ${SOURCE_FILES})





target_link_libraries(
        FFmpegDemo
        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
        SDL2
)

