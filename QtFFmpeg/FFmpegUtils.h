
#include <string>
#include <QMutex>

extern "C" {
#include<libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
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
    SwsContext *swsCtx = NULL;
    AVCodecContext *videoCtx;

    QMutex mutex;
    int videoStream, audioStream;

    int totalSec = 0;

public:
    static FFmpegUtils *Get() {
        static FFmpegUtils ff;
        return &ff;
    }

    bool openVideo(const char *path);

    AVPacket readVideo();

    void closeVideo();

    AVFrame *decodeMedia(const AVPacket *pkt);

    string getError();

    bool toRgb(const AVFrame *yuv, int outwidth,
               int outheight);
    //析构函数
//    virtual ~FFmpegUtils();
};


#endif //QTFFMPEG_FFMPEGUTILS_H




