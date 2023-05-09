#ifndef AV_VIDEO_DECODER_H
#define AV_VIDEO_DECODER_H

#include "core.h"
#include "av_video_codec.h"
struct AVPacket;
struct AVFrame;

namespace Donut
{
	class DONUT_API AVVideoDecoder : public AVVideoCodec
	{
	public:
		AVVideoDecoder() {}
		~AVVideoDecoder();

		virtual int openContext() override;
		virtual int flushCodec() override;

		int sendPacket(AVPacket* pkt);
		int recvFrame(AVFrame* frame);

		int initHwDecode(int type = 4); // AV_HWDEVICE_TYPE_DXVA2

		AVFrame* getFrame() { return decoded_frame_; }
	protected:
		AVFrame* decoded_frame_ = nullptr;
	};

}

#endif