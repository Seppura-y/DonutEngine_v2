#include "av_video_decode_handler.h"

extern"C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}
#include "log.h"

namespace Donut
{
AVVideoDecodeHandler::AVVideoDecodeHandler()
{
    //cache_ = std::make_unique<DonutAVDataCache<AVFrame*>>();
}
AVVideoDecodeHandler::~AVVideoDecodeHandler()
{
}

int AVVideoDecodeHandler::openDecoder(AVCodecParameters* param)
{
    if (!param)
    {
        DN_CORE_WARN("param is null");
        return -1;
    }
    std::unique_lock<std::mutex> lock(mtx_);
    AVCodecContext* codec_ctx = decoder_.createContext(param->codec_id, true);
    if (!codec_ctx)
    {
        DN_CORE_WARN("create context failed");
        return -1;
    }
    avcodec_parameters_to_context(codec_ctx, param);
    decoder_.setCodecContext(codec_ctx);
    if (decoder_.openContext() != 0)
    {
        DN_CORE_ERROR("open context failed");
        return -1;
    }

    return 0;
}

int AVVideoDecodeHandler::openDecoder(std::shared_ptr<AVParamWarpper> param)
{
    return openDecoder(param->para);
}

void AVVideoDecodeHandler::handle(void* data)
{
    if (!data)
    {
        DN_CORE_WARN("data is null");
        return;
    }
    AVPacket* pkt = (AVPacket*)data;
    if (pkt->stream_index != 0)
    {
        av_packet_unref(pkt);
        //av_packet_free(&pkt);
        return;
    }
    pkt_list_.push((AVPacket*)data);
    av_packet_unref((AVPacket*)data);
}

AVFrame* AVVideoDecodeHandler::getDecodedFrame()
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (!is_need_play_ || !decoded_frame_ || !decoded_frame_->buf[0])
    {
        return nullptr;
    }
    AVFrame* frame = av_frame_alloc();
    int ret = av_frame_ref(frame, decoded_frame_);
    if (ret != 0)
    {
        av_frame_free(&frame);
        DN_CORE_WARN("frame ref failed");
        return nullptr;
    }
    is_need_play_ = false;
    return frame;
}

void AVVideoDecodeHandler::threadLoop()
{
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (!decoded_frame_)
        {
            decoded_frame_ = av_frame_alloc();
        }
    }

    while (!is_exit_)
    {
        auto pkt = pkt_list_.pop();
        if (!pkt)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            continue;
        }
        int ret = decoder_.sendPacket(pkt);
        av_packet_free(&pkt);
        if (ret != 0)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            continue;
        }
        
        {
            std::unique_lock<std::mutex> lock(mtx_);
            ret = decoder_.recvFrame(decoded_frame_);
            if (ret == 0)
            {
                is_need_play_ = true;

            }
        }


        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (decoded_frame_)
        {
            av_frame_free(&decoded_frame_);
        }
    }
}

}
