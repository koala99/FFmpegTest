#include "utils.h"
#include <iostream>


extern "C" {
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
#include<lame/lame.h>

}
using namespace std;


void decodeVideo2YuvPcm(char *filepath) {
    AVFormatContext *pFormatCtx;
    int i, videoindex, audioindex;
    AVCodecContext *videoCodecCtx,
            *audioCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame;
    uint8_t *out_buffer;
    AVPacket *packet;
    AVCodecParameters *vCodecPar, *aCodecPar;
    int y_size;
    int ret;
    struct SwsContext *img_convert_ctx;


    FILE *video_fp_yuv = fopen("../file/video.yuv", "wb+");
    FILE *audio_fp_pcm = fopen("../file/audio.pcm", "wb+");

//    FILE *fp_h264 = fopen("output.h264", "wb+");


    av_register_all();//注册所有组件
    avformat_network_init();//初始化网络
    pFormatCtx = avformat_alloc_context();//初始化一个AVFormatContext

    if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {//打开输入的视频文件
        printf("Couldn't open input stream.\n");
        return;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {//获取视频文件信息
        printf("Couldn't find stream information.\n");
        return;
    }
    videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
        } else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioindex = i;
        }

    if (videoindex == -1) {
        printf("Didn't find a video stream.\n");
        return;
    }

    vCodecPar = pFormatCtx->streams[videoindex]->codecpar;
    videoCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(vCodecPar->codec_id);//查找解码器
    if (pCodec == NULL) {
        printf("Codec not found.\n");
        return;
    }
    if (avcodec_open2(videoCodecCtx, pCodec, NULL) < 0) {//打开解码器
        printf("Could not open codec.\n");
        return;
    }


    aCodecPar = pFormatCtx->streams[audioindex]->codecpar;
    audioCodecCtx = pFormatCtx->streams[audioindex]->codec;
    pCodec = avcodec_find_decoder(aCodecPar->codec_id);//查找解码器

    if (pCodec == NULL) {
        printf("Codec not found.\n");
        return;
    }
    if (avcodec_open2(audioCodecCtx, pCodec, NULL) < 0) {//打开解码器
        printf("Could not open codec.\n");
        return;
    }


    pFrame = av_frame_alloc();
//    out_buffer = (uint8_t *) av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
//    avpicture_fill((AVPicture *) pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_dump_format(pFormatCtx, 0, filepath, 0);
    printf("-------------------------------------------------\n");

    while (av_read_frame(pFormatCtx, packet) >= 0) {//读取一帧压缩数据

        if (packet->stream_index == videoindex) {
            avcodec_send_packet(videoCodecCtx, packet);
            ret = avcodec_receive_frame(videoCodecCtx, pFrame);
            switch (ret) {
                case 0:
                    break;
                case AVERROR_EOF:
                    break;
                case AVERROR(EAGAIN):
                    break;
                case AVERROR(EINVAL):
                    printf("Invalid argument\n");
                    break;
                default:
                    break;
            }
            // Write pixel data
            if (videoCodecCtx->pix_fmt == AV_PIX_FMT_YUV420P|| videoCodecCtx->pix_fmt == AV_PIX_FMT_YUVJ420P) {
                for (int y = 0; y < vCodecPar->height; y++)
                    fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, vCodecPar->width, video_fp_yuv);
                for (int y = 0; y < vCodecPar->height / 2; y++) {
                    fwrite(pFrame->data[1] + y * pFrame->linesize[1], 1, vCodecPar->width / 2, video_fp_yuv);
                }
                for (int y = 0; y < vCodecPar->height / 2; y++) {
                    fwrite(pFrame->data[2] + y * pFrame->linesize[2], 1, vCodecPar->width / 2, video_fp_yuv);
                }
            } else if (videoCodecCtx->pix_fmt == AV_PIX_FMT_YUV422P) {

            }   // Close file

        } else if (packet->stream_index == audioindex) {
            avcodec_send_packet(audioCodecCtx, packet);
            ret = avcodec_receive_frame(audioCodecCtx, pFrame);
            int out_buffer_size = pFrame->nb_samples * av_get_bytes_per_sample(audioCodecCtx->sample_fmt);
            //写入文件进行测试
//            int size = av_get_bytes_per_sample(pFormatCtx->streams[audioindex]->codec->sample_fmt);

            int data_size = av_get_bytes_per_sample(audioCodecCtx->sample_fmt);

//            if (pFrame->data[0] && pFrame->data[1]) {
//                for (int i = 0; i < pFormatCtx->streams[audioindex]->codec->frame_size; i++) {
//                    fwrite(pFrame->data[0] + i * size, 1, size, audio_fp_pcm);
//                    fwrite(pFrame->data[1] + i * size, 1, size, audio_fp_pcm);
//                }
//            } else if (pFrame->data[0]) {
//                printf("1\n");
//
//                fwrite(pFrame->data[0], 1, pFrame->linesize[0], audio_fp_pcm);
//            }

            for (int i = 0; i < pFrame->nb_samples; i++) {
                for (int ch = 0; ch < audioCodecCtx->channels; ch++) {
                    fwrite(pFrame->data[ch] + data_size * i, 1, data_size, audio_fp_pcm);
                }
            }


//            for (int i = 0; i < pFrame->nb_samples; i++) {
//                for (int ch = 0; ch < audioCodecCtx->channels; ch++) {
//                    fwrite(pFrame->data[ch] + out_buffer_size * i, 1, out_buffer_size, audio_fp_pcm);
//                }
//            }
        }
    }
    fclose(audio_fp_pcm);
    fclose(video_fp_yuv);
    av_packet_unref(packet);
    av_free(pFrame);
    avcodec_close(videoCodecCtx);
    avcodec_close(audioCodecCtx);
    avformat_close_input(&pFormatCtx);
}

void encodeYuvPcm2Video(char *yuvPath, char *pcmPath) {

}


void encodeH264AAac2Video(char *h264Path, char *aacPath) {

}


void docedeVideo2H264Aac(char *filepath) {

}

void encodePcm2Mp3(char *pcmPath) {
    int ret = -1;
    FILE *pcmFile;
    FILE *mp3File;
    lame_t lameClient;

    int sampleRate = 44100;


    pcmFile = fopen(pcmPath, "rb");
    if (pcmFile) {
        mp3File = fopen("/Users/lilei/Desktop/media_project/test.mp3", "wb+");
    }
    if (mp3File) {
        lameClient = lame_init();
        lame_set_in_samplerate(lameClient, sampleRate);
        lame_set_out_samplerate(lameClient, sampleRate);
        lame_set_num_channels(lameClient, 2);//采样率
//        lame_set_brate(lameClient, 128);  /* 比特率 */
        lame_set_quality(lameClient, 2); //0到9 越小越好
        ret = lame_init_params(lameClient);
    }
    if (ret < 0) {
        cout << "配置参数失败" << endl;
    } else {
        cout << "配置参数成功" << endl;
    }
    int read, write = 0;
    int bufferSize = 8192;
    short int pcm_buffer[bufferSize * 2];
    unsigned char mp3_buffer[bufferSize];
    do {
        read = fread(pcm_buffer, 2 * sizeof(short int), bufferSize, pcmFile);
        if (read == 0)
            write = lame_encode_flush(lameClient, mp3_buffer, bufferSize);
        else
            write = lame_encode_buffer_interleaved(lameClient, pcm_buffer, read, mp3_buffer, bufferSize);
        fwrite(mp3_buffer, write, 1, mp3File);

    } while (read != 0);
//    delete[] pcm_buffer;
//    delete[] mp3_buffer;
    if (pcmFile)
        fclose(pcmFile);
    if (mp3File) {
        fclose(mp3File);
        lame_close(lameClient);
    }

}
