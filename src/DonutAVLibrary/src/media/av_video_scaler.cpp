#include "av_video_scaler.h"

#include <iostream>
extern"C"
{
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
}

#include "av_data_tools.h"

using namespace std;

static void PrintError(int err)
{
    char buffer[1024] = { 0 };
    av_strerror(err, buffer, sizeof(buffer));
    cout << buffer << endl;
}

namespace Donut
{
    AVVideoScaler::AVVideoScaler()
    {
        input_linesize_  = new int[AV_NUM_DATA_POINTERS];
        output_linesize_ = new int[AV_NUM_DATA_POINTERS];
    }
    AVVideoScaler::~AVVideoScaler()
    {
        delete[] input_linesize_;
        delete[] output_linesize_;
        input_linesize_ = nullptr;
        output_linesize_ = nullptr;
    }

    int AVVideoScaler::initScaler(int in_fmt, int out_fmt)
    {
        //unique_lock<mutex> lock(mtx_);
        //if (sws_ctx_)
        //{
        //    sws_freeContext(sws_ctx_);
        //    sws_ctx_ = nullptr;
        //}
        if (output_width_ < 0 || output_height_ < 0 || input_width_ < 0 || input_height_ < 0)
        {
            //LOGERROR("(out_Width_ < 0 || out_Height_ < 0 || input_width_ < 0 || input_height_ < 0)");
            return false;
        }

        sws_ctx_ = sws_getCachedContext(
            sws_ctx_,
            input_width_, input_height_,
            (AVPixelFormat)in_fmt,
            output_width_, output_height_,
            (AVPixelFormat)out_fmt,
            SWS_BILINEAR,
            0, 0, 0
        );

        if (!sws_ctx_)
        {
            //LOGERROR("sws_getCachedContext failed");
            return false;
        }
        return 0;
    }

    int AVVideoScaler::initScaler(int input_width, int input_height, int input_fmt, int output_width, int output_height, int output_fmt)
    {
        //unique_lock<mutex> lock(mtx_);
        //if (sws_ctx_)
        //{
        //    sws_freeContext(sws_ctx_);
        //    sws_ctx_ = nullptr;
        //}
        input_width_ = input_width;
        input_height_ = input_height;
        input_fmt_ = input_fmt;
        
        output_width_ = output_width;
        output_height_ = output_height;
        output_fmt_ = output_fmt;
        
        if (output_width_ < 0 || output_height_ < 0 || input_width_ < 0 || input_height_ < 0)
        {
            //LOGERROR("(out_Width_ < 0 || out_Height_ < 0 || input_width_ < 0 || input_height_ < 0)");
            return false;
        }

        sws_ctx_ = sws_getCachedContext(
            sws_ctx_,
            input_width_, input_height_,
            (AVPixelFormat)input_fmt_,
            output_width_, output_height_,
            (AVPixelFormat)output_fmt_,
            SWS_BILINEAR,
            0, 0, 0
        );

        if (!sws_ctx_)
        {
            //LOGERROR("sws_getCachedContext failed");
            return false;
        }
        return 0;
    }



    void AVVideoScaler::setInputParam(int input_width, int input_height, int input_fmt)
    {
        input_width_ = input_width;
        input_height_ = input_height;
        input_fmt_ = input_fmt;
    }

    void AVVideoScaler::setoutputParam(int output_width, int output_height, int output_fmt)
    {
        output_width_ = output_width;
        output_height_ = output_height;
        output_fmt_ = output_fmt;
    }

    int AVVideoScaler::getScaledFrame(AVFrame* input, AVFrame* output)
    {
        //unique_lock<mutex> lock(mtx_);
        if (!sws_ctx_)
        {
            //LOGERROR("sws_ctx_ is null , frame scale failed");
            return -1;
        }

        if (input_width_ < 0 || input_height_ < 0 || output_width_ < 0 || output_height_ < 0)
        {
            //LOGERROR("input_width_ < 0 || input_height_ < 0 || output_width_ < 0 || output_height_ < 0)");
            return -1;
        }
        int ret = sws_scale(sws_ctx_, input->data, input->linesize, 0, input_height_, output->data, output->linesize);
        if (ret < 0)
        {
            PrintError(ret);
            return -1;
        }

        return 0;
    }

    int AVVideoScaler::scaleRawData(const uint8_t* const * src, const int* src_linesize, uint8_t* const* dst, const int* dst_linesize)
    {
        if (!sws_ctx_ || !src || src_linesize[0] <= 0 || dst_linesize[0] <= 0)
            return -1;

        int ret = sws_scale(sws_ctx_, src, src_linesize, 0, input_height_, dst, dst_linesize);
        if (ret <= 0)
        {
            PrintError(ret);
            return ret;
        }
        return ret;
    }

    AVFrame* AVVideoScaler::getScaledFrame(void* data, int* linesize)
    {
        //unique_lock<mutex> lock(mtx_);
        if (!sws_ctx_ || !data || linesize[0] < 0)return nullptr;

        uint8_t* inData[AV_NUM_DATA_POINTERS] = { 0 };
        inData[0] = (uint8_t*)data;
        //uint8_t* inData = (uint8_t*)data;

        AVFrame* outFrame = av_frame_alloc();
        outFrame->width = output_width_;
        outFrame->height = output_height_;
        outFrame->format = AV_PIX_FMT_YUV420P;
        outFrame->pts = 0;

        int ret = av_frame_get_buffer(outFrame, 0);
        if (ret != 0)
        {
            av_frame_free(&outFrame);
            PrintError(ret);
            return nullptr;
        }

        int src_line_size = (input_width_ * 4);
        ret = sws_scale(sws_ctx_, inData, &src_line_size, 0, input_height_, outFrame->data, outFrame->linesize);
        if (ret <= 0)
        {
            av_frame_free(&outFrame);
            PrintError(ret);
            return nullptr;
        }
        return outFrame;
    }

}

