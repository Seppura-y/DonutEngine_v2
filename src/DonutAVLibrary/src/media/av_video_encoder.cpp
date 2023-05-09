#include "av_video_encoder.h"
#include <iostream>

extern"C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

#ifdef _WIN32
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#endif

using namespace std;

static void printError(int err)
{
	char buffer[1024] = { 0 };
	av_strerror(err, buffer, sizeof(buffer));
	cout << buffer << endl;
}

#define PRINT_ERR_P(err) if(err != 0) {printError(err);return nullptr;}
#define PRINT_ERR_I(err) if(err != 0) {printError(err);return -1;}

namespace Donut
{
	AVVideoEncoder::~AVVideoEncoder()
	{
		if (frame_)
		{
			av_frame_free(&frame_);
			frame_ = nullptr;
		}
		if (encoded_packet_)
		{
			av_packet_free(&encoded_packet_);
			encoded_packet_ = nullptr;
		}
	}

	int AVVideoEncoder::openContext()
	{
		unique_lock<mutex> lock(mtx_);
		if (codec_ctx_ == nullptr)
		{
			cout << "OpenContext failed : codec_ctx_ is null" << endl;
			return -1;
		}
		int ret = -1;

		//AVDictionary* dict = nullptr;
		//av_dict_set(&dict, "tune", "zerolatency", 0);
		//av_dict_set(&dict, "preset", "ultrafast", 0);
		ret = avcodec_open2(codec_ctx_, NULL, &options_);
		PRINT_ERR_I(ret);

		frame_ = av_frame_alloc();
		frame_->width = codec_ctx_->width;
		frame_->height = codec_ctx_->height;
		frame_->format = codec_ctx_->pix_fmt;
		
		ret = av_frame_get_buffer(frame_, 0);
		PRINT_ERR_I(ret);


		encoded_packet_ = av_packet_alloc();

		return 0;
	}

	int AVVideoEncoder::flushCodec()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (codec_ctx_)
		{
			return avcodec_send_frame(codec_ctx_, nullptr);
		}
		return 0;
	}

	int AVVideoEncoder::sendFrame(AVFrame* frame)
	{
		unique_lock<mutex> lock(mtx_);
		if (codec_ctx_ == nullptr)
		{
			cout << "codec_ctx_ is nullptr" << endl;
			return -1;
		}
		int ret = av_frame_make_writable(frame);
		if (ret != 0)
		{
			return -1;
		}

		ret = avcodec_send_frame(codec_ctx_, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR(EOF))
		{
			cout << "avcodec_send_frame falied (ret == AVERROR(EAGAIN) || ret == AVERROR(EOF))" << endl;
			printError(ret);
			return -1;
		}
		else if (ret == 0)
		{
			return 0;
		}
		else
		{
			cout << "avcodec_send_frame falied" << endl;
			printError(ret);
			return -1;
		}
	}

	int AVVideoEncoder::recvPacket(AVPacket* packet)
	{
		unique_lock<mutex> lock(mtx_);
		if (codec_ctx_ == nullptr)
		{
			cout << "codec_ctx_ is nullptr" << endl;
			return -1;
		}

		int ret = avcodec_receive_packet(codec_ctx_, packet);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR(EOF))
		{
			return -1;
		}
		else if(ret == 0)
		{
			return 0;
		}
		else
		{
			return -1;
		}
		return 0;
	}

}
