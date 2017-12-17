
#include "FFmpegUtils.h"

static double r2d(AVRational avRational) {
    return avRational.num == 0 ? 0 : (double) avRational.num / (double) avRational.den;
}

int FFmpegUtils::openVideo(const char *path) {
    closeVideo();
    mutex.lock();
    int ret = avformat_open_input(&ic, path, 0, 0);
    if (ret != 0) {
        mutex.unlock();
        av_strerror(ret, errorBuf, sizeof(errorBuf));
        return 0;
    }
    if (ret == 0) {
        totalSec = ic->duration / AV_TIME_BASE;
        printf("file time is %d\n", totalSec);
    }
    for (int i = 0; i < ic->nb_streams; i++) {
        AVCodecContext *enc = ic->streams[i]->codec;
        if (enc->codec_type == AVMEDIA_TYPE_VIDEO) {//解码视频
            videoStream = i;
            fps = r2d(ic->streams[i]->avg_frame_rate);
            AVCodec *codec = avcodec_find_decoder(enc->codec_id);
            if (!codec) {
                printf("can't find video codec");
                mutex.unlock();
                return totalSec;
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

        } else if (enc->codec_type == AVMEDIA_TYPE_AUDIO) {//解码音频
            audioStream = i;

            AVCodec *codec = avcodec_find_decoder(enc->codec_id);
            if (avcodec_open2(enc, codec, NULL) < 0) {
                mutex.unlock();
                printf("can't find audio codec");
                return -1;
            }
            sampleRate = enc->sample_rate;
            channel = enc->channels;
//            av_get_bytes_per_sample(enc->sample_fmt);
//            switch (enc->sample_fmt) {
//
//                case AV_SAMPLE_FMT_NONE:
//                    sampleSize = -1;
//                    break;
//                case AV_SAMPLE_FMT_U8:
//                    sampleSize = 8;
//                    break;
//                case AV_SAMPLE_FMT_S16:
//                    sampleSize = 16;
//                    break;
//                case AV_SAMPLE_FMT_S32:
//                    sampleSize = 32;
//                    break;
//                case AV_SAMPLE_FMT_FLT:
//                case AV_SAMPLE_FMT_DBL:
//                    sampleSize = 36;
//                    break;
//
//                case AV_SAMPLE_FMT_U8P:
//                    printf("rate:1");
//                    break;
//                case AV_SAMPLE_FMT_S16P:
//                    printf("rate:2");
//                    break;
//                case AV_SAMPLE_FMT_S32P:
//                    printf("rate:3");
//                    break;
//                case AV_SAMPLE_FMT_FLTP:
//                    printf("rate:4");
//                    break;
//                case AV_SAMPLE_FMT_DBLP:
//                    printf("rate:5");
//                    break;
//                case AV_SAMPLE_FMT_S64:
//                    printf("rate:5");
//                    break;
//                case AV_SAMPLE_FMT_S64P:
//                    printf("rate:6");
//                    break;
//                default:
//                    sampleSize = 16;
//            }
        }
    }
//    printf("rate:%d;channels:%d;sampleSize:%d \n", sampleRate,
//           channel, sampleSize);

    mutex.unlock();
    return totalSec;
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

int FFmpegUtils::getPts(const AVPacket *pkt) {
    mutex.lock();
    if (!ic) {
        mutex.unlock();
        return -1;
    }
    int pts = pkt->pts * r2d(ic->streams[pkt->stream_index]->time_base);
    mutex.unlock();
    return pts;
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
    if (audioFrame) {
        av_frame_free(&audioFrame);
    }
    if (swsCtx) {
        sws_freeContext(swsCtx);
    }
    if (swrCtx) {
        swr_free(&swrCtx);
    }
    mutex.unlock();
}

string FFmpegUtils::getError() {
    mutex.lock();
    string str = this->errorBuf;
    mutex.unlock();
    return str;
}

int FFmpegUtils::decodeMedia(const AVPacket *pkt) {
    mutex.lock();
    if (!ic) {
        mutex.unlock();
        return NULL;
    }
    if (yuvFrame == NULL) {
        yuvFrame = av_frame_alloc();
    }
    if (audioFrame == NULL) {
        audioFrame = av_frame_alloc();
    }
    AVFrame *frame = yuvFrame;
    if (pkt->stream_index == audioStream) {
        frame = audioFrame;
    } else {
        frame = yuvFrame;
    }
    int re = avcodec_send_packet(ic->streams[pkt->stream_index]->codec, pkt);
    if (re != 0) {
        mutex.unlock();
        return NULL;
    }
    re = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec, frame);
    if (re != 0) {
        mutex.unlock();
        return NULL;
    }
    mutex.unlock();
    pts = frame->pts * r2d(ic->streams[pkt->stream_index]->time_base);
    //可以在改变音频时记录进度
    return pts;
}

bool FFmpegUtils::toRgb(char *out, int outWidth,
                        int outHeight) {

    mutex.lock();
    if (!ic || !yuvFrame) {
        mutex.unlock();
        return false;
    }
    AVCodecContext *videoCtx = ic->streams[videoStream]->codec;
    swsCtx = sws_getCachedContext(NULL, videoCtx->width, videoCtx->height,
                                  videoCtx->pix_fmt, outWidth, outHeight,
                                  AV_PIX_FMT_BGRA, SWS_BICUBIC, NULL, NULL, NULL);
    uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
//    char *rgb = new char[outWidth * outHeight * 4];
//    data[0] = (uint8_t *) rgb;
    data[0] = (uint8_t *) out;
    int linesize[AV_NUM_DATA_POINTERS] = {0};
    linesize[0] = outWidth * 4;
    int h = sws_scale(swsCtx, yuvFrame->data, yuvFrame->linesize, 0,
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

int FFmpegUtils::toPcm(char *out) {
    mutex.lock();
    if (!ic || !audioFrame || !out) {
        mutex.unlock();
        return 0;
    }
    AVCodecContext *ctx = ic->streams[audioStream]->codec;
    if (swrCtx == NULL) {
        swrCtx = swr_alloc();
//        printf("layout:%d;rate:%d;channel:%d; \n", ctx->channel_layout, ctx->sample_rate, ctx->channels);
        swr_alloc_set_opts(swrCtx, ctx->channel_layout,
                           AV_SAMPLE_FMT_S16, ctx->sample_rate, ctx->channels,
                           ctx->sample_fmt, ctx->sample_rate, 0, 0); //S16
        swr_init(swrCtx);
    }
//    AV_SAMPLE_FMT_S16;
//    AV_SAMPLE_FMT_FLTP
    uint8_t *data[1];
    data[0] = (uint8_t *) out;
    int len = swr_convert(swrCtx, data, 10000, (const uint8_t **) audioFrame->data,
                          audioFrame->nb_samples);
    printf("convert_len:%d\n", len);

    if (len < 0) {
        mutex.unlock();
        return 0;
    }
    int toltal = ctx->frame_size * 2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_FLTP);
    int outsize = av_samples_get_buffer_size(NULL, ctx->channels,
                                             audioFrame->nb_samples, AV_SAMPLE_FMT_S16, 0); //
    mutex.unlock();
    return outsize;
}


FFmpegUtils::FFmpegUtils() {
    errorBuf[0] = '\0';
    av_register_all();
}


