//#include <iostream>

#include "mainwindow.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qapplication.h>
#include "FFmpegUtils.h"


int decodeVideo();

static double r2d(AVRational avRational) {
    return avRational.num == 0 ? 0 : (double) avRational.num / (double) avRational.den;

}

int main(int argc, char *argv[]) {
    decodeVideo();
//    QScopedPointer<QApplication> app(new QApplication(argc, argv));
//    QScopedPointer<QLabel> label(new QLabel("视频测试"));
//    label->show();
//    return app->exec();

}

int decodeVideo() {
    char *path = "test.mp4";
    if (FFmpegUtils::Get()->openVideo(path)) {
        printf("open success");
    } else {
        printf("open failed %s", FFmpegUtils::Get()->getError().c_str());
        return -1;
    }
    while (1) {
        AVPacket pkt = FFmpegUtils::Get()->readVideo();
        if (pkt.size == 0)
            break;
        printf("pts= %d\n", pkt.pts);
        AVFrame *yuv = FFmpegUtils::Get()->decodeMedia(&pkt);
        if (yuv) {
            printf("decode success\n");
            FFmpegUtils::Get()->toRgb(yuv, 800, 600);
        }
        av_packet_unref(&pkt);
    }

//    printf("ic_stream:%d", ic->nb_streams);
//    for (int i = 0; i < ic->nb_streams; i++) {
//        AVCodecContext *enc = ic->streams[i]->codec;
//        if (enc->codec_type == AVMEDIA_TYPE_VIDEO) {
//            videoStream = i;
//            videoCtx = enc;
//            AVCodec *codec = avcodec_find_decoder(enc->codec_id);
//            if (!codec) {
//                printf("can't find video codec");
//            }
//            int err = avcodec_open2(enc, codec, NULL);
//            if (err != 0) {
//                char buf[1024] = {0};
//                av_strerror(err, buf, sizeof(buf));
//                printf(buf);
//                return -1;
//            }
//
//        } else if (enc->codec_type == AVMEDIA_TYPE_AUDIO) {
//            audioStream = i;
//        }
//    }
//    AVFrame *yuvFrame = av_frame_alloc();
//    SwsContext *swsCtx = NULL;
//    int outWidth = 480;
//    int outHeight = 640;
//    char *rgb = new char[outWidth * outHeight * 4];
//
//    while (1) {
//        AVPacket pkt;
//        ret = av_read_frame(ic, &pkt);
//        if (ret != 0) {
//            printf("读取frame 失败");
//            break;
//        }
//        int pts = pkt.pts * r2d(ic->streams[pkt.stream_index]->time_base) * 1000;
//////  老写法
////        int got_picture = 0;
////        int re = avcodec_decode_video2(videoCtx, yuvFrame, &got_picture, &pkt);
////        if (got_picture) {
////            printf("[%d]", re);
////        }
////        printf("pts= %d\n", pts);
//
//
//        int re = avcodec_send_packet(videoCtx, &pkt);
//        if (re != 0) {
//            av_packet_unref(&pkt);
//            continue;
//        }
//
//        swsCtx = sws_getCachedContext(NULL, videoCtx->width, videoCtx->height,
//                                      videoCtx->pix_fmt, outWidth, outHeight,
//                                      AV_PIX_FMT_BGRA, SWS_BICUBIC, NULL, NULL, NULL);
//        if (!swsCtx) {
//            printf("get swsContext false");
//            return -1;
//        }
//
//        if (avcodec_receive_frame(videoCtx, yuvFrame) == 0) {
//            printf("pts= %d\n", pts);
//        }
//        uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
//        data[0] = (uint8_t *) rgb;
//        int linesize[AV_NUM_DATA_POINTERS] = {0};
//        linesize[0] = outWidth * 4;
//        int h = sws_scale(swsCtx, yuvFrame->data, yuvFrame->linesize, 0,
//                          videoCtx->height, data, linesize);
//        if (h > 0) {
//            printf("convert success %d\n", h);
//
//        }
//        printf("pts= %d\n", pts);
//        if (swsCtx) {
//            sws_freeContext(swsCtx);
//        }
//        av_packet_unref(&pkt);
//    }
}
