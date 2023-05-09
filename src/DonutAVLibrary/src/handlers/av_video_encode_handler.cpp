#include "av_video_encode_handler.h"
#include <iostream>

extern"C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#ifdef _WIN32
#include <Windows.h>
#endif

static long long NowMs()
{
	return (clock() / (CLOCKS_PER_SEC / 1000));
}


static void PrintError(int err)
{
	char buffer[1024] = { 0 };
	av_strerror(err, buffer, sizeof(buffer));
	std::cout << buffer << std::endl;
}

static int getCpuNumber()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}

namespace Donut
{
	int AVVideoEncodeHandler::initHandler(AVCodecParameters* param,int out_width, int out_height,AVRational* src_timebase,AVRational* src_frame_rate)
{
	std::unique_lock<std::mutex> lock(mtx_);
	int isok = 0;
	AVCodecContext* codec_ctx = encoder_.createContext(AV_CODEC_ID_H264, false);
	if (codec_ctx == nullptr)
	{
		std::cout << "encode handler create context failed" << std::endl;
		return -1;
	}
	output_width_ = out_width;
	output_height_ = out_height;
	if (param)
	{
		if (param->width != out_width && param->height != out_height)
		{
			is_need_scale_ = true;
			//video_scaler_.setInputParam(param->width, param->height, out_width, out_height);
			//video_scaler_.initScale(param->format, param->format);
			video_scaler_.initScaler(param->width, param->height, param->format, out_width, out_height, param->format);

			scaled_frame_ = av_frame_alloc();
			scaled_frame_->format = param->format;
			scaled_frame_->width = output_width_;
			scaled_frame_->height = output_height_;
			isok = av_frame_get_buffer(scaled_frame_, 0);
			if (isok != 0)
			{
				std::cout << "set scaled_frame failed : " << std::flush;
				PrintError(isok);
				return -1;
			}
		}
		else
		{
			is_need_scale_ = false;
			output_width_ = param->width;
			output_height_ = param->height;
		}
	}

	codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
	codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	codec_ctx->width = output_width_;
	codec_ctx->height = output_height_;

	codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	if (src_timebase)
	{
		if (!media_src_timebase_)
		{
			media_src_timebase_ = new AVRational();
		}
		media_src_timebase_->num = src_timebase->num;
		media_src_timebase_->den = src_timebase->den;
		codec_ctx->time_base = { media_src_timebase_->num,media_src_timebase_->den };
	}
	else
	{
		codec_ctx->time_base = av_get_time_base_q();
	}

	if (src_frame_rate)
	{
		if (!video_src_frame_rate_)
		{
			video_src_frame_rate_ = new AVRational();
		}
		video_src_frame_rate_->num = src_frame_rate->num;
		video_src_frame_rate_->den = src_frame_rate->den;
		codec_ctx->framerate = { video_src_frame_rate_->num,video_src_frame_rate_->den };
	}
	else
	{
		codec_ctx->framerate = { 25,1 };
	}
	
	codec_ctx->thread_count = getCpuNumber();

	//AVCodecParameters par;
	codec_ctx->max_b_frames = 0;
	codec_ctx->gop_size = 50;
	codec_ctx->bit_rate = 20 * 1024 * 8;

	encoder_.setCodecContext(codec_ctx);

	isok = encoder_.setOption("preset", "ultrafast");
	if (isok != 0)
	{
		return -1;
	}
	isok = encoder_.setOption("qp", 23);
	if (isok != 0)
	{
		return -1;
	}
	isok = encoder_.setOption("tune", "zerolatency");
	if (isok != 0)
	{
		return -1;
	}

	isok = encoder_.setOption("nal-hrd", "cbr");
	if (isok != 0)
	{
		return -1;
	}

	isok = encoder_.openContext();
	if (isok != 0)
	{
		//LOGERROR("encode_.OpenContext failed");
		return -1;
	}
	return 0;
}

void AVVideoEncodeHandler::handle(void* pkg)
{
	if (!pkg)
	{
		std::cout << "pkg is null" << std::endl;
		return;
	}
	AVFrame* frame = (AVFrame*)(pkg);
	//if (pkg->av_type_ == AVHandlerPackageAVType::AVHANDLER_PACKAGE_AV_TYPE_VIDEO &&
	//	pkg->type_ == AVHandlerPackageType::AVHANDLER_PACKAGE_TYPE_FRAME &&
	//	pkg->payload_.frame_ == nullptr)
	//{
	//	std::cout << "AVHandlerPackage payload is null" << std::endl;
	//	return;
	//}

	if (is_pause_)
	{
		//av_frame_free(&pkg->payload_.frame_);
	}
	else
	{
		frame_list_.push(frame);
		//frame_list_.Push(pkg->payload_.frame_);
	}
	//av_frame_unref(pkg->payload_.frame_);
	av_frame_unref(frame);
	return;
}

void AVVideoEncodeHandler::setEncodePause(bool status)
{
	std::unique_lock<std::mutex> lock(mtx_);
	is_pause_ = status;
}

void AVVideoEncodeHandler::threadLoop()
{
	int ret = -1;
	AVPacket* pkt = av_packet_alloc();
	AVHandlerPackage* pkg = new AVHandlerPackage();
	while (!is_exit_)
	{
		if (is_pause_)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(5));
			continue;
		}

		AVFrame* frame = frame_list_.pop();
		if (frame != nullptr && (frame->data[0] == nullptr || frame->linesize[0] == 0))
		{
			av_frame_unref(frame);
			av_frame_free(&frame);
			std::this_thread::sleep_for(std::chrono::microseconds(1));
			continue;
		}
		if(!frame)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1));
			continue;
		}

		int64_t du = frame->pkt_duration;
		frame->pict_type = AV_PICTURE_TYPE_NONE;

		if (is_need_scale_)
		{
			if (!scaled_frame_)
			{
				scaled_frame_ = av_frame_alloc();
				scaled_frame_->format = frame->format;
				scaled_frame_->width = output_width_;
				scaled_frame_->height = output_height_;
				ret = av_frame_get_buffer(scaled_frame_, 0);
				if (ret != 0)
				{
					PrintError(ret);
					continue;
				}
			}
			scaled_frame_->pts = frame->pts;
			scaled_frame_->pkt_pts = frame->pkt_pts;
			scaled_frame_->pkt_dts = frame->pkt_dts;
			scaled_frame_->pkt_duration = frame->pkt_duration;

			if (scaled_frame_->data[0] && scaled_frame_->linesize[0])
			{
				video_scaler_.getScaledFrame(frame, scaled_frame_);
				ret = encoder_.sendFrame(scaled_frame_);
				av_frame_unref(frame);
				av_frame_free(&frame);
				av_frame_unref(scaled_frame_);
				av_frame_free(&scaled_frame_);
				if (ret != 0)
				{
					//std::cout << "encode handler : send frame failed " << std::endl;
					//this_thread::sleep_for(1ms);
					continue;
				}
			}
			else
			{
				continue;
			}

		}
		else
		{
			ret = encoder_.sendFrame(frame);
			av_frame_unref(frame);
			av_frame_free(&frame);
			if (ret != 0)
			{
				//std::cout << "encode handler : send frame failed " << std::endl;
				//this_thread::sleep_for(1ms);
				continue;
			}
		}


		//if (media_src_timebase_)
		//{
		//	if (frame->pts)
		//	{
		//		int64_t scaled_pts = ScaleToMsec(frame->pts, *media_src_timebase_);
		//		frame->pts = scaled_pts;
		//	}
		//	if (frame->pkt_pts)
		//	{
		//		int64_t scaled_pkt_pts = ScaleToMsec(frame->pkt_pts, *media_src_timebase_);
		//		frame->pkt_pts = scaled_pkt_pts;
		//	}
		//	if (frame->pkt_dts)
		//	{
		//		int64_t scaled_dts = ScaleToMsec(frame->pkt_dts, *media_src_timebase_);
		//		frame->pkt_dts = scaled_dts;
		//	}
		//	if (frame->pkt_duration)
		//	{
		//		int64_t scaled_duration = ScaleToMsec(frame->pkt_duration, *media_src_timebase_);
		//		frame->pkt_duration = scaled_duration;
		//	}
		//}
		//else
		//{
		//	//frame->pts = (encoded_count_++) * (encoder_.get_codec_ctx()->time_base.den) / (encoder_.get_codec_ctx()->time_base.num);
		//	//frame->pkt_pts = frame->pts;
		//	//frame->pkt_dts = frame->pts;
		//}

		ret = encoder_.recvPacket(pkt);
		if (ret != 0)
		{
			std::cout << "encode handler : recv packet failed " << std::endl;
			av_packet_unref(pkt);
			std::this_thread::sleep_for(std::chrono::microseconds(1));
			continue;
		}
		if (pkt->buf == nullptr || pkt->size == 0 || pkt->data == nullptr)
		{
			av_packet_unref(pkt);
			std::this_thread::sleep_for(std::chrono::microseconds(1));
			continue;
		}
		pkt->pts;
		pkt->dts;
		pkt->duration;
		if (pkt->duration == 0)
		{
			pkt->duration = du;
		}
		cache_pkt_list_.push(pkt);
		av_packet_unref(pkt);

		if (cache_pkt_list_.size() >= 10)
		{
			cache_avaliable_ = true;
		}
		if (cache_avaliable_)
		{
			if (is_video_callback_enabled_)
			{
				AVPacket* pkt = cache_pkt_list_.pop();
				if (video_callback_)
				{
					video_callback_(pkt);
					av_packet_unref(pkt);
					av_packet_free(&pkt);
				}
				else
				{
					av_packet_unref(pkt);
					av_packet_free(&pkt);
					continue;
				}
			}
			else
			{
				AVPacket* pkt = cache_pkt_list_.pop();
				if (getNextHandler())
				{
					pkg->av_type_ = AVHandlerPackageAVType::AVHANDLER_PACKAGE_AV_TYPE_VIDEO;
					pkg->type_ = AVHandlerPackageType::AVHANDLER_PACKAGE_TYPE_PACKET;
					pkg->payload_.packet_ = pkt;
					getNextHandler()->handle(pkg);
					av_packet_unref(pkt);
					av_packet_free(&pkt);
				}
				else
				{
					av_packet_unref(pkt);
					av_packet_free(&pkt);
					continue;
				}
			}
		}
	}
	frame_list_.clear();
	cache_pkt_list_.clear();
	encoder_.sendFrame(nullptr);
	while (1)
	{
		encoder_.recvPacket(pkt);
		if (pkt->data)
		{
			av_packet_unref(pkt);
		}
		else
			break;

	}
	av_packet_free(&pkt);
	delete pkg;
}

int AVVideoEncodeHandler::copyCodecExtraData(uint8_t* buffer, int& size)
{
	return encoder_.getCodecExtraData(buffer, size);
}

std::shared_ptr<AVParamWarpper> AVVideoEncodeHandler::copyCodecParameters()
{
	return encoder_.copyCodecParam();
}

uint8_t* AVVideoEncodeHandler::getSpsData()
{
	std::unique_lock<std::mutex> lock(mtx_);
	return encoder_.getSpsData();
}

uint8_t* AVVideoEncodeHandler::getPpsData()
{
	std::unique_lock<std::mutex> lock(mtx_);
	return encoder_.getPpsData();
}

int AVVideoEncodeHandler::getSpsSize()
{
	std::unique_lock<std::mutex> lock(mtx_);
	return encoder_.getSpsSize();
}

int AVVideoEncodeHandler::getPpsSize()
{
	std::unique_lock<std::mutex> lock(mtx_);
	return encoder_.getPpsSize();
}

void AVVideoEncodeHandler::stop()
{
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (scaled_frame_)
		{
			av_frame_unref(scaled_frame_);
			av_frame_free(&scaled_frame_);
			scaled_frame_ = nullptr;
		}
	}
	IAVBaseHandler::stop();
}
}
