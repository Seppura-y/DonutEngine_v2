#include "donut_media_manager.h"

#include "log.h"
#include "av_demux_handler.h"
#include "av_video_decode_handler.h"


namespace Donut
{
    DonutMediaManager::DonutMediaManager()
    {

    }

    DonutMediaManager::~DonutMediaManager()
    {
        if (demux_handler_)
        {
            delete demux_handler_;
            demux_handler_ = nullptr;
        }

        if (video_decode_handler_)
        {
            delete video_decode_handler_;
            video_decode_handler_ = nullptr;
        }
    }

    int DonutMediaManager::initPlayer()
    {
        return 0;
    }

    void DonutMediaManager::updateTimePos()
    {
    }

    void DonutMediaManager::updateTotalDuration()
    {
    }

    void DonutMediaManager::updateSoundVolume(int value)
    {
    }

    void DonutMediaManager::mediaEndReached()
    {
    }

    int DonutMediaManager::openMediaFile(const char* file_path, void* hwnd)
    {
        int ret = -1;
        if (!demux_handler_)
        {
            demux_handler_ = new AVDemuxHandler();
        }
        ret = demux_handler_->openAVSource(file_path);
        if (ret != 0)
        {
            goto failed;
        }

        if (!video_decode_handler_)
        {
            video_decode_handler_ = new AVVideoDecodeHandler();
        }
        ret = video_decode_handler_->openDecoder(demux_handler_->copyVideoParameters());
        if (ret != 0)
        {
            goto failed;
        }

        demux_handler_->setVideoHandler(video_decode_handler_);
        demux_handler_->start();
        video_decode_handler_->start();

        return ret;

    failed:
        if (demux_handler_)
        {
            delete demux_handler_;
            demux_handler_ = nullptr;
        }

        if (video_decode_handler_)
        {
            delete video_decode_handler_;
            video_decode_handler_ = nullptr;
        }
        return ret;
    }

    void DonutMediaManager::setPlaying()
    {
    }

    void DonutMediaManager::setPause()
    {
    }

    void DonutMediaManager::setStop()
    {
    }

    void DonutMediaManager::setTimePos(double value)
    {
    }

    void DonutMediaManager::setSoundVolume(int value)
    {
    }

}
