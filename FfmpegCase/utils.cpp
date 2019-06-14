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

    //打开输入的音频文件
    if ((ret = avformat_open_input(&pAudioInFmtCxt, mp3Path, NULL, NULL)) < 0) {
        printf("Could not open input audio file.");
        return;
    }
    //获取音频文件信息
    if ((ret = avformat_find_stream_info(pAudioInFmtCxt, NULL)) < 0) {
        printf("Failed to retrieve input stream information");
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


