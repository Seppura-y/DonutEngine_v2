#include "av_format_base.h"

extern"C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <iostream>

#include "log.h"

namespace Donut
{
AVFormatBase::AVFormatBase()
{
	if (!audio_src_timebase_)
	{
		audio_src_timebase_ = new AVRational();
	}
	if (!video_src_timebase_)
	{
		video_src_timebase_ = new AVRational();
	}
}

AVFormatBase::~AVFormatBase()
{
	if (audio_src_timebase_)
	{
		delete audio_src_timebase_;
		audio_src_timebase_ = nullptr;
	}
	if (video_src_timebase_)
	{
		delete video_src_timebase_;
		video_src_timebase_ = nullptr;
	}
}

int AVFormatBase::setFormatContext(AVFormatContext* ctx)
{
	closeContext();
	std::unique_lock<std::mutex> lock(mtx_);
	fmt_ctx_ = ctx;
	if (fmt_ctx_ == nullptr)
	{
		is_network_connected_ = false;
		return 0;
	}
	is_network_connected_ = true;
	last_proc_time_ = GetCurrentTimeMsec();
	if (timeout_threshold_ > 0)
	{
		AVIOInterruptCB callback = { timeoutCallback, this };
		fmt_ctx_->interrupt_callback = callback;
	}

	audio_index_ = -1;
	video_index_ = -1;
	for (int i = 0; i < fmt_ctx_->nb_streams; i++)
	{
		if (fmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_index_ = fmt_ctx_->streams[i]->index;
			audio_codec_id_ = fmt_ctx_->streams[i]->codecpar->codec_id;
			audio_src_timebase_->den = fmt_ctx_->streams[i]->time_base.den;
			audio_src_timebase_->num = fmt_ctx_->streams[i]->time_base.num;
		}

		if (fmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_index_ = i;
			video_codec_id_ = fmt_ctx_->streams[i]->codecpar->codec_id;
			video_src_timebase_->den = fmt_ctx_->streams[i]->time_base.den;
			video_src_timebase_->num = fmt_ctx_->streams[i]->time_base.num;
			
			// If demux, get sps pps data
			if (fmt_ctx_->iformat)
			{

			}
		}
	}
	return 0;
}

int AVFormatBase::closeContext()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (fmt_ctx_)
	{
		if (fmt_ctx_->oformat)
		{
			/* -muxing: set by the user before avformat_write_header().The caller must
			* take care of closing / freeing the IO context.
			*/
			if (fmt_ctx_->pb)
			{
				avio_closep(&fmt_ctx_->pb);
			}
			avformat_free_context(fmt_ctx_);
		}
		else if (fmt_ctx_->iformat)
		{
			avformat_close_input(&fmt_ctx_);
		}
		else
		{
			avformat_free_context(fmt_ctx_);
		}
		fmt_ctx_ = nullptr;
	}

	return 0;
}

int AVFormatBase::copyCodecParameters(int stream_index, AVCodecParameters* dst)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return -1;
	}
	if (stream_index < 0 || stream_index > fmt_ctx_->nb_streams)
	{
		return -1;
	}
	int ret = avcodec_parameters_copy(dst, fmt_ctx_->streams[stream_index]->codecpar);
	if (ret < 0)
	{
		return -1;
	}
	return 0;
}

int AVFormatBase::copyCodecParameters(int stream_index, AVCodecContext* dst)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return -1;
	}
	if (stream_index < 0 || stream_index > fmt_ctx_->nb_streams)
	{
		return -1;
	}
	int ret = avcodec_parameters_to_context(dst, fmt_ctx_->streams[stream_index]->codecpar);
	if (ret < 0)
	{
		return -1;
	}
	return 0;
}

std::shared_ptr<AVParamWarpper> AVFormatBase::copyVideoParameters()
{
	std::shared_ptr<AVParamWarpper> param = std::make_shared<AVParamWarpper>();
	std::unique_lock<std::mutex> lock(mtx_);
	if (!hasVideo() || !fmt_ctx_)
	{
		return {};
	}
	avcodec_parameters_copy(param->para, fmt_ctx_->streams[video_index_]->codecpar);
	*param->time_base = fmt_ctx_->streams[video_index_]->time_base;
	return param;
}

std::shared_ptr<AVParamWarpper> AVFormatBase::copyAudioParameters()
{
	std::shared_ptr<AVParamWarpper> param = std::make_shared<AVParamWarpper>();
	std::unique_lock<std::mutex> lock(mtx_);
	if (!hasAudio() || !fmt_ctx_)
	{
		return {};
	}
	avcodec_parameters_copy(param->para, fmt_ctx_->streams[audio_index_]->codecpar);
	*param->time_base = fmt_ctx_->streams[audio_index_]->time_base;
	return param;
}

void AVFormatBase::getVideoTimebase(AVRational* rational)
{
	rational->num = video_src_timebase_->num;
	rational->den = video_src_timebase_->den;
}

void AVFormatBase::getAudioTimebase(AVRational* rational)
{
	rational->num = audio_src_timebase_->num;
	rational->den = audio_src_timebase_->den;
}

int AVFormatBase::timeScale(int index, AVPacket* pkt, AVRational src, long long pts)
{
	if (!pkt)
	{
		DN_CORE_WARN("timeScale failed : (!pkt)");
		return -1;
	}
	//std::unique_lock<std::mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		DN_CORE_WARN("timeScale failed : (!fmt_ctx_)");
		return -1;
	}
	AVStream* stream = fmt_ctx_->streams[index];
	pkt->pts = av_rescale_q_rnd(pkt->pts - pts, src, stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	pkt->dts = av_rescale_q_rnd(pkt->dts - pts, src, stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	pkt->duration = av_rescale_q_rnd(pkt->duration, src, stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	pkt->pos = -1;
	return 0;
}

void AVFormatBase::setTimeoutThreshold(int timeout)
{
	std::unique_lock<std::mutex> lock(mtx_);
	this->timeout_threshold_ = timeout;
	if (fmt_ctx_)
	{
		AVIOInterruptCB callback = { timeoutCallback, this };
		fmt_ctx_->interrupt_callback = callback;
	}
}

int AVFormatBase::timeoutCallback(void* opaque)
{
	AVFormatBase* fmt = static_cast<AVFormatBase*>(opaque);
	if (fmt->isTimeout())
	{
		std::cout << "timeout occur" << std::endl;
		return 1;
	}
	return 0;
}

bool AVFormatBase::isTimeout()
{
	int64_t tmp = GetCurrentTimeMsec();
	if (is_timeout_enabled_ && (tmp - last_proc_time_ >= timeout_threshold_))
	{
		is_network_connected_ = false;
		last_proc_time_ = GetCurrentTimeMsec();
		return true;
	}
	return false;
}

}
