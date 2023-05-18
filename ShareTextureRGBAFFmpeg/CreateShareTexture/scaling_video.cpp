#include "scaling_video.h"

ScalingVideo::ScalingVideo(int srcW, int srcH, enum AVPixelFormat srcFormat,
    int dstW, int dstH, enum AVPixelFormat dstFormat):srcW(srcW),srcH(srcH),srcFormat(srcFormat),
        dstW(dstW),dstH(dstH),dstFormat(dstFormat){
    init();
}
ScalingVideo::~ScalingVideo() {
    uninit();
}
bool ScalingVideo::init() {
    bool isSuccess = true;
    sws_ctx = sws_getContext(srcW, srcH, srcFormat,
        dstW, dstH, dstFormat,
        SWS_BILINEAR, NULL, NULL, NULL);
    if (sws_ctx == NULL) {
        isSuccess = false;
    }
    int ret;
    /* buffer is going to be written to rawvideo file, no alignment */
    if ((ret = av_image_alloc(dst_data, dst_linesize,
        dstW, dstH, dstFormat, 1)) < 0) {
        fprintf(stderr, "Could not allocate destination image\n");
        isSuccess = false;
    }
    dst_bufsize = ret;
    return isSuccess;
}
void ScalingVideo::uninit() {
    av_freep(&dst_data[0]);
    sws_freeContext(sws_ctx);
}
int ScalingVideo::scaleVideo(const uint8_t* const srcSlice[],
    const int srcStride[], int srcSliceY, int srcSliceH) {
    int ret = sws_scale(sws_ctx, (const uint8_t* const*)srcSlice,
        srcStride, 0, srcSliceH, dst_data, dst_linesize);
    return ret;
}