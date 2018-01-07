#include <iostream>

#include <curses.h>
#include<iostream>
#include<cstdio>
#include<cstring>
#include<algorithm>
#include<queue>
#include<vector>
#include<cmath>
#include<string>
#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<algorithm>

#include <stdlib.h>
#include <stdio.h>

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>

#include <libavformat/avio.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "SDL2/SDL.h"

#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>


#include <inttypes.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/timestamp.h>
}


using namespace std;

#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>


int decodeVideo(const char *in_filename, const char *out_filename, double startTime, double endTime);

int mixVideo(const char *in_filename_v, const char *in_filename_a);

int contactVideo(const char *in_filename1, const char *in_filename2, const char *out_filename);


double get_rotation(AVStream *st);


int main() {
//    decodeVideoWithResample("test.mp4", "test22.mp4");
//    return 0;
//    decodeVideo("test4.mp4", "test22.mp4", 11.001, 17.2);
//    mixVideo("test22.mp4", "man.mp3");
    contactVideo("test.mp4", "test.mp4", "test33.mp4");
}


#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>


double get_rotation(AVStream *st) {
    AVDictionaryEntry *rotate_tag = av_dict_get(st->metadata, "rotate", NULL, 0);
    double theta = 0;

    if (rotate_tag && *rotate_tag->value && strcmp(rotate_tag->value, "0")) {
        //char *tail;
        //theta = av_strtod(rotate_tag->value, &tail);
        theta = atof(rotate_tag->value);
        // if (*tail)
        // theta = 0;"
    }

    theta -= 360 * floor(theta / 360 + 0.9 / 360);

    if (fabs(theta - 90 * round(theta / 90)) > 2)
        printf("Odd rotation angle");

    printf("get_rotation %f\n", theta);

    return theta;
}


static double r2d(AVRational avRational) {
    return avRational.num == 0 ? 0 : (double) avRational.num / (double) avRational.den;
}

extern "C"
{
int decodeVideo1(const char *in_filename, const char *out_filename, double startTime, double endTime) {
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;

    AVPacket pkt;
    int ret, i;
    int stream_index = 0;
    int *stream_mapping = new int[2];
    int stream_mapping_size = 0;

    av_register_all();

    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
        fprintf(stderr, "Could not open input file '%s'", in_filename);
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
        goto end;
    }

    av_dump_format(ifmt_ctx, 0, in_filename, 0);

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (!ofmt_ctx) {
        fprintf(stderr, "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    stream_mapping_size = ifmt_ctx->nb_streams;
//    printf("stream_mapping_size:%d;value:%d,\n",stream_mapping_size,sizeof(*stream_mapping));
//    av_malloc(stream_mapping_size * sizeof(*stream_mapping));


    if (!stream_mapping) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    ofmt = ofmt_ctx->oformat;

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *out_stream;
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapping[i] = -1;
            continue;
        }

        if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            get_rotation(in_stream);
        }
        stream_mapping[i] = stream_index++;

        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            fprintf(stderr, "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
        if (ret < 0) {
            fprintf(stderr, "Failed to copy codec parameters\n");
            goto end;
        }
        out_stream->codecpar->codec_tag = 0;
    }

    av_dump_format(ofmt_ctx, 0, out_filename, 1);

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open output file '%s'", out_filename);
            goto end;
        }
    }

    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
        goto end;
    }


    while (1) {
        AVStream *in_stream, *out_stream;
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        in_stream = ifmt_ctx->streams[pkt.stream_index];

        if (pkt.stream_index >= stream_mapping_size ||
            stream_mapping[pkt.stream_index] < 0) {
            av_packet_unref(&pkt);
            continue;
        }

        pkt.stream_index = stream_mapping[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];

        AVRational *time_base = &ifmt_ctx->streams[pkt.stream_index]->time_base;
        double currentTime = pkt.pts * (double) (time_base->num) / ((double) time_base->den);


        if (ofmt_ctx->streams[pkt.stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            ret = av_dict_set(&out_stream->metadata, "rotate", "90", 0); //设置旋转角度
//            log_packet(ifmt_ctx, &pkt, "in");
//            AVRational *time_base = &ifmt_ctx->streams[pkt.stream_index]->time_base;
//            double currentTime = pkt.pts * (double) (time_base->num) / ((double) time_base->den);

        }

        if (currentTime >= startTime && currentTime <= endTime) {
            /* copy packet */
            pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                       (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                       (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
            pkt.pos = -1;
//            log_packet(ofmt_ctx, &pkt, "out");

            ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
            if (ret < 0) {
                fprintf(stderr, "Error muxing packet\n");
                break;
            }
            av_packet_unref(&pkt);
        }
    }
    av_write_trailer(ofmt_ctx);
    end:

    avformat_close_input(&ifmt_ctx);

/* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    av_freep(&stream_mapping);

    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        return 1;
    }

    return 0;
}
}


int decodeVideo(const char *in_filename, const char *out_filename, double startTime, double endTime) {
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;

    AVPacket pkt;
    int ret, i;
    int stream_index = 0;
    int *stream_mapping = new int[2];
    int stream_mapping_size = 0;
    double angle = 0;
    char buf[100];

    av_register_all();

    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
        fprintf(stderr, "Could not open input file '%s'", in_filename);
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
        goto end;
    }

    av_dump_format(ifmt_ctx, 0, in_filename, 0);

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (!ofmt_ctx) {
        fprintf(stderr, "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    stream_mapping_size = ifmt_ctx->nb_streams;
//    printf("stream_mapping_size:%d;value:%d,\n",stream_mapping_size,sizeof(*stream_mapping));
//    av_malloc(stream_mapping_size * sizeof(*stream_mapping));


    if (!stream_mapping) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    ofmt = ofmt_ctx->oformat;

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *out_stream;
        AVStream *in_stream = ifmt_ctx->streams[i];
//        AVCodecParameters *in_codecpar = in_stream->codecpar;
        AVCodecContext *in_codecpar = in_stream->codec;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapping[i] = -1;
            continue;
        }

        if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            angle = get_rotation(in_stream);
            sprintf(buf, "%lf", angle);
        }
        stream_mapping[i] = stream_index++;

        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            fprintf(stderr, "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

//        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
        ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
        if (ret < 0) {
            fprintf(stderr, "Failed to copy codec parameters\n");
            goto end;
        }
        out_stream->codec->codec_tag = 0;
    }

    av_dump_format(ofmt_ctx, 0, out_filename, 1);

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open output file '%s'", out_filename);
            goto end;
        }
    }

    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
        goto end;
    }


    while (1) {
        AVStream *in_stream, *out_stream;
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        in_stream = ifmt_ctx->streams[pkt.stream_index];

        if (pkt.stream_index >= stream_mapping_size ||
            stream_mapping[pkt.stream_index] < 0) {
            av_packet_unref(&pkt);
            continue;
        }

        pkt.stream_index = stream_mapping[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];

        AVRational *time_base = &ifmt_ctx->streams[pkt.stream_index]->time_base;
        double currentTime = pkt.pts * (double) (time_base->num) / ((double) time_base->den);


        if (ofmt_ctx->streams[pkt.stream_index]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {

            ret = av_dict_set(&out_stream->metadata, "rotate", buf, 0); //设置旋转角度
//            log_packet(ifmt_ctx, &pkt, "in");
//            AVRational *time_base = &ifmt_ctx->streams[pkt.stream_index]->time_base;
//            double currentTime = pkt.pts * (double) (time_base->num) / ((double) time_base->den);

        }

        if (currentTime >= startTime && currentTime <= endTime) {
/* copy packet */
            pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                       (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                       (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
            pkt.pos = -1;
//            log_packet(ofmt_ctx, &pkt, "out");

            ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
            if (ret < 0) {
                fprintf(stderr, "Error muxing packet\n");
                break;
            }
            av_packet_unref(&pkt);
        }
    }
    av_write_trailer(ofmt_ctx);
    end:

    avformat_close_input(&ifmt_ctx);

/* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    av_freep(&stream_mapping);

    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        return 1;
    }

    return 0;
}


int mixVideo(const char *in_filename_v, const char *in_filename_a) {

    AVOutputFormat *ofmt = NULL;
//Input AVFormatContext and Output AVFormatContext
    AVFormatContext *ifmt_ctx_v = NULL, *ifmt_ctx_a = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
    int ret, i;
    int videoindex_v = -1, videoindex_out = -1;
    int audioindex_a = -1, audioindex_out = -1;
    int frame_index = 0;
    int64_t cur_pts_v = 0, cur_pts_a = 0;
    double angle = 0;
    char buf[100];
    const char *out_filename = "mixVideo.mp4";//Output file URL
    av_register_all();
//Input
    if ((ret = avformat_open_input(&ifmt_ctx_v, in_filename_v, 0, 0)) < 0) {//打开输入的视频文件
        printf("Could not open input file.");
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx_v, 0)) < 0) {//获取视频文件信息
        printf("Failed to retrieve input stream information");
        goto end;
    }

    if ((ret = avformat_open_input(&ifmt_ctx_a, in_filename_a, 0, 0)) < 0) {//打开输入的音频文件
        printf("Could not open input file.");
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0) {//获取音频文件信息
        printf("Failed to retrieve input stream information");
        goto end;
    }
    printf("===========Input Information==========\n");
    av_dump_format(ifmt_ctx_v, 0, in_filename_v, 0);
    av_dump_format(ifmt_ctx_a, 0, in_filename_a, 0);
    printf("======================================\n");
//Output

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);//初始化输出码流的AVFormatContext。
    if (!ofmt_ctx) {
        printf("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        return -1;
    }
    ofmt = ofmt_ctx->oformat;

//从输入的AVStream中获取一个输出的out_stream
    for (i = 0; i < ifmt_ctx_v->nb_streams; i++) {
//Create output AVStream according to input AVStream
        if (ifmt_ctx_v->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVStream *in_stream = ifmt_ctx_v->streams[i];
            AVStream *out_stream = avformat_new_stream(ofmt_ctx,
                                                       in_stream->codec->codec);//创建流通道AVStream
            videoindex_v = i;
            angle = get_rotation(in_stream);
            sprintf(buf, "%lf", angle);

            videoindex_out = out_stream->index;
//Copy the settings of AVCodecContext
            if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
                printf("Failed to copy context from input to output stream codec context\n");
                break;
            }
            out_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
            break;
        }
    }

    for (i = 0; i < ifmt_ctx_a->nb_streams; i++) {
//Create output AVStream according to input AVStream
        if (ifmt_ctx_a->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            AVStream *in_stream = ifmt_ctx_a->streams[i];
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
            audioindex_a = i;
            if (!out_stream) {
                printf("Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                goto end;
            }
            audioindex_out = out_stream->index;
//Copy the settings of AVCodecContext
            if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
                printf("Failed to copy context from input to output stream codec context\n");
                goto end;
            }
            out_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

            break;
        }
    }

    printf("==========Output Information==========\n");
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    printf("======================================\n");

//Open output file
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0) {//打开输出文件。
            printf("Could not open output file '%s'", out_filename);
            return -1;
        }
    }
//Write file header
    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        printf("Error occurred when opening output file\n");
        return -1;
    }


//FIX
#if USE_H264BSF
    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
    AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
#endif

    while (1) {
        AVFormatContext *ifmt_ctx;
        int stream_index = 0;
        AVStream *in_stream, *out_stream;

//Get an AVPacket .   av_compare_ts是比较时间戳用的。通过该函数可以决定该写入视频还是音频。
        if (av_compare_ts(cur_pts_v, ifmt_ctx_v->streams[videoindex_v]->time_base, cur_pts_a,
                          ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0) {
            ifmt_ctx = ifmt_ctx_v;
            stream_index = videoindex_out;

            if (av_read_frame(ifmt_ctx, &pkt) >= 0) {
                do {
                    in_stream = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if (pkt.stream_index == videoindex_v) {
//FIX：No PTS (Example: Raw H.264) H.264裸流没有PTS，因此必须手动写入PTS
                        ret = av_dict_set(&out_stream->metadata, "rotate", buf, 0);

                        cur_pts_v = pkt.pts;
                        break;
                    }
                } while (av_read_frame(ifmt_ctx, &pkt) >= 0);
            } else {
                break;
            }
        } else {
            ifmt_ctx = ifmt_ctx_a;
            stream_index = audioindex_out;
            if (av_read_frame(ifmt_ctx, &pkt) >= 0) {
                do {
                    in_stream = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if (pkt.stream_index == audioindex_a) {
                        cur_pts_a = pkt.pts;

                        break;
                    }
                } while (av_read_frame(ifmt_ctx, &pkt) >= 0);
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



//Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index = stream_index;
        printf("Write 1 Packet. size:%5d\tpts:%lld\n", pkt.size, pkt.pts);
//Write AVPacket 音频或视频裸流
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
            printf("Error muxing packet\n");
            break;
        }
        av_free_packet(&pkt);

    }
//Write file trailer
    av_write_trailer(ofmt_ctx);

#if USE_H264BSF
    av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
    av_bitstream_filter_close(aacbsfc);
#endif

    end:
    avformat_close_input(&ifmt_ctx_v);
    avformat_close_input(&ifmt_ctx_a);
/* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        printf("Error occurred.\n");
        return -1;
    }
    return 0;
}


int contactVideo(const char *in_filename1, const char *in_filename2, const char *out_filename) {


    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt1_ctx = NULL, *ofmt_ctx = NULL;
    AVFormatContext *ifmt2_ctx = NULL;
    int ret, i;

    size_t curPts, curDts, curPts1, curDts1;
    double angle = 0;
    char buf[100];

    av_register_all();

    if ((ret = avformat_open_input(&ifmt1_ctx, in_filename1, 0, 0)) < 0) {
        fprintf(stderr, "Could not open input file '%s'", in_filename1);
    }

    if ((ret = avformat_find_stream_info(ifmt1_ctx, 0)) < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
    }

    av_dump_format(ifmt1_ctx, 0, in_filename1, 0);


    if ((ret = avformat_open_input(&ifmt2_ctx, in_filename2, 0, 0)) < 0) {
        fprintf(stderr, "Could not open input file '%s'", in_filename2);
    }

    if ((ret = avformat_find_stream_info(ifmt2_ctx, 0)) < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
    }

    av_dump_format(ifmt2_ctx, 0, in_filename2, 0);


    if ((ret = avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename)) < 0) {
        printf("can not alloc context for output!\n");
        return ret;
    }

    ofmt = ofmt_ctx->oformat;
    AVStream *out_video_stream;
    AVStream *out_audio_stream;
    int video_stream_index, audio_stream_index;

    for (i = 0; i < ifmt1_ctx->nb_streams; i++) {
        AVStream *in_stream = ifmt1_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;


        if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            angle = get_rotation(in_stream);
            sprintf(buf, "%lf", angle);


            video_stream_index = i;
            out_video_stream = avformat_new_stream(ofmt_ctx, NULL);
            if (!out_video_stream) {
                printf("Failed allocating output1 video stream\n");
                ret = AVERROR_UNKNOWN;
                return ret;
            }
            if ((ret = avcodec_copy_context(out_video_stream->codec, ifmt1_ctx->streams[i]->codec)) < 0) {
                printf("can not copy the video codec context!\n");
                return ret;
            }
            out_video_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
                out_video_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
            }


        } else if (in_codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {

            audio_stream_index = i;
            out_audio_stream = avformat_new_stream(ofmt_ctx, NULL);

            if (!out_audio_stream) {
                printf("Failed allocating output1 video stream\n");
                ret = AVERROR_UNKNOWN;
                return ret;
            }
            if ((ret = avcodec_copy_context(out_audio_stream->codec, ifmt1_ctx->streams[i]->codec)) < 0) {
                printf("can not copy the video codec context!\n");
                return ret;
            }
            out_audio_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
                out_audio_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
            }

        }
    }


//        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
//        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);


    av_dump_format(ofmt_ctx, 0, out_filename, 1);

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open output file '%s'", out_filename);
        }
    }


    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
    }


    AVFormatContext *input_ctx = ifmt1_ctx;
    AVPacket pkt;
    int pts_v, pts_a, dts_v, dts_a;

    while (1) {
        if (0 > av_read_frame(input_ctx, &pkt)) {
            if (input_ctx == ifmt1_ctx) {
                float vedioDuraTime, audioDuraTime;

                //calc the first media dura time
                vedioDuraTime = ((float) input_ctx->streams[video_stream_index]->time_base.num /
                                 (float) input_ctx->streams[video_stream_index]->time_base.den) * ((float) pts_v);
                audioDuraTime = ((float) input_ctx->streams[audio_stream_index]->time_base.num /
                                 (float) input_ctx->streams[audio_stream_index]->time_base.den) * ((float) pts_a);

                //calc the pts and dts end of the first media
                if (audioDuraTime > vedioDuraTime) {
                    dts_v = pts_v = audioDuraTime / ((float) input_ctx->streams[video_stream_index]->time_base.num /
                                                     (float) input_ctx->streams[video_stream_index]->time_base.den);
                    dts_a++;
                    pts_a++;
                } else {
                    dts_a = pts_a = vedioDuraTime / ((float) input_ctx->streams[audio_stream_index]->time_base.num /
                                                     (float) input_ctx->streams[audio_stream_index]->time_base.den);
                    dts_v++;
                    pts_v++;
                }
                input_ctx = ifmt2_ctx;
                continue;
            }
            break;
        }
        if (pkt.stream_index == video_stream_index) {
            if (input_ctx == ifmt2_ctx) {
                pkt.pts += pts_v;
                pkt.dts += dts_v;
            } else {
                pts_v = pkt.pts;
                dts_v = pkt.dts;
            }
        } else if (pkt.stream_index == audio_stream_index) {
            if (input_ctx == ifmt2_ctx) {
                pkt.pts += pts_a;
                pkt.dts += dts_a;
            } else {
                pts_a = pkt.pts;
                dts_a = pkt.dts;
            }
        }

        pkt.pts = av_rescale_q_rnd(pkt.pts, input_ctx->streams[pkt.stream_index]->time_base,
                                   ofmt_ctx->streams[pkt.stream_index]->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, input_ctx->streams[pkt.stream_index]->time_base,
                                   ofmt_ctx->streams[pkt.stream_index]->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.pos = -1;

        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
            printf("Error muxing packet\n");
            //break;
        }
        av_free_packet(&pkt);
    }


    av_write_trailer(ofmt_ctx);
    end:
/* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    avformat_close_input(&ifmt1_ctx);
    avformat_close_input(&ifmt2_ctx);

    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        return 1;
    }

    return 0;
}




