#include <iostream>
#include "utils.h"
#include "base_utils.h"

using namespace std;


int main() {
//    sdl2Player("../file/test.mp4");
//    cout << "当前的时间戳是:" << BaseUtils().getTimeStamp() << endl;
//    changeMp3("../file/test.mp4", "../file/test.mp3");
//    decodeVideo2YuvPcm("../file/test.mp4");
//    encodeYuvPcm2Video("/Users/lilei/Desktop/media_project/video.yuv",
//                       "/Users/lilei/Desktop/media_project/audio.pcm");
//    encodePcm2Mp3("/Users/lilei/Desktop/media_project/audio.pcm");

//    clipVideo("../file/test.mp4", "test1.mp4", 5.0, 15.0);
    audioPlayer("../file/test.mp4");
    return 0;
}


//yuv420p:标准yuv格式 I420
//YV12和
/**
 * DTS（Decoding Time Stamp）：即解码时间戳，这个时间戳的意义在于告诉播放器该在什么时候解码这一帧的数据。
   PTS（Presentation Time Stamp）：即显示时间戳，这个时间戳用来告诉播放器该在什么时候显示这一帧的数据。
 */
