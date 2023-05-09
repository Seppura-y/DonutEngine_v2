#include "av_video_codec.h"

extern"C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

#include <iostream>

static void printError(int err)
{
	char buffer[1024] = { 0 };
	av_strerror(err, buffer, sizeof(buffer));
	std::cout << buffer << std::endl;
}

#define PRINT_ERR_P(err) if(err != 0) {printError(err);return nullptr;}
#define PRINT_ERR_I(err) if(err != 0) {printError(err);return -1;}


namespace Donut
{
	int AVVideoCodec::setOption(const char* key, const char* value)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (codec_ctx_ == nullptr)
		{
			std::cout << "SetOption failed : codec_ctx_ is null" << std::endl;
			return -1;
		}
		int ret = av_opt_set(codec_ctx_->priv_data, key, value, 0);
		PRINT_ERR_I(ret);

		return 0;
	}

	int AVVideoCodec::setOption(const char* key, const int value)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (codec_ctx_ == nullptr)
		{
			std::cout << "SetOption failed : codec_ctx_ is null " << std::endl;
			return -1;
		}

		int ret = av_opt_set_int(codec_ctx_->priv_data, key, value, 0);
		PRINT_ERR_I(ret);

		return 0;
	}

	uint8_t* AVVideoCodec::getSpsData()
	{
		return nullptr;
	}

	uint8_t* AVVideoCodec::getPpsData()
	{
		return nullptr;
	}

	int AVVideoCodec::getSpsSize()
	{
		return 0;
	}

	int AVVideoCodec::getPpsSize()
	{
		return 0;
	}
}