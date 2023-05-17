extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <string>

struct FrameData {
    uint8_t* dst_data[4];
    int dst_linesizes[4];
    enum AVPixelFormat pix_fmt;
    int width;
    int height;
};
struct VideoInfo {
    enum AVPixelFormat pix_fmt;
    int width;
    int height;
};
class DemuxingDecoding {
public:
    DemuxingDecoding(std::wstring filename);
    ~DemuxingDecoding();
    bool getNetxtFrame(FrameData &frameData);
    bool getVideoInfo(VideoInfo& videoInfo);
private:
    int output_video_frame(AVFrame* frame);
    int output_audio_frame(AVFrame* frame);
    int decode_packet(AVCodecContext* dec, const AVPacket* pkt);
    int decode_videoPacket(AVCodecContext* dec, const AVPacket* pkt, bool& bValidFrame);
    int open_codec_context(int* stream_idx,
        AVCodecContext** dec_ctx, AVFormatContext* fmt_ctx, enum AVMediaType type);
    int get_format_from_sample_fmt(const char** fmt,
        enum AVSampleFormat sample_fmt);
    void setFrameData(FrameData& frameData);
    int init();
    void uninit();

private:
    AVFormatContext* fmt_ctx;
    AVCodecContext* video_dec_ctx;
    AVCodecContext* audio_dec_ctx;
    int width;
    int height;
    enum AVPixelFormat pix_fmt;
    AVStream* video_stream;
    AVStream* audio_stream;
    const char* src_filename;
    const char* video_dst_filename;
    const char* audio_dst_filename;
    FILE* video_dst_file;
    FILE* audio_dst_file;

    uint8_t* video_dst_data[4];
    int   video_dst_linesize[4];
    int video_dst_bufsize;

    int video_stream_idx;
    int audio_stream_idx;
    AVFrame* frame;
    AVPacket* pkt;
    int video_frame_count;
    int audio_frame_count;

    std::wstring m_filename;

    bool m_isInitSuccess;
    bool isFinish_av_read_frame;
    bool isFinish_avcodec_receive_frame;
};