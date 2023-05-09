#include "av_demux_handler.h"


#include "log.h"
#include "av_data_tools.h"

extern"C"
{
#include <libavformat/avformat.h>
}

namespace Donut
{
AVDemuxHandler::AVDemuxHandler()
{
}

AVDemuxHandler::~AVDemuxHandler()
{
}

int AVDemuxHandler::openAVSource(const char* url, int timeout)
{
    this->url_ = url;
    this->timeout_threshold_ = timeout;
    demuxer_.closeContext();
    AVFormatContext* fmt_ctx = demuxer_.openContext(url_.c_str());
    if (!fmt_ctx)
    {
        DN_CORE_ERROR("demuxer_.openContext failed");
        return -1;
    }
    demuxer_.setFormatContext(fmt_ctx);
    demuxer_.setTimeoutThreshold(timeout_threshold_);

    has_audio_ = demuxer_.hasAudio();
    has_video_ = demuxer_.hasVideo();

    audio_index_ = demuxer_.getAudioIndex();
    video_index_ = demuxer_.getVideoIndex();
    return 0;
}

void AVDemuxHandler::threadLoop()
{
    AVPacket* demux_pkt = av_packet_alloc();
    while (!is_exit_)
    {
        if (is_pause_)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5));
            continue;
        }
        if (demuxer_.readPacket(demux_pkt) != 0)
        {
            std::cout << "demuxer_.readPacket failed" << std::endl;
            if (!demuxer_.isNetworkConnected())
            {
                openAVSource(url_.c_str(), timeout_threshold_);
            }

            av_packet_unref(demux_pkt);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            continue;
        }
        //std::cout << "." << std::flush;
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        //if (getNextHandler())
        //{
        //    getNextHandler()->handle((void*)demux_pkt);
        //}
        if (has_audio_ && audio_handler_ && demux_pkt->stream_index == audio_index_)
        {
            audio_handler_->handle((void*)demux_pkt);
        }
        else if (has_video_ && video_handler_ && demux_pkt->stream_index == video_index_)
        {
            video_handler_->handle((void*)demux_pkt);
        }

        av_packet_unref(demux_pkt);
    }

    av_packet_free(&demux_pkt);
}

void AVDemuxHandler::handle(void* package)
{
}

std::shared_ptr<AVParamWarpper> AVDemuxHandler::copyVideoParameters()
{
    return demuxer_.copyVideoParameters();
}

std::shared_ptr<AVParamWarpper> AVDemuxHandler::copyAudioParameters()
{
    return demuxer_.copyAudioParameters();
}

void AVDemuxHandler::setVideoHandler(IAVBaseHandler* handler)
{
    std::unique_lock<std::mutex> lock(mtx_);
    video_handler_ = handler;
}

void AVDemuxHandler::setAudioHandler(IAVBaseHandler* handler)
{
    std::unique_lock<std::mutex> lock(mtx_);
    audio_handler_ = handler;
}


}
