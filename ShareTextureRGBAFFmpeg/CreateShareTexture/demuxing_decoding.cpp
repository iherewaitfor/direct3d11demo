#include "demuxing_decoding.h"
#include <Windows.h>
//#include <stringapiset.h>
wchar_t* TransformUTF8ToUnicodeM(const char* _str)
{
    int textlen = 0;
    wchar_t* result = NULL;

    if (_str)
    {
        textlen = MultiByteToWideChar(CP_UTF8, 0, _str, -1, NULL, 0);
        result = (wchar_t*)malloc((textlen + 1) * sizeof(wchar_t));
        memset(result, 0, (textlen + 1) * sizeof(wchar_t));
        MultiByteToWideChar(CP_UTF8, 0, _str, -1, (LPWSTR)result, textlen);
    }

    return result;
}

char* TransformUnicodeToUTF8M(const wchar_t* _str)
{
    char* result = NULL;
    int textlen = 0;

    if (_str)
    {
        textlen = WideCharToMultiByte(CP_UTF8, 0, _str, -1, NULL, 0, NULL, NULL);
        result = (char*)malloc((textlen + 1) * sizeof(char));
        memset(result, 0, sizeof(char) * (textlen + 1));
        WideCharToMultiByte(CP_UTF8, 0, _str, -1, result, textlen, NULL, NULL);
    }

    return result;
}

DemuxingDecoding::DemuxingDecoding(std::wstring filename) {
    fmt_ctx = NULL;
    video_dec_ctx = NULL;
    audio_dec_ctx;
    width = 0;
    height = 0;
    video_stream = NULL;
    audio_stream = NULL;
    src_filename = NULL;
    video_dst_filename = NULL;
    audio_dst_filename = NULL;
    video_dst_file = NULL;
    audio_dst_file = NULL;
    video_dst_bufsize;

    video_stream_idx = -1;
    audio_stream_idx = -1;
    frame = NULL;
    pkt = NULL;
    video_frame_count = 0;
    audio_frame_count = 0;

    m_filename = filename;
    m_isInitSuccess = false;
    isFinish_av_read_frame = false;
    isFinish_avcodec_receive_frame = true;

    init();
}

DemuxingDecoding::~DemuxingDecoding() {
    uninit();
}



int DemuxingDecoding::output_video_frame(AVFrame *frame)
{
    if (frame->width != width || frame->height != height ||
        frame->format != pix_fmt) {
        /* To handle this change, one could call av_image_alloc again and
         * decode the following frames into another rawvideo file. */
        fprintf(stderr, "Error: Width, height and pixel format have to be "
                "constant in a rawvideo file, but the width, height or "
                "pixel format of the input video changed:\n"
                "old: width = %d, height = %d, format = %s\n"
                "new: width = %d, height = %d, format = %s\n",
                width, height, av_get_pix_fmt_name(pix_fmt),
                frame->width, frame->height,
                av_get_pix_fmt_name((enum AVPixelFormat)frame->format));
        return -1;
    }

    printf("video_frame n:%d coded_n:%d\n",
           video_frame_count++, frame->coded_picture_number);

    /* copy decoded frame to destination buffer:
     * this is required since rawvideo expects non aligned data */
    av_image_copy(video_dst_data, video_dst_linesize,
                  (const uint8_t **)(frame->data), frame->linesize,
                  pix_fmt, width, height);

    /* write to rawvideo file */
    fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
    return 0;
}

int DemuxingDecoding::output_audio_frame(AVFrame *frame)
{
    size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample((enum AVSampleFormat)frame->format);
    //printf("audio_frame n:%d nb_samples:%d pts:%s\n",
    //       audio_frame_count++, frame->nb_samples,
    //       av_ts2timestr(frame->pts, &audio_dec_ctx->time_base));

    /* Write the raw audio data samples of the first plane. This works
     * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
     * most audio decoders output planar audio, which uses a separate
     * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
     * In other words, this code will write only the first audio channel
     * in these cases.
     * You should use libswresample or libavfilter to convert the frame
     * to packed data. */
    fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);

    return 0;
}

int DemuxingDecoding::decode_packet(AVCodecContext *dec, const AVPacket *pkt)
{
    int ret = 0;

    // submit the packet to the decoder
    ret = avcodec_send_packet(dec, pkt);
    if (ret < 0) {
        //fprintf(stderr, "Error submitting a packet for decoding (%s)\n", av_err2str(ret));
        return ret;
    }

    // get all the available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec, frame);
        if (ret < 0) {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;

            //fprintf(stderr, "Error during decoding (%s)\n", av_err2str(ret));
            return ret;
        }

        // write the frame data to output file
        if (dec->codec->type == AVMEDIA_TYPE_VIDEO) {
            ret = output_video_frame(frame);
        }
        else
            ret = output_audio_frame(frame);

        av_frame_unref(frame);
        if (ret < 0)
            return ret;
    }

    return 0;
}

int DemuxingDecoding::decode_videoPacket(AVCodecContext* dec, const AVPacket* pkt, bool& bValidFrame)
{
    int ret = 0;
    bValidFrame = false;

    if (isFinish_avcodec_receive_frame) {
        // submit the packet to the decoder
        ret = avcodec_send_packet(dec, pkt);
        if (ret < 0) {
            //fprintf(stderr, "Error submitting a packet for decoding (%s)\n", av_err2str(ret));
            return ret;
        }
    }

    // get all the available frames from the decoder
    while (ret >= 0 || !isFinish_avcodec_receive_frame) {
        ret = avcodec_receive_frame(dec, frame);
        if (ret < 0) {
            isFinish_avcodec_receive_frame = true;
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;

            //fprintf(stderr, "Error during decoding (%s)\n", av_err2str(ret));
            return ret;
        }
        // write the frame data to output file
        if (frame->width != width || frame->height != height ||
            frame->format != pix_fmt) {
            /* To handle this change, one could call av_image_alloc again and
             * decode the following frames into another rawvideo file. */
            fprintf(stderr, "Error: Width, height and pixel format have to be "
                "constant in a rawvideo file, but the width, height or "
                "pixel format of the input video changed:\n"
                "old: width = %d, height = %d, format = %s\n"
                "new: width = %d, height = %d, format = %s\n",
                width, height, av_get_pix_fmt_name(pix_fmt),
                frame->width, frame->height,
                av_get_pix_fmt_name((enum AVPixelFormat)frame->format));
            isFinish_avcodec_receive_frame = true;
            return -1;
        }

        printf("video_frame n:%d coded_n:%d\n",
            video_frame_count++, frame->coded_picture_number);

        /* copy decoded frame to destination buffer:
         * this is required since rawvideo expects non aligned data */
        av_image_copy(video_dst_data, video_dst_linesize,
            (const uint8_t**)(frame->data), frame->linesize,
            pix_fmt, width, height);

        /* write to rawvideo file */
        fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
        
        ret = 0;
        isFinish_avcodec_receive_frame = false;
        bValidFrame = true;
        av_frame_unref(frame);
        return 0;
    }
    return 0;
}

int DemuxingDecoding::open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    const AVCodec *dec = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), src_filename);
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders */
        if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }

    return 0;
}

int DemuxingDecoding::get_format_from_sample_fmt(const char **fmt,
                                      enum AVSampleFormat sample_fmt)
{
    int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
        { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
        { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
        { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
        { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
        { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
    };
    *fmt = NULL;

    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }

    fprintf(stderr,
            "sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
    return -1;
}

int DemuxingDecoding::init()
{
    int ret = 0;
    //src_filename = "guilinvideo.mp4";
    video_dst_filename = "guilinvideo.yuv";
    audio_dst_filename = "audiodest.mp3";
    src_filename = TransformUnicodeToUTF8M(m_filename.c_str());
    do {
        /* open input file, and allocate format context */
        if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
            fprintf(stderr, "Could not open source file %s\n", src_filename);
            return 1;
        }
        /* retrieve stream information */
        if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
            fprintf(stderr, "Could not find stream information\n");
            return 1;
        }
        if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
            video_stream = fmt_ctx->streams[video_stream_idx];

            video_dst_file = fopen(video_dst_filename, "wb");
            if (!video_dst_file) {
                fprintf(stderr, "Could not open destination file %s\n", video_dst_filename);
                ret = 1;
                break;
            }

            /* allocate image where the decoded image will be put */
            width = video_dec_ctx->width;
            height = video_dec_ctx->height;
            pix_fmt = video_dec_ctx->pix_fmt;
            ret = av_image_alloc(video_dst_data, video_dst_linesize,
                width, height, pix_fmt, 1);
            if (ret < 0) {
                fprintf(stderr, "Could not allocate raw video buffer\n");
                break;
            }
            ret = 0;
            video_dst_bufsize = ret;
        }
        if (open_codec_context(&audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
            audio_stream = fmt_ctx->streams[audio_stream_idx];
            audio_dst_file = fopen(audio_dst_filename, "wb");
            if (!audio_dst_file) {
                fprintf(stderr, "Could not open destination file %s\n", audio_dst_filename);
                ret = 1;
                break;
            }
        }
        /* dump input information to stderr */
        av_dump_format(fmt_ctx, 0, src_filename, 0);
        if (!audio_stream && !video_stream) {
            fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
            ret = 1;
            break;
        }
        frame = av_frame_alloc();
        if (!frame) {
            fprintf(stderr, "Could not allocate frame\n");
            ret = AVERROR(ENOMEM);
            break;
        }
        pkt = av_packet_alloc();
        if (!pkt) {
            fprintf(stderr, "Could not allocate packet\n");
            ret = AVERROR(ENOMEM);
            break;
        }
        if (video_stream)
            printf("Demuxing video from file '%s' into '%s'\n", src_filename, video_dst_filename);
        if (audio_stream)
            printf("Demuxing audio from file '%s' into '%s'\n", src_filename, audio_dst_filename);
    } while (false);

    m_isInitSuccess = (ret == 0);
    return ret;
}

bool DemuxingDecoding::getVideoInfo(VideoInfo& videoInfo) {
    if (m_isInitSuccess) {
        videoInfo.pix_fmt = pix_fmt;
        videoInfo.width = width;
        videoInfo.height = height;
        return true;
    }
    return false;
}
void DemuxingDecoding::setFrameData(FrameData& frameData) {
    for (int i = 0; i < 3; i++) {
        frameData.dst_data[i] = video_dst_data[i];
        frameData.dst_linesizes[i] = video_dst_linesize[i];
    }
    frameData.pix_fmt = pix_fmt;
    frameData.width = width;
    frameData.height = height;
}
bool DemuxingDecoding::getNetxtFrame(FrameData& frameData) {

    int ret = 0;
    if (!isFinish_avcodec_receive_frame) {
        bool isValidFrame = true;
        ret = decode_videoPacket(video_dec_ctx, pkt, isValidFrame);
        if (isValidFrame) {
            setFrameData(frameData);
            return true;
        }
    }
    if (!isFinish_av_read_frame) {
        /* read frames from the file */
        while (av_read_frame(fmt_ctx, pkt) >= 0) {
            // check if the packet belongs to a stream we are interested in, otherwise
            // skip it

            bool bVideoStream = false;
            bool isValidFrame = false;
            if (pkt->stream_index == video_stream_idx) {
                bVideoStream = true;
                ret = decode_videoPacket(video_dec_ctx, pkt, isValidFrame);

            }
            else if (pkt->stream_index == audio_stream_idx) {
                ret = decode_packet(audio_dec_ctx, pkt);
            }
            av_packet_unref(pkt);
            if(bVideoStream && isValidFrame) {
                setFrameData(frameData);
                return true;
            }
            if (ret < 0)
                break;
        }
    }

    isFinish_av_read_frame = true;
    /* flush the decoders */
    if (video_dec_ctx) {
        bool isValidFrame = false;
        decode_videoPacket(video_dec_ctx, NULL, isValidFrame);
        if (isValidFrame) {
            setFrameData(frameData);
            return true;
        }
    }
    if (audio_dec_ctx)
        decode_packet(audio_dec_ctx, NULL);
    return false;
}

void DemuxingDecoding::uninit() {
    avcodec_free_context(&video_dec_ctx);
    avcodec_free_context(&audio_dec_ctx);
    avformat_close_input(&fmt_ctx);
    if (video_dst_file)
        fclose(video_dst_file);
    if (audio_dst_file)
        fclose(audio_dst_file);
    av_packet_free(&pkt);
    av_frame_free(&frame);
    av_free(video_dst_data[0]);
}
