#include "ivideo_renderer.h"
#include "sdl_renderer.h"
#include "qgl_renderer.h"

#include <iostream>
#include <thread>

extern"C"
{
#include <libavcodec/avcodec.h>
}

void sleepForMicroSecs(unsigned int ms)
{
	auto begin = clock();
	for (int i = 0; i < ms; i++)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		if ((clock() - begin) / (CLOCKS_PER_SEC / 1000) >= ms)
		{
			//std::cout << "sleep for fps : " << fps << std::endl;
			break;
		}
	}
}

IVideoRenderer* IVideoRenderer::createRenderer(RenderType type)
{
	switch (type)
	{
		case RenderType::RENDER_TYPE_SDL:
		{
			IVideoRenderer* renderer = new SDLRenderer();
			return renderer;
		}
		case RenderType::RENDER_TYPE_OPENGL:
		{
			IVideoRenderer* renderer = new QGLRenderer();
			return renderer;
		}
		default:
			break;
	}
    return nullptr;
}

int IVideoRenderer::initRenderer(AVCodecParameters* param)
{
	if (!param)
	{
		return -1;
	}
	PixFormat fmt = PixFormat::PIX_FORMAT_YUV420P;
	switch (param->format)
	{
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_YUVJ420P:
		fmt = PixFormat::PIX_FORMAT_YUV420P;
		break;
	default:
		break;
	}


	return initRenderer(param->width,param->height,fmt);
}

int IVideoRenderer::renderFrame(AVFrame* frame)
{
	int ret = -1;
	if (!frame || !frame->data[0])
	{
		std::cout << "DrawView : frame or frame->data is nullptr" << std::endl;
		return -1;
	}
	render_count_++;
	if (begin_ms_ <= 0)
	{
		begin_ms_ = clock();
	}
	else if ((clock() - begin_ms_) / (CLOCKS_PER_SEC / 1000) >= 1000)
	{
		render_fps_ = render_count_;
		render_count_ = 0;
		begin_ms_ = clock();
	}

	switch (frame->format)
	{
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_YUVJ420P:
		ret = renderYuvPlanarData(frame->data[0], frame->linesize[0], frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2]);
		break;
	case AV_PIX_FMT_RGBA:
	case AV_PIX_FMT_ARGB:
	case AV_PIX_FMT_BGRA:
		ret = renderPackedData(frame->data[0], frame->linesize[0]);
		break;
	default:
		ret = -1;
		break;
	}
	av_frame_free(&frame);
	return ret;
}
