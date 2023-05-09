#ifndef AV_VIDEO_ENCODE_HANDLER_H
#define AV_VIDEO_ENCODE_HANDLER_H

#include "iav_base_handler.h"

#include "av_video_encoder.h"
#include "av_video_scaler.h"

namespace Donut
{
	class DONUT_API AVVideoEncodeHandler : public IAVBaseHandler
	{
	public:
		int initHandler(AVCodecParameters* param,int out_width, int out_height,AVRational* src_timebase,AVRational* src_frame_rate);
		virtual void handle(void* pkg) override;
		virtual void stop()override;
		void setEncodePause(bool status);
		std::shared_ptr<AVParamWarpper> copyCodecParameters();

		int copyCodecExtraData(uint8_t* buffer, int& size);
		uint8_t* getSpsData();
		uint8_t* getPpsData();
		int getSpsSize();
		int getPpsSize();
	protected:
		virtual void threadLoop() override;
	private:
		bool is_pause_ = true;
		bool cache_avaliable_ = false;
		int encoded_count_ = 0;
		AVVideoEncoder encoder_;
		AVFrameDataList frame_list_;
		AVCachedPacketDataList cache_pkt_list_;

		bool is_need_scale_ = false;
		int output_width_ = -1;
		int output_height_ = -1;
		AVFrame* scaled_frame_ = nullptr;
		AVVideoScaler video_scaler_;

		AVRational* media_src_timebase_ = nullptr;
		AVRational* video_src_frame_rate_ = nullptr;
	};

}

#endif