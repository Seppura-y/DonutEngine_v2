#include "av_mux_handler.h"


#include "log.h"
#include "av_data_tools.h"

extern"C"
{
#include <libavformat/avformat.h>
}
#include <iostream>
#include <string>
#include <sstream>

namespace Donut
{
AVMuxHandler::AVMuxHandler()
{
}

AVMuxHandler::~AVMuxHandler()
{
}

int AVMuxHandler::openAVMuxer(const char* url, AVCodecParameters* video_param, AVRational* video_timebase, AVCodecParameters* audio_param, AVRational* audio_timebase, AVMuxType type, uint8_t* extra_data, int extra_data_size)
{
    AVFormatContext* fmt_ctx = nullptr;
    int video_index = -1;
    int audio_index = -1;
    int ret = -1;
    if (!url)
    {
        DN_CORE_ERROR("url error");
        goto failed;
    }
    if (
        (video_param && !video_timebase) || (!video_param && video_timebase) ||
        (audio_param && !audio_timebase) || (!audio_param && audio_timebase)
        )
    {
        DN_CORE_ERROR("lack of params");
        goto failed;
    }

    fmt_ctx = muxer_.openContext(url, video_param, audio_param, mux_type_);
    if (!fmt_ctx)
    {
        DN_CORE_WARN("open muxer failed");
        goto failed;
    }
    video_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (video_index < 0)
    {
        DN_CORE_WARN("open muxer failed : video stream not found");
        goto failed;
    }
    else
    {
        if (extra_data)
        {
            memcpy(fmt_ctx->streams[video_index]->codecpar->extradata, extra_data, extra_data_size);
            fmt_ctx->streams[video_index]->codecpar->extradata_size = extra_data_size;
            av_dump_format(fmt_ctx, video_index, url, 1);
        }
    }

    audio_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audio_index < 0)
    {
        DN_CORE_WARN("open muxer failed : audio stream not found");
        goto failed;
    }
    else
    {
        av_dump_format(fmt_ctx, audio_index, url, 1);
    }

    {
        std::unique_lock<std::mutex> lock(mtx_);
        url_ = std::string(url);
        if (audio_param)
        {
            if (!audio_params_)
            {
                audio_params_ = AVParamWarpper::create();
            }

            if (!audio_timebase)
            {
                DN_CORE_WARN("audio param found but audio timebase is null");
                goto failed;
            }

            has_audio_ = true;
            avcodec_parameters_copy(audio_params_->para, audio_param);
            *audio_params_->time_base = *audio_timebase;
            muxer_.setAudioTimebase(audio_params_->time_base);
        }

        if (video_param)
        {
            if (!video_params_)
            {
                video_params_ = AVParamWarpper::create();
            }

            if (!video_timebase)
            {
                DN_CORE_WARN("video param found but video timebase is null");
                goto failed;
            }

            if (extra_data && extra_data_size > 0)
            {
                if (!extradata_)
                {
                    extradata_ = new uint8_t[extra_data_size];
                }
                memcpy(extradata_, extra_data, extra_data_size);
            }

            has_video_ = true;
            avcodec_parameters_copy(video_params_->para, video_param);
            *video_params_->time_base = *video_timebase;
            muxer_.setVideoTimebase(video_params_->time_base);
        }

        mux_type_ = type;
        muxer_.setFormatContext(fmt_ctx);
        muxer_.setMuxerType(mux_type_);
        muxer_.resetAudioBeginPts();
        muxer_.resetVideoBeginPts();
    }
    return 0;
failed:
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (fmt_ctx)
        {
            avformat_free_context(fmt_ctx);
        }
        if (audio_params_)
        {
            delete audio_params_;
            audio_params_ = nullptr;
        }
        if (video_params_)
        {
            delete video_params_;
            video_params_ = nullptr;
        }
        if (extradata_)
        {
            delete extradata_;
            extradata_ = nullptr;
            extradata_size_ = 0;
        }
        has_audio_ = false;
        has_video_ = false;
    }
    return -1;
}

int AVMuxHandler::resetMuxer()
{
    std::unique_lock<std::mutex> lock(mtx_);
    muxer_.closeContext();
    AVFormatContext* fmt_ctx = muxer_.openContext(url_.c_str(), video_params_->para, audio_params_->para, mux_type_);
    if (!fmt_ctx)
    {
        DN_CORE_ERROR("reset muxer failed");
        goto failed;
    }

    int video_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (video_stream_index >= 0)
    {
        if (extradata_)
        {
            memcpy(fmt_ctx->streams[video_index_]->codecpar->extradata, extradata_, extradata_size_);
            fmt_ctx->streams[video_index_]->codecpar->extradata_size = extradata_size_;
            av_dump_format(fmt_ctx, video_index_, url_.c_str(), 1);
        }
    }

    muxer_.setFormatContext(fmt_ctx);

    if (has_audio_)
    {
        muxer_.setAudioTimebase(audio_params_->time_base);
    }
    if (has_video_)
    {
        muxer_.setVideoTimebase(video_params_->time_base);
    }

    return 0;
failed:
    if (fmt_ctx)
    {
        avformat_free_context(fmt_ctx);
    }

    return -1;
}

void AVMuxHandler::threadLoop()
{
    int ret = -1;
    AVPacket* pkt = nullptr;
    muxer_.resetAudioBeginPts();
    muxer_.resetVideoBeginPts();
    muxer_.writeHeader();
    if (has_video_)
    {
        AVRational video_src_timebase = *(video_params_->time_base);
    }
    if (has_audio_)
    {
        AVRational audio_src_timebase = *(audio_params_->time_base);
    }
    while (!is_exit_)
    {
        if (is_pause_)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5));
            continue;
        }
        if (!muxer_.isNetworkConnected())
        {
            muxer_.closeContext();
            ret = resetMuxer();
            if (ret != 0)
            {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                continue;
            }
            muxer_.writeHeader();
        }

        pkt = pkt_list_.pop();
        if (pkt && (!pkt->data || pkt->size == 0))
        {
            av_packet_free(&pkt);
            continue;
        }
        if (!pkt)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            continue;
        }
        
        muxer_.writePacket(pkt);
        av_packet_free(&pkt);
    }

    muxer_.writeTrailer();
    muxer_.closeContext();
    pkt_list_.clear();
}

void AVMuxHandler::handle(void* package)
{
    if (!package) return;
    AVPacket* pkt = static_cast<AVPacket*>(package);
    std::unique_lock<std::mutex> lock(mtx_);
    pkt_list_.push(pkt);
}

std::shared_ptr<AVParamWarpper> AVMuxHandler::copyVideoParameters()
{
    return std::shared_ptr<AVParamWarpper>();
}

std::shared_ptr<AVParamWarpper> AVMuxHandler::copyAudioParameters()
{
    return std::shared_ptr<AVParamWarpper>();
}

}

