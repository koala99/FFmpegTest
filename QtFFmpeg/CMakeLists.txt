cmake_minimum_required(VERSION 3.9)
project(QtFFmpeg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/usr/local/Cellar/ffmpeg/3.4.1/include/ ,
        /usr/local/Cellar/sdl2/2.0.7/include ,
        /usr/local/opt/qt/include)
link_directories(/usr/local/Cellar/ffmpeg/3.4.1/lib/ ,
        /usr/local/Cellar/sdl2/2.0.7/lib ,
        /usr/local/opt/qt/lib)


set(CMAKE_CXX_STANDARD 11)


set(CMAKE_PREFIX_PATH "/usr/local/Cellar/qt/5.10.0")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTOUIC ON) # .ui files
set(CMAKE_AUTORCC ON)

qt5_wrap_ui(ui_FILES mainwindow.ui)

#SET(CMAKE_C_COMPILER "/usr/local/Cellar/gcc/7.2.0/bin/gcc-7")
#SET(CMAKE_CXX_COMPILER "/usr/local/Cellar/gcc/7.2.0/bin/g++-7")




set(SOURCE_FILES main.cpp mainwindow.cpp mainwindow.h
        FFmpegUtils.cpp FFmpegUtils.h
        ${ui_FILES})




add_executable(QtFFmpeg  ${SOURCE_FILES})


target_link_libraries(
        QtFFmpeg
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
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui
)