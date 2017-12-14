
#include "FFmpegUtils.h"

bool FFmpegUtils::openVideo(const char *path) {
    closeVideo();
    mutex.lock();
    int ret = avformat_open_input(&ic, path, 0, 0);
    if (ret != 0) {
        mutex.unlock();
        av_strerror(ret, errorBuf, sizeof(errorBuf));
        return false;
    }
    if (ret == 0) {
        totalSec = ic->duration / AV_TIME_BASE;
        printf("file time is %d\n", totalSec);
    }
    for (int i = 0; i < ic->nb_streams; i++) {
        AVCodecContext *enc = ic->streams[i]->codec;
        if (enc->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            AVCodec *codec = avcodec_find_decoder(enc->codec_id);
            if (!codec) {
                printf("can't find video codec");
                return true;
                mutex.unlock();
            }
            videoCtx = enc;
            int err = avcodec_open2(enc, codec, NULL);
            if (err != 0) {
                char buf[1024] = {0};
                av_strerror(err, buf, sizeof(buf));
                printf(buf);
                mutex.unlock();
                return -1;
            }

        } else if (enc->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i;
        }
    }

    mutex.unlock();
    return true;
}


AVPacket FFmpegUtils::readVideo() {
    AVPacket pkt;
    memset(&pkt, 0, sizeof(AVPacket));
    mutex.lock();
    if (!ic) {
        mutex.unlock();
        return pkt;
    }
    int err = av_read_frame(ic, &pkt);
    if (err != 0) {
        av_strerror(err, errorBuf, sizeof(errorBuf));
    }
    mutex.unlock();
    return pkt;
}


//互斥
void FFmpegUtils::closeVideo() {
    mutex.lock();
    if (ic) {
        avformat_close_input(&ic);
    }
    if (yuvFrame) {
        av_frame_free(&yuvFrame);
    }
    if (swsCtx) {
        sws_freeContext(swsCtx);
    }
    mutex.unlock();
}

string FFmpegUtils::getError() {
    mutex.lock();
    string str = this->errorBuf;
    mutex.unlock();
    return str;
}

AVFrame *FFmpegUtils::decodeMedia(const AVPacket *pkt) {
    mutex.lock();
    if (!ic) {
        mutex.unlock();
        return NULL;
    }
    if (yuvFrame == NULL) {
        yuvFrame = av_frame_alloc();
    }
    int re = avcodec_send_packet(ic->streams[pkt->stream_index]->codec, pkt);
    if (re != 0) {
        mutex.unlock();
        return NULL;
    }
    re = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec, yuvFrame);
    if (re != 0) {
        mutex.unlock();
        return NULL;
    }
    mutex.unlock();
    return yuvFrame;
}

bool FFmpegUtils::toRgb(const AVFrame *yuv, int outWidth,
                        int outHeight) {

    mutex.lock();
    AVCodecContext *videoCtx = ic->streams[videoStream]->codec;
    swsCtx = sws_getCachedContext(NULL, videoCtx->width, videoCtx->height,
                                  videoCtx->pix_fmt, outWidth, outHeight,
                                  AV_PIX_FMT_BGRA, SWS_BICUBIC, NULL, NULL, NULL);
    uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
    char *rgb = new char[outWidth * outHeight * 4];
    data[0] = (uint8_t *) rgb;
    int linesize[AV_NUM_DATA_POINTERS] = {0};
    linesize[0] = outWidth * 4;
    int h = sws_scale(swsCtx, yuv->data, yuv->linesize, 0,
                      videoCtx->height, data, linesize);
    if (h > 0) {
        printf("convert success %d\n", h);

    }
    if (swsCtx) {
        sws_freeContext(swsCtx);
    }
    mutex.unlock();
    return true;
}


FFmpegUtils::FFmpegUtils() {
    errorBuf[0] = '\0';
    av_register_all();

}