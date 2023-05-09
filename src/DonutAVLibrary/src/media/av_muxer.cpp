#include "av_muxer.h"
extern"C"
{
#include <libavformat/avformat.h>
}

#include <iostream>

#include "log.h"

static void printError(int err)
{
	char buffer[1024] = { 0 };
	av_strerror(err, buffer, sizeof(buffer));
	std::cout << buffer << std::endl;
}

#define PRINT_ERR_P(err) if(err != 0) {printError(err);return nullptr;}
#define PRINT_ERR_I(err) if(err != 0) {printError(err);return -1;}

#define GET_ERR_STR(err,str) 	char buffer[1024] = { 0 }; av_strerror(err, buffer, sizeof(buffer)); std::string str(buffer);

namespace Donut
{
	AVMuxer::~AVMuxer()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (video_src_timebase_)
	{
		delete video_src_timebase_;
		video_src_timebase_ = nullptr;
	}
	if (audio_src_timebase_)
	{
		delete audio_src_timebase_;
		audio_src_timebase_ = nullptr;
	}
}

AVFormatContext* AVMuxer::openContext(const char* url, AVCodecParameters* video_params, AVCodecParameters* audio_params, AVMuxType type)
{
	if (!url || (!video_params && !audio_params))
	{
		DN_CORE_WARN("null params while open context");
		return nullptr;
	}

	AVFormatContext* fmt_ctx = nullptr;
	int ret = -1;

	switch (type)
	{
		case(AVMuxType::AV_MUX_TYPE_FILE):
		{
			ret = avformat_alloc_output_context2(&fmt_ctx, nullptr, nullptr, url);
			break;
		}
		case(AVMuxType::AV_MUX_TYPE_FFRTMP):
		{
			ret = avformat_alloc_output_context2(&fmt_ctx, nullptr, "flv", url);
			break;
		}
		case(AVMuxType::AV_MUX_TYPE_LIBRTMP):
		{
			DN_CORE_WARN("librtmp is not supported yet");
			break;
		}
		default:
		{
			DN_CORE_ERROR("mux type not found");
			break;
		}
	}
	if (ret < 0)
	{
		GET_ERR_STR(ret, str);
		DN_CORE_ERROR("open context failed : {}",str);
		return nullptr;
	}

	if (video_params)
	{
		AVStream* video_stream = avformat_new_stream(fmt_ctx, nullptr);
		if (!video_stream)
		{
			GET_ERR_STR(ret, str);
			DN_CORE_ERROR("create video stream failed : {}", str);
			avformat_free_context(fmt_ctx);
			return nullptr;
		}

		video_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
		ret = avcodec_parameters_copy(video_stream->codecpar, video_params);
		if (ret < 0)
		{
			GET_ERR_STR(ret, str);
			DN_CORE_ERROR("video params copy failed : {}", str);
			avformat_free_context(fmt_ctx);
			return nullptr;
		}
	}

	if(audio_params)
	{
		AVStream* audio_stream = avformat_new_stream(fmt_ctx, nullptr);
		audio_stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
		if (!audio_stream)
		{
			GET_ERR_STR(ret, str);
			DN_CORE_ERROR("create video stream failed : {}", str);
			avformat_free_context(fmt_ctx);
			return nullptr;
		}

		audio_stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
		ret = avcodec_parameters_copy(audio_stream->codecpar, audio_params);
		if (ret < 0)
		{
			GET_ERR_STR(ret, str);
			DN_CORE_ERROR("video params copy failed : {}", str);
			avformat_free_context(fmt_ctx);
			return nullptr;
		}
	}

	ret = avio_open(&fmt_ctx->pb, url, AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		GET_ERR_STR(ret, str);
		DN_CORE_ERROR("avio open failed : {}", str);
		avformat_free_context(fmt_ctx);
		is_network_connected_ = false;
		return nullptr;
	}
	else
	{
		is_network_connected_ = true;
	}

	return fmt_ctx;
}

int AVMuxer::writeHeader()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		DN_CORE_ERROR("writeheader failed : fmt_ctx_ is null ");
		return -1;
	}
	int ret = avformat_write_header(fmt_ctx_, nullptr);
	if (ret < 0)
	{
		GET_ERR_STR(ret, str);
		DN_CORE_ERROR("avformat_write_header failed : {}", str);
		return -1;
	}

	return 0;
}

int AVMuxer::writePacket(AVPacket* pkt)
{
	int ret = -1;
	std::unique_lock<std::mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return -1;
	}

	if (pkt->stream_index == getAudioIndex())
	{
		if (audio_begin_pts_ < 0)
		{
			audio_begin_pts_ = pkt->pts;
		}
		if (audio_src_timebase_)
		{
			timeScale(getAudioIndex(), pkt, *audio_src_timebase_, audio_begin_pts_);
		}
	}

	if (pkt->stream_index == getVideoIndex())
	{
		if (video_begin_pts_ < 0)
		{
			video_begin_pts_ = pkt->pts;
		}
		if (video_src_timebase_)
		{
			timeScale(getVideoIndex(), pkt, *video_src_timebase_, video_begin_pts_);
		}
	}
	if (mux_type_ == AVMuxType::AV_MUX_TYPE_FILE)
	{
		ret = av_interleaved_write_frame(fmt_ctx_, pkt);
		if (ret != 0)
		{
			GET_ERR_STR(ret, str);
			DN_CORE_WARN("av_interleaved_write_frame failed : {}", str);
			return -1;
		}
	}
	else
	{
		ret = av_write_frame(fmt_ctx_, pkt);
		if (ret != 0)
		{
			if (ret == -10054 || ret == -10053)
			{
				is_network_connected_ = false;
			}
			GET_ERR_STR(ret, str);
			DN_CORE_WARN("av_write_frame failed : {}", str);
			return -1;
		}
		//else
		//{
		//	is_network_connected_ = true;
		//}
	}

	return 0;
}

int AVMuxer::writeTrailer()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		DN_CORE_ERROR("write trailer failed : fmt_ctx_ is null");
		return -1;
	}

	int ret = av_write_trailer(fmt_ctx_);
	if (ret != 0)
	{
		GET_ERR_STR(ret, str);
		DN_CORE_ERROR("av_write_tailer failed : {}", str);
		return -1;
	}

	return 0;
}

void AVMuxer::setAudioTimebase(AVRational* timebase)
{
	if (!timebase) return;
	std::unique_lock<std::mutex> lock(mtx_);
	if (!audio_src_timebase_)
	{
		audio_src_timebase_ = new AVRational();
	}
	*audio_src_timebase_ = *timebase;
}

void AVMuxer::setVideoTimebase(AVRational* timebase)
{
	if (!timebase) return;
	std::unique_lock<std::mutex> lock(mtx_);
	if (!video_src_timebase_)
	{
		video_src_timebase_ = new AVRational();
	}
	*video_src_timebase_ = *timebase;
}

}