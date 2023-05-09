#ifndef AV_MUXER_H
#define AV_MUXER_H

#include "core.h"
#include "av_format_base.h"


struct AVRational;

namespace Donut
{
	enum class AVMuxType
	{
		AV_MUX_TYPE_FILE = 0,
		AV_MUX_TYPE_FFRTMP,
		AV_MUX_TYPE_LIBRTMP
	};

	class DONUT_API AVMuxer : public AVFormatBase
	{
	public:
		~AVMuxer();
		AVFormatContext* openContext(const char* url, AVCodecParameters* video_params = nullptr,AVCodecParameters* audio_params = nullptr, AVMuxType type = AVMuxType::AV_MUX_TYPE_FILE);

		int writeHeader();
		int writePacket(AVPacket* pkt);
		int writeTrailer();

		void setAudioTimebase(AVRational* timebase);
		void setVideoTimebase(AVRational* timebase);

		void resetAudioBeginPts() { std::unique_lock<std::mutex> lock(mtx_); audio_begin_pts_ = -1; }
		void resetVideoBeginPts() { std::unique_lock<std::mutex> lock(mtx_); video_begin_pts_ = -1;}

		void setMuxerType(AVMuxType type) { mux_type_ = type; }
	private:
		AVRational* video_src_timebase_ = nullptr;
		AVRational* audio_src_timebase_ = nullptr;

		int64_t video_begin_pts_ = -1;
		int64_t audio_begin_pts_ = -1;

		AVMuxType mux_type_ = AVMuxType::AV_MUX_TYPE_FILE;
	};

}

#endif