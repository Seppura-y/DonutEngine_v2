#ifndef AV_VIDEO_DECODE_HANDLER_H
#define AV_VIDEO_DECODE_HANDLER_H

#include "iav_base_handler.h"
#include "av_video_decoder.h"

#include "av_data_tools.h"
#include "donut_buffers.h"

namespace Donut
{
	class DONUT_API AVVideoDecodeHandler : public IAVBaseHandler
	{
	public:
		AVVideoDecodeHandler();
		~AVVideoDecodeHandler();
		int openDecoder(AVCodecParameters* param);
		int openDecoder(std::shared_ptr<AVParamWarpper> param);
		void handle(void* data) override;

		AVFrame* getDecodedFrame();
	protected:
		void threadLoop() override;
	private:
		bool is_need_play_ = false;

		AVVideoDecoder decoder_;
		AVPacketDataList pkt_list_;

		AVFrame* decoded_frame_ = nullptr;

		std::unique_ptr<DonutAVDataCache<AVFrame*>> cache_;
	};

}

#endif