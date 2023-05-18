extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>
}
#include <string>

class ScalingVideo {
public:
    ScalingVideo(int srcW, int srcH, enum AVPixelFormat srcFormat,
        int dstW, int dstH, enum AVPixelFormat dstFormat);
    virtual ~ScalingVideo();
    /* convert to destination format */
    int scaleVideo(const uint8_t* const srcSlice[],
        const int srcStride[], int srcSliceY, int srcSliceH);
private:
    bool init();
    void uninit();
public:
    uint8_t* dst_data[4];
    int dst_linesize[4];
    int dst_bufsize;
private:
    struct SwsContext* sws_ctx;
    int srcW;
    int srcH;
    enum AVPixelFormat srcFormat;
    int dstW;
    int dstH;
    enum AVPixelFormat dstFormat;


};