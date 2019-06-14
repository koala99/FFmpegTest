#include <iostream>
#include "utils.h"
#include "base_utils.h"

using namespace std;


int main() {
//    cout << "当前的时间戳是:" << BaseUtils().getTimeStamp() << endl;
    changeMp3("../file/test.mp4", "../file/test.mp3");
//    decodeVideo2YuvPcm("../file/test.mp4");
//    encodeYuvPcm2Video("/Users/lilei/Desktop/media_project/video.yuv",
//                       "/Users/lilei/Desktop/media_project/audio.pcm");
//    encodePcm2Mp3("/Users/lilei/Desktop/media_project/audio.pcm");
    return 0;
}


//yuv420p:标准yuv格式 I420
//YV12和
