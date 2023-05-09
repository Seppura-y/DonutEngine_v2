#ifndef AV_VIDEO_ENCODER_H
#define AV_VIDEO_ENCODER_H

#include "core.h"
#include "av_video_codec.h"

struct AVFrame;
struct AVPacket;

namespace Donut
{
	class DONUT_API AVVideoEncoder : public AVVideoCodec
	{
	public:
		AVVideoEncoder(){}
		virtual ~AVVideoEncoder();

		virtual int openContext() override;
		virtual int flushCodec() override;

		int sendFrame(AVFrame* frame);
		int recvPacket(AVPacket* packet);

		AVFrame* getFrame() { return frame_; }
	protected:
		AVFrame* frame_ = nullptr;
		AVPacket* encoded_packet_ = nullptr;
	};
}


#endif