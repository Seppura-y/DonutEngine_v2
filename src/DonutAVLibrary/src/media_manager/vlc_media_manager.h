#ifndef VLC_MEDIA_MANAGER_H
#define VLC_MEDIA_MANAGER_H

#include "core.h"
#include "media_types.h"
#include "av_media_manager.h"

namespace Donut
{
	class DONUT_API VLCMediaManager : public AVMediaManager
	{
	public:

		virtual int initPlayer() = 0;
		virtual void updateTimePos() = 0;
		virtual void updateTotalDuration() = 0;
		virtual void updateSoundVolume(int value) = 0;
		virtual void mediaEndReached() = 0;

		virtual int openMediaFile(const char* file_path, void* hwnd = nullptr) = 0;
		virtual void setPlaying() = 0;
		virtual void setPause() = 0;
		virtual void setStop() = 0;

		virtual void setTimePos(double value) = 0;
		virtual void setSoundVolume(int value) = 0;


	protected:

	private:

	};
}

#endif