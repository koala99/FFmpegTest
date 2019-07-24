#include "utils.h"
#include <iostream>


extern "C" {
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
#include<libavutil/imgutils.h>
#include<libswscale/swscale.h>
#include <libswresample/swresample.h>
#include<libavdevice/avdevice.h>
#include "libavutil/time.h"

#include<lame/lame.h>
#include <SDL2/SDL.h>

}
using namespace std;


#define MAX_AUDIO_FRAME_SIZE 19200
static Uint8 *audio_chunk;
static Uint32 audio_len;
static Uint8 *audio_pos;


//音频设备需要更多数据的时候会调用该回调函数
void read_audio_data(void *udata, Uint8 *stream, int len) {
    fprintf(stderr, "stream addr:%p, audio_len:%d, len:%d\n",
            stream,
            audio_len,
            len);
    //首先使用SDL_memset()将stream中的数据设置为0
    SDL_memset(stream, 0, len);
    if (audio_len == 0)
        return;
    len = (len > audio_len ? audio_len : len);

    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}


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
            if (videoCodecCtx->pix_fmt == AV_PIX_FMT_YUV420P || videoCodecCtx->pix_fmt == AV_PIX_FMT_YUVJ420P) {
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

void encodePcm2Mp3(char *mp3Path, char *pcmPath) {
    int ret = -1;
    FILE *pcmFile;
    FILE *mp3File;
    lame_t lameClient;

    int sampleRate = 44100;


    pcmFile = fopen(pcmPath, "rb");
    if (pcmFile) {
        mp3File = fopen(mp3Path, "wb+");
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


void changeMp3(char *videoPath, char *mp3Path) {
    AVOutputFormat *pOutputFmt = NULL;
    AVFormatContext *pVideoInFmtCxt = NULL;//输入的视频的FormatContext
    AVFormatContext *pAudioInFmtCxt = NULL;//输入的音频的FormatContext
    AVFormatContext *pOutFmtCxt = NULL;//输出的音视频的FormatContext
    AVPacket pkt;
    int ret, i;
    int videoindex_v = -1, videoindex_out = -1;
    int audioindex_a = -1, audioindex_out = -1;
    int frame_index = 0;
    int64_t cur_pts_v = 0;//视频的pts
    int64_t cur_pts_a = 0;//音频的pts

    char errorbuf[1024] = {0};

    const char *pVideoOutFilePath = "outBg.mp4";


    av_register_all();
    //打开输入的视频文件
    if ((ret = avformat_open_input(&pVideoInFmtCxt, videoPath, NULL, NULL)) < 0) {
        printf("Could not open input video file.");
        return;
    }
    //获取视频文件信息
    if ((ret = avformat_find_stream_info(pVideoInFmtCxt, NULL)) < 0) {
        printf("Failed to retrieve input video stream information");
        return;
    }


    //初始化输出码流的AVFormatContext //pOutFmtCxt,NULL,NULL,pVideoOutFilePath
    avformat_alloc_output_context2(&pOutFmtCxt, NULL, NULL, pVideoOutFilePath);
    if (!pOutFmtCxt) {
        printf("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        return;
    }

    pOutputFmt = pOutFmtCxt->oformat;

    //从输入的AVStream中获取一个输出的out_stream，视频输出流
    for (i = 0; i < pVideoInFmtCxt->nb_streams; i++) {
        //根据输入的视频流创建一个输出的视频流
        if (pVideoInFmtCxt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVStream *in_stream = pVideoInFmtCxt->streams[i];
            //创建输出流通道AVStream
            AVCodecContext *pCodecCtx = avcodec_alloc_context3(NULL);
            if (pCodecCtx == NULL) {
                printf("Could not allocate AVCodecContext\n");
                return;
            }
            avcodec_parameters_to_context(pCodecCtx, in_stream->codecpar);

            AVStream *out_stream = avformat_new_stream(pOutFmtCxt, pCodecCtx->codec);
            videoindex_v = i;
            if (!out_stream) {
                printf("Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                break;
            }
            videoindex_out = out_stream->index;

            //Copy the settings of AVCodecContext
            if ((ret = avcodec_parameters_from_context(out_stream->codecpar, pCodecCtx)) < 0) {
                printf("Failed to copy codec context to out_stream codecpar context\n");
                return;
            }

            pCodecCtx->codec_tag = 0;
            if (pOutFmtCxt->oformat->flags & AVFMT_GLOBALHEADER)
                pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            break;
        }
    }

    //从输入的AVStream中获取一个输出的out_stream，音频输出流
    for (i = 0; i < pAudioInFmtCxt->nb_streams; i++) {
        if (pAudioInFmtCxt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            AVStream *in_stream = pAudioInFmtCxt->streams[i];
            //创建输出流通道AVStream
            AVCodecContext *pCodecCtx = avcodec_alloc_context3(NULL);
            if (pCodecCtx == NULL) {
                printf("Could not allocate AVCodecContext\n");
                return;
            }
            avcodec_parameters_to_context(pCodecCtx, in_stream->codecpar);
            AVStream *out_stream = avformat_new_stream(pOutFmtCxt, pCodecCtx->codec);

            audioindex_a = i;
            if (!out_stream) {
                printf("Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                return;
            }
            audioindex_out = out_stream->index;

            if ((ret = avcodec_parameters_from_context(out_stream->codecpar, pCodecCtx)) < 0) {
                printf("Failed to copy codec context to out_stream codecpar context\n");
                return;
            }

            pCodecCtx->codec_tag = 0;
            if (pOutFmtCxt->oformat->flags & AVFMT_GLOBALHEADER)
                pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            break;
        }
    }

    //打开输出文件
    if (!(pOutputFmt->flags & AVFMT_NOFILE)) {
        if (avio_open(&pOutFmtCxt->pb, pVideoOutFilePath, AVIO_FLAG_WRITE) < 0) {//打开输出文件。
            printf("Could not open output file '%s'", pVideoOutFilePath);
            return;
        }
    }

    //写文件头
    if (avformat_write_header(pOutFmtCxt, NULL) < 0) {
        printf("Error occurred when opening output file\n");
        return;
    }

#if USE_H264BSF
    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
    AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
#endif
    while (1) {
        AVFormatContext *pInFmtCtx;
        int stream_index = 0;
        AVStream *in_stream, *out_stream;
        // av_compare_ts是比较时间戳用的。通过该函数可以决定该写入视频还是音频
        if (av_compare_ts(cur_pts_v, pVideoInFmtCxt->streams[videoindex_v]->time_base,
                          cur_pts_a, pAudioInFmtCxt->streams[audioindex_a]->time_base) <= 0) {
            //printf("写视频数据");
            pInFmtCtx = pVideoInFmtCxt;//视频
            //这里要赋值了，注意注意
            stream_index = videoindex_out;

            if (av_read_frame(pInFmtCtx, &pkt) >= 0) {//读取流
                do {
                    in_stream = pInFmtCtx->streams[pkt.stream_index];
                    out_stream = pOutFmtCxt->streams[stream_index];

                    if (pkt.stream_index == videoindex_v) {
                        // H.264裸流没有PTS，因此必须手动写入PTS
                        if (pkt.pts == AV_NOPTS_VALUE) {
                            //写PTS
                            AVRational time_base1 = in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration = (double) AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts = (double) (frame_index * calc_duration) /
                                      (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            pkt.dts = pkt.pts;
                            pkt.duration = (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_v = pkt.pts;
                        //printf("cur_pts_v === %lld",cur_pts_v);
                        break;
                    }
                } while (av_read_frame(pInFmtCtx, &pkt) >= 0);
            } else {
                break;
            }
        } else {
            printf("写音频数据");
            pInFmtCtx = pAudioInFmtCxt;
            stream_index = audioindex_out;
            if (av_read_frame(pInFmtCtx, &pkt) >= 0) {
                do {
                    in_stream = pInFmtCtx->streams[pkt.stream_index];
                    out_stream = pOutFmtCxt->streams[stream_index];

                    if (pkt.stream_index == audioindex_a) {
                        //FIX：No PTS
                        //Simple Write PTS
                        if (pkt.pts == AV_NOPTS_VALUE) {
                            //Write PTS
                            AVRational time_base1 = in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration = (double) AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts = (double) (frame_index * calc_duration) /
                                      (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            pkt.dts = pkt.pts;
                            pkt.duration = (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_a = pkt.pts;
                        printf("cur_pts_a === %lld", cur_pts_a);
                        break;
                    }
                } while (av_read_frame(pInFmtCtx, &pkt) >= 0);
            } else {
                break;
            }

        }
        //FIX:Bitstream Filter
#if USE_H264BSF
        av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
#if USE_AACBSF
        av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
        printf("pkt.pts = %lld ", pkt.pts);
        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        printf("pkt.pts == %lld", pkt.pts);
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index = stream_index;
        printf("Write 1 Packet. size:%5d\tpts:%lld\n", pkt.size, pkt.pts);
        //Write AVPacket 音频或视频裸流
        if ((ret = av_interleaved_write_frame(pOutFmtCxt, &pkt)) < 0) {
            av_strerror(ret, errorbuf, sizeof(errorbuf));
            printf("ERROR : %s", errorbuf);
            printf("Error muxing packet error code === %d\n", ret);
            av_packet_unref(&pkt);
            break;
        }
        //av_free_packet(&pkt);
        av_packet_unref(&pkt);
    }

    //写文件尾
    av_write_trailer(pOutFmtCxt);

#if USE_H264BSF
    av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
    av_bitstream_filter_close(aacbsfc);
#endif

    avformat_close_input(&pVideoInFmtCxt);
    avformat_close_input(&pAudioInFmtCxt);
    if (pOutFmtCxt && !(pOutputFmt->flags & AVFMT_NOFILE))
        avio_close(pOutFmtCxt->pb);
    avformat_free_context(pOutFmtCxt);
    if (ret < 0 && ret != AVERROR_EOF) {
        printf("Error occurred.\n");
        return;
    }
}

void sdl2Player(char *file) {
    AVFormatContext *pFormatCtx = NULL;
    int videoStream;
    AVCodecParameters *pCodecParameters = NULL;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    AVFrame *pFrame = NULL;
    AVPacket packet;

    SDL_Rect rect;
    Uint32 pixformat;
    SDL_Window *win = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;

    //默认窗口大小
    int w_width = 640;
    int w_height = 480;

    //SDL初始化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL - %s\n", SDL_GetError());
        return;
    }


    // 打开输入文件
    if (avformat_open_input(&pFormatCtx, file, NULL, NULL) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open  video file!");
        goto __FAIL;
    }

    //找到视频流
    videoStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoStream == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Din't find a video stream!");
        goto __FAIL;// Didn't find a video stream
    }

    // 流参数
    pCodecParameters = pFormatCtx->streams[videoStream]->codecpar;

    //获取解码器
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unsupported codec!\n");
        goto __FAIL; // Codec not found
    }

    // 初始化一个编解码上下文
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(pCodecCtx, pCodecParameters) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't copy codec context");
        goto __FAIL;// Error copying codec context
    }

    // 打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open decoder!\n");
        goto __FAIL; // Could not open codec
    }

    // Allocate video frame
    pFrame = av_frame_alloc();

    w_width = pCodecCtx->width;
    w_height = pCodecCtx->height;

    //创建窗口
    win = SDL_CreateWindow("播放器",
                           SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED,
                           w_width, w_height,
                           SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window by SDL");
        goto __FAIL;
    }

    //创建渲染器
    renderer = SDL_CreateRenderer(win, -1, 0);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create Renderer by SDL");
        goto __FAIL;
    }

    pixformat = SDL_PIXELFORMAT_IYUV;//YUV格式
    // 创建纹理
    texture = SDL_CreateTexture(renderer,
                                pixformat,
                                SDL_TEXTUREACCESS_STREAMING,
                                w_width,
                                w_height);


    //读取数据
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStream) {
            //解码
            avcodec_send_packet(pCodecCtx, &packet);
            while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {

                SDL_UpdateYUVTexture(texture, NULL,
                                     pFrame->data[0], pFrame->linesize[0],
                                     pFrame->data[1], pFrame->linesize[1],
                                     pFrame->data[2], pFrame->linesize[2]);

                // Set Size of Window
                rect.x = 0;
                rect.y = 0;
                rect.w = pCodecCtx->width;
                rect.h = pCodecCtx->height;

                //展示
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_RenderPresent(renderer);
            }
        }

        av_packet_unref(&packet);

        // 事件处理
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                goto __QUIT;
            default:
                break;
        }


    }

    __QUIT:

    __FAIL:
    if (pFrame) {
        av_frame_free(&pFrame);
    }

    // Close the codec
    if (pCodecCtx) {
        avcodec_close(pCodecCtx);
    }

    if (pCodecParameters) {
        avcodec_parameters_free(&pCodecParameters);
    }

    // Close the video file
    if (pFormatCtx) {
        avformat_close_input(&pFormatCtx);
    }

    if (win) {
        SDL_DestroyWindow(win);
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

    if (texture) {
        SDL_DestroyTexture(texture);
    }

    SDL_Quit();
}

void clipVideo(char *in_filename, char *out_filename, double start_seconds, double end_seconds) {

    AVFormatContext *ifmt_ctx = NULL;
    AVFormatContext *ofmt_ctx = NULL;
    AVOutputFormat *ofmt = NULL;
    AVPacket pkt;
    int ret;

    avformat_open_input(&ifmt_ctx, in_filename, 0, 0);

//本质上调用了avformat_alloc_context、av_guess_format这两个函数，即创建了输出上下文，又根据输出文件后缀生成了最适合的输出容器
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    ofmt = ofmt_ctx->oformat;


    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            fprintf(stderr, "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            return;
        }
        avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
        out_stream->codecpar->codec_tag = 0;
    }
    avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
    // 写头信息
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
        return;
    }

//跳转到指定帧
    ret = av_seek_frame(ifmt_ctx, -1, start_seconds * AV_TIME_BASE, AVSEEK_FLAG_ANY);
    if (ret < 0) {
        fprintf(stderr, "Error seek\n");
        return;
    }

// 根据流数量申请空间，并全部初始化为0
    int64_t dts_start_from = (int64_t) malloc(sizeof(int64_t) * ifmt_ctx->nb_streams);
    memset(&dts_start_from, 0, sizeof(int64_t) * ifmt_ctx->nb_streams);

    int64_t pts_start_from = (int64_t) malloc(sizeof(int64_t) * ifmt_ctx->nb_streams);
//    int p = (int )malloc(len * sizeof(int))
    memset(&pts_start_from, 0, sizeof(int64_t) * ifmt_ctx->nb_streams);

    while (1) {
        AVStream *in_stream, *out_stream;

        //读取数据
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;

        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];

        // 时间超过要截取的时间，就退出循环
        if (av_q2d(in_stream->time_base) * pkt.pts > end_seconds) {
            av_packet_unref(&pkt);
            break;
        }
// 根据流数量申请空间，并全部初始化为0
        int64_t *dts_start_from = (int64_t *) malloc(sizeof(int64_t) * ifmt_ctx->nb_streams);
        memset(dts_start_from, 0, sizeof(int64_t) * ifmt_ctx->nb_streams);

        int64_t *pts_start_from = (int64_t *) malloc(sizeof(int64_t) * ifmt_ctx->nb_streams);
        memset(pts_start_from, 0, sizeof(int64_t) * ifmt_ctx->nb_streams);

        while (1) {
            AVStream *in_stream, *out_stream;

            //读取数据
            ret = av_read_frame(ifmt_ctx, &pkt);
            if (ret < 0)
                break;

            in_stream = ifmt_ctx->streams[pkt.stream_index];
            out_stream = ofmt_ctx->streams[pkt.stream_index];

            // 时间超过要截取的时间，就退出循环
            if (av_q2d(in_stream->time_base) * pkt.pts > end_seconds) {
                av_packet_unref(&pkt);
                break;
            }

            // 将截取后的每个流的起始dts 、pts保存下来，作为开始时间，用来做后面的时间基转换
            if (dts_start_from[pkt.stream_index] == 0) {
                dts_start_from[pkt.stream_index] = pkt.dts;
            }
            if (pts_start_from[pkt.stream_index] == 0) {
                pts_start_from[pkt.stream_index] = pkt.pts;
            }
            int64_t diff1 = pkt.pts - pts_start_from[pkt.stream_index];
            // 时间基转换
            pkt.pts = av_rescale_q_rnd(diff1, in_stream->time_base,
                                       out_stream->time_base, (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.dts = av_rescale_q_rnd(pkt.dts - dts_start_from[pkt.stream_index], in_stream->time_base,
                                       out_stream->time_base, (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

            if (pkt.pts < 0) {
                pkt.pts = 0;
            }
            if (pkt.dts < 0) {
                pkt.dts = 0;
            }

            pkt.duration = (int) av_rescale_q((int64_t) pkt.duration, in_stream->time_base, out_stream->time_base);
            pkt.pos = -1;

            //一帧视频播放时间必须在解码时间点之后，当出现pkt.pts < pkt.dts时会导致程序异常，所以我们丢掉有问题的帧，不会有太大影响。
            if (pkt.pts < pkt.dts) {
                continue;
            }

            ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
            if (ret < 0) {
                fprintf(stderr, "Error write packet\n");
                break;
            }

            av_packet_unref(&pkt);
        }

//释放资源
        free(dts_start_from);
        free(pts_start_from);

//写文件尾信息
        av_write_trailer(ofmt_ctx);
    }


}

void sdl2PlayerNormal(char *filename) {
    AVFormatContext *pFormatCtx;
    int videoStream, audioStream;

    //音频同步
    double audio_clock, frame_timer, frame_last_pts, frame_last_delay;
    //视频同步
    double video_clock, video_current_pts, video_current_pts_time;
    AVStream *audio_st;
    AVCodecContext *audio_ctx;
//    PacketQueue audioQ;
//    uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];

}


void audioPlayer(char *audioPath) {

    AVFormatContext *pFormatCtx = NULL; //for opening multi-media file

    int i, audioStream = -1;

    AVCodecParameters *pCodecParameters = NULL; //codec context
    AVCodecContext *pCodecCtx = NULL;

    AVCodec *pCodec = NULL; // the codecer
    AVFrame *pFrame = NULL;
    AVPacket *packet;
    uint8_t *out_buffer;

    int64_t in_channel_layout;
    struct SwrContext *au_convert_ctx;

    if (avformat_open_input(&pFormatCtx, audioPath, NULL, NULL) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open video file!");
        return; // Couldn't open file
    }

    audioStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

    if (audioStream == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Din't find a video stream!");
        return;// Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    pCodecParameters = pFormatCtx->streams[audioStream]->codecpar;

    // Find the decoder for the video stream
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unsupported codec!\n");
        return; // Codec not found
    }

    // Copy context
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(pCodecCtx, pCodecParameters) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't copy codec context");
        return;// Error copying codec context
    }

    // Open codec
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open decoder!\n");
        return; // Could not open codec
    }
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_init_packet(packet);
    pFrame = av_frame_alloc();

    //处理声音
    uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO; //输出声道
    int out_nb_samples = 1024;
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    int out_sample_rate = 441000;
    int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
    int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);
    out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE * 2);



    //Init
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf("Could not initialize SDL - %s\n", SDL_GetError());
        return;
    }

    SDL_AudioSpec spec;
    spec.freq = out_sample_rate;
    spec.format = AUDIO_S16SYS;
    spec.channels = out_channels;
    spec.silence = 0;
    spec.samples = out_nb_samples;
    spec.callback = read_audio_data;
    spec.userdata = pCodecCtx;
    if (SDL_OpenAudio(&spec, NULL) < 0) {
        printf("can't open audio.\n");
        return;
    }

    in_channel_layout = av_get_default_channel_layout(pCodecCtx->channels);
    printf("in_channel_layout --->%d\n", in_channel_layout);
    au_convert_ctx = swr_alloc();
    au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, out_channel_layout, out_sample_fmt, out_sample_rate,
                                        in_channel_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0, NULL);
    swr_init(au_convert_ctx);

    SDL_PauseAudio(0);

    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == audioStream) {
            avcodec_send_packet(pCodecCtx, packet);
            while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
                swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t **) pFrame->data,
                            pFrame->nb_samples); // 转换音频
            }

            audio_chunk = (Uint8 *) out_buffer;
            audio_len = out_buffer_size;
            audio_pos = audio_chunk;

            while (audio_len > 0) {
                SDL_Delay(1);//延迟播放
            }
        }
        av_packet_unref(packet);
    }
    swr_free(&au_convert_ctx);
    SDL_Quit();
}


void pushStream(char *in_filename, char *out_filename) {

    AVOutputFormat *ofmt = NULL;
    //Input AVFormatContext and Output AVFormatContext
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
//    const char *in_filename, *out_filename;
    int ret, i;
    int videoindex=-1;
    int frame_index=0;
    int64_t start_time=0;

//      = "dakongtou.mp4";//输入URL（Input file URL）
//    //in_filename  = "shanghai03_p.h264";
//
//     = "rtmp://localhost/test/livestream";//输出 URL（Output URL）[RTMP]

    av_register_all();
    //Network
    avformat_network_init();
    //Input
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
        printf( "Could not open input file.");
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        printf( "Failed to retrieve input stream information");
        goto end;
    }

    for(i=0; i<ifmt_ctx->nb_streams; i++)
        if(ifmt_ctx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
            videoindex=i;
            break;
        }

    av_dump_format(ifmt_ctx, 0, in_filename, 0);

    //Output

    avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", out_filename); //RTMP
    //avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", out_filename);//UDP

    if (!ofmt_ctx) {
        printf( "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    ofmt = ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
        if (!out_stream) {
            printf( "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        //Copy the settings of AVCodecContext
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
        if (ret < 0) {
            printf( "Failed to copy context from input to output stream codec context\n");
            goto end;
        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    //Dump Format------------------
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    //Open output URL
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            printf( "Could not open output URL '%s'", out_filename);
            goto end;
        }
    }
    //Write file header
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        printf( "Error occurred when opening output URL\n");
        goto end;
    }

    start_time=av_gettime();
    while (1) {
        AVStream *in_stream, *out_stream;
        //Get an AVPacket
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        //FIX：No PTS (Example: Raw H.264)
        //Simple Write PTS
        if(pkt.pts==AV_NOPTS_VALUE){
            //Write PTS
            AVRational time_base1=ifmt_ctx->streams[videoindex]->time_base;
            //Duration between 2 frames (us)
            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
            //Parameters
            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
            pkt.dts=pkt.pts;
            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
        }
        //Important:Delay
        if(pkt.stream_index==videoindex){
            AVRational time_base=ifmt_ctx->streams[videoindex]->time_base;
            AVRational time_base_q={1,AV_TIME_BASE};
            int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if (pts_time > now_time)
                av_usleep(pts_time - now_time);

        }

        in_stream  = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];
        /* copy packet */
        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        //Print to Screen
        if(pkt.stream_index==videoindex){
            printf("Send %8d video frames to output URL\n",frame_index);
            frame_index++;
        }
        //ret = av_write_frame(ofmt_ctx, &pkt);
        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

        if (ret < 0) {
            printf( "Error muxing packet\n");
            break;
        }

        av_free_packet(&pkt);

    }
    //Write file trailer
    av_write_trailer(ofmt_ctx);
    end:
    avformat_close_input(&ifmt_ctx);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        printf( "Error occurred.\n");
        return ;
    }
}

void pushCamera(){

    AVDictionary *options = NULL;
    AVCodec *pCodec;
    AVStream *videoStream = NULL;
    AVFormatContext *pContext;
    AVInputFormat *infmt;
    AVCodecContext *pCodecContext;
    int videoIndex = -1;

    //调用设备
    av_register_all();
    avformat_network_init();
    avdevice_register_all();
    pContext = avformat_alloc_context();
    infmt = av_find_input_format("avfoundation");

    av_dict_set(&options, "framerate", "30", 0);
    av_dict_set(&options, "video_size", "1280x720", 0);
    av_dict_set(&options, "pixel_format", "uyvy422", 0);
    int ret = avformat_open_input(&pContext, "0", infmt, &options);
    if (ret != 0) {
        printf("can't open camera \n");
        goto Last;
    }
    if (avformat_find_stream_info(pContext, NULL) < 0) {
        printf("can't find stream \n");
        goto Last;
    }
    for (int i = 0; i < pContext->nb_streams; i++) {
        if (pContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            videoStream = pContext->streams[i];
            break;
        }
    }
    if (videoIndex == -1) {
        printf("can't find video stream\n");
        goto Last;
    }
    pCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (pCodec == NULL) {
        printf("can't find decoder \n");
        goto Last;
    }
    pCodecContext = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecContext, videoStream->codecpar);
    if (avcodec_open2(pCodecContext, pCodec, NULL) != 0) {
        printf("can't open decoder\n");
        goto Last;
    }




    Last:
    {
        avformat_close_input(&pContext);
        return;
    }
}


