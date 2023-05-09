#ifndef DONUT_MEDIA_MANAGER_H
#define DONUT_MEDIA_MANAGER_H

#include "core.h"
#include "media_types.h"
#include "av_media_manager.h"
#include "donut_buffers.h"

namespace Donut
{
	struct AVFrame;

	class AVDemuxHandler;
	class AVVideoDecodeHandler;

	class DONUT_API DonutMediaManager : public AVMediaManager
	{
	public:
		DonutMediaManager();
		~DonutMediaManager();

		virtual int initPlayer() override;
		virtual void updateTimePos() override;
		virtual void updateTotalDuration() override;
		virtual void updateSoundVolume(int value) override;
		virtual void mediaEndReached() override;

		virtual int openMediaFile(const char* file_path, void* hwnd) override;
		virtual void setPlaying() override;
		virtual void setPause() override;
		virtual void setStop() override;

		virtual void setTimePos(double value) override;
		virtual void setSoundVolume(int value) override;

	protected:

	private:
		AVDemuxHandler* demux_handler_ = nullptr;
		AVVideoDecodeHandler* video_decode_handler_ = nullptr;

	};
}

#endif