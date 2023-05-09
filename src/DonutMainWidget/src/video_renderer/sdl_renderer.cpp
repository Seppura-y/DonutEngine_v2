#include "sdl_renderer.h"
#include <SDL2/SDL.h>

#include <iostream>

int SDLRenderer::initRenderer(int width, int height, PixFormat fmt, void* win_id)
{
	if (initSdlLibrary() != 0)
	{
		std::cout << "InitSdlLibrary failed" << std::endl;
		return -1;
	}

	int format = -1;
	std::unique_lock<std::mutex> lock(mtx_);

	width_ = width;
	height_ = height;
	pix_fmt_ = fmt;
	window_id_ = win_id;

	resetRenderer();

	if (!window_)
	{
		if (!win_id)
		{
			window_ = SDL_CreateWindow(
				"win",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				width, height,
				SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
			);
		}
		else
		{
			window_ = SDL_CreateWindowFrom(win_id);
		}
	}

	if (!window_)
	{
		std::cout << "SDL_CreateWindow failed " << SDL_GetError() << std::endl;
		return -1;
	}

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	if (!renderer_)
	{
		std::cout << "SDL_CreateRenderer failed : " << SDL_GetError() << std::endl;
		return -1;
	}

	switch (pix_fmt_)
	{
	case PixFormat::PIX_FORMAT_YUV420P:
		format = SDL_PIXELFORMAT_IYUV;
		break;
	case PixFormat::PIX_FORMAT_ARGB:
		format = SDL_PIXELFORMAT_ARGB32;
		break;
	case PixFormat::PIX_FORMAT_BGRA:
		format = SDL_PIXELFORMAT_BGRA32;
		break;
	case PixFormat::PIX_FORMAT_RGBA:
		format = SDL_PIXELFORMAT_RGBA32;
		break;
	}

	texture_ = SDL_CreateTexture(renderer_, format, SDL_TEXTUREACCESS_STREAMING, width_, height_);
	if (!texture_)
	{
		std::cout << "SDL_CreateTexture failed : " << SDL_GetError() << std::endl;
		return -1;
	}
    return 0;
}

int SDLRenderer::renderPackedData(const uint8_t* data, int linesize)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!window_ || !renderer_ || !texture_)
	{
		std::cout << "DrawView failed : (!window_ || !renderer_ || !texture_)" << std::endl;
		return -1;
	}

	if (width_ <= 0 || height_ <= 0)
	{
		std::cout << "DrawView failed : (width_ == 0 || height_ == 0)" << std::endl;
		return -1;
	}

	if (linesize <= 0)
	{
		switch (pix_fmt_)
		{
		case PixFormat::PIX_FORMAT_YUV420P:
			linesize = width_;
			break;
		case PixFormat::PIX_FORMAT_ARGB:
		case PixFormat::PIX_FORMAT_BGRA:
		case PixFormat::PIX_FORMAT_RGBA:
			linesize = width_ * 4;
			break;
		default:
			std::cout << "DrawView : could not find a linesize" << std::endl;
			return -1;
		}
	}
	if (linesize <= 0)
	{
		return -1;
	}

	SDL_RenderClear(renderer_);

	int ret = SDL_UpdateTexture(texture_, nullptr, data, linesize);
	if (ret != 0)
	{
		std::cout << "SDL_UpdateTexture failed : " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Rect src_rect;
	SDL_Rect* dst_rect = nullptr;
	if (scaled_width_ > 0 && scaled_height_ > 0)
	{
		src_rect.x = 0;
		src_rect.y = 0;
		src_rect.w = scaled_width_;
		src_rect.h = scaled_height_;
		dst_rect = &src_rect;
	}
	ret = SDL_RenderCopy(renderer_, texture_, nullptr, dst_rect);
	if (ret != 0)
	{
		std::cout << "SDL_RenderCopy failed : " << SDL_GetError() << std::endl;
		return -1;
	}
	SDL_RenderPresent(renderer_);

	return 0;
}

int SDLRenderer::renderYuvPlanarData(const uint8_t* y_buffer, int y_size, const uint8_t* u_buffer, int u_size, const uint8_t* v_buffer, int v_size)
{
	if (!y_buffer || y_size <= 0 || !u_buffer || u_size <= 0 || !v_buffer || v_size <= 0)
	{
		std::cout << "DrawView failed : (!y_buffer || y_size <= 0 || !u_buffer || u_size || !v_buffer || v_size)" << std::endl;
		return -1;
	}

	std::unique_lock<std::mutex> lock(mtx_);

	if (!window_ || !texture_ || !renderer_)
	{
		std::cout << "DrawView failed : (!window_ || !texture_ || !renderer_)" << std::endl;
		return -1;
	}

	if (width_ == 0 || height_ == 0)
	{
		std::cout << "DrawView failed : (width_ == 0 || height_ == 0)" << std::endl;
		return -1;
	}

	SDL_RenderClear(renderer_);

	int ret = SDL_UpdateYUVTexture(texture_, nullptr, y_buffer, y_size, u_buffer, u_size, v_buffer, v_size);
	if (ret != 0)
	{
		std::cout << "SDL_UpdateYUVTexture failed : " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Rect src_rect;
	SDL_Rect* dst_rect = nullptr;
	if (scaled_width_ > 0 && scaled_height_ > 0)
	{
		src_rect.x = 0;
		src_rect.y = 0;
		src_rect.w = scaled_width_;
		src_rect.h = scaled_height_;
		dst_rect = &src_rect;
	}
	ret = SDL_RenderCopy(renderer_, texture_, nullptr, dst_rect);
	if (ret != 0)
	{
		std::cout << "SDL_RenderCopy failed : " << SDL_GetError() << std::endl;
		return -1;
	}
	SDL_RenderPresent(renderer_);

	return 0;
}

void SDLRenderer::destroyRenderer()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (texture_)
	{
		SDL_DestroyTexture(texture_);
	}

	if (renderer_)
	{
		SDL_DestroyRenderer(renderer_);
	}

	if (window_)
	{
		SDL_DestroyWindow(window_);
	}
	texture_ = nullptr;
	renderer_ = nullptr;
	window_ = nullptr;
}

bool SDLRenderer::isRendererExit()
{
	SDL_Event ev;
	SDL_WaitEventTimeout(&ev, 1);
	if (ev.type == SDL_QUIT)
	{
		return true;
	}
	return false;
}

int SDLRenderer::initSdlLibrary()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!is_lib_init_)
	{
		int ret = SDL_Init(SDL_INIT_VIDEO);
		if (ret != 0)
		{
			std::cout << "SDL_Init failed" << SDL_GetError() << std::endl;
			return -1;
		}

		// Linear scale
		ret = SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
		if (ret != SDL_TRUE)
		{
			std::cout << "SDL_SetHint failed" << SDL_GetError() << std::endl;
			return -1;
		}

		is_lib_init_ = true;
	}
	return 0;
}

void SDLRenderer::resetRenderer()
{
	//std::unique_lock<std::mutex> lock(mtx_);
	if (texture_)
	{
		SDL_DestroyTexture(texture_);
	}

	if (renderer_)
	{
		SDL_DestroyRenderer(renderer_);
	}

	texture_ = nullptr;
	renderer_ = nullptr;
}
