#include "VideoThread.h"
#include "FFmpegUtils.h"
#include "AudioPlay.h"
#include <list>

using namespace std;
static list<AVPacket> videos;
static int apts = -1;

void VideoThread::run() {
    char out[10000] = {0};
    while (!isStop) {
        if (!FFmpegUtils::Get()->isPlaying) {
            msleep(10);
            continue;
        }

        while (videos.size() > 0) {
            AVPacket pack = videos.front();
            int pts = FFmpegUtils::Get()->getPts(&pack);
            if (pts > apts) {
                break;
            }
            FFmpegUtils::Get()->decodeMedia(&pack);
            av_packet_unref(&pack);
            videos.pop_front();
        }
        int free = AudioPlay::Get()->getFree();
        printf("size:%d \n", free);
        if (free < 4096) { //算出一帧大小
            msleep(1);
            continue;
        }
        AVPacket pkt = FFmpegUtils::Get()->readVideo();
        if (pkt.size < 0) {
            msleep(10);
            continue;
        }
        if (pkt.stream_index == FFmpegUtils::Get()->audioStream) {
            apts = FFmpegUtils::Get()->decodeMedia(&pkt);
            av_packet_unref(&pkt);
            int len = FFmpegUtils::Get()->toPcm(out);
            printf("len:%d \n", len);
            AudioPlay::Get()->writeData(out, len);
            continue;
        }
        if (pkt.stream_index != FFmpegUtils::Get()->videoStream) {//防止其他流，比如字幕流
            av_packet_unref(&pkt);
            continue;
        }
        videos.push_back(pkt);

        //根据视频帧率控制速度
//        if (FFmpegUtils::Get()->fps > 0)
//            msleep(1000 / FFmpegUtils::Get()->fps);
    }
}


void VideoThread::stop() {
    isStop = true;
    quit();
    wait();
}

VideoThread::VideoThread() {

}
