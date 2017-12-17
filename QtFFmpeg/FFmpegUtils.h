
#include <string>
#include <QMutex>


extern "C" {
#include<libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
using namespace std;

#ifndef QTFFMPEG_FFMPEGUTILS_H
#define QTFFMPEG_FFMPEGUTILS_H


class FFmpegUtils {


protected:
    FFmpegUtils();

    char errorBuf[1024];
    AVFormatContext *ic = NULL;
    AVFrame *yuvFrame = NULL;
    AVFrame *audioFrame = NULL;
    SwsContext *swsCtx = NULL;
    SwrContext *swrCtx = NULL;
    AVCodecContext *videoCtx;

    QMutex mutex;

public:
    int videoStream, audioStream;
    int totalSec = 0;
    int fps = 0;
    bool isPlaying = false;
    int pts = 0;

    int sampleRate = 0;
    int channel = 0;
//    int sampleSize=0;

    static FFmpegUtils *Get() {
        static FFmpegUtils ff;
        return &ff;
    }

    int getPts(const AVPacket *pkt);

    int toPcm(char *out);

    int openVideo(const char *path);


    AVPacket readVideo();

    void closeVideo();

    int decodeMedia(const AVPacket *pkt);

    string getError();

    bool toRgb(char *out, int outwidth,
               int outheight);
    //析构函数
//    virtual ~FFmpegUtils();
};


#endif //QTFFMPEG_FFMPEGUTILS_H




