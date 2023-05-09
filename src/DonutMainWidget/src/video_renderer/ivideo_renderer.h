#ifndef IVIDEO_RENDERER_H
#define IVIDEO_RENDERER_H

#include <mutex>

#include "media_types.h"

void sleepForMicroSecs(unsigned int ms);

//enum class PixFormat
//{
//	PIX_FORMAT_YUV420P = 0,
//	PIX_FORMAT_ARGB = 25,
//	PIX_FORMAT_RGBA = 26,
//	PIX_FORMAT_BGRA = 28
//};
//
//enum class RenderType
//{
//	RENDER_TYPE_SDL = 0,
//	RENDER_TYPE_OPENGL
//};

struct AVFrame;
struct AVCodecParameters;

class IVideoRenderer
{
public:
	static IVideoRenderer* createRenderer(RenderType type = RenderType::RENDER_TYPE_SDL);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="fmt"></param>
	/// <param name="win_id"></param>
	/// <returns></returns>
	virtual int initRenderer(int width, int height, PixFormat fmt = PixFormat::PIX_FORMAT_RGBA, void* win_id = nullptr) = 0;
	virtual int initRenderer(AVCodecParameters* param);
	virtual void destroyRenderer() = 0;
	virtual bool isRendererExit() = 0;

	/// <summary>
	/// if linesize <=0, calculate it depends on width and pixel format
	/// </summary>
	/// <param name="data"></param>
	/// <param name="linesize"></param>
	/// <returns></returns>
	virtual int renderPackedData(const uint8_t* data, int linesize = 0) = 0;
	virtual int renderYuvPlanarData(const uint8_t* y_buffer, int y_size, const uint8_t* u_buffer, int u_size, const uint8_t* v_buffer, int v_size) = 0;
	virtual int renderFrame(AVFrame* frame);

	int get_render_fps() { return render_fps_; }
protected:

protected:
	std::mutex mtx_;
	int width_ = 0;
	int height_ = 0;
	int scaled_width_ = 0;
	int scaled_height_ = 0;
	void* window_id_ = nullptr;
	PixFormat pix_fmt_ = PixFormat::PIX_FORMAT_YUV420P;

	int render_fps_ = 0;
	int render_count_ = 0;
	long long begin_ms_ = 0;
};

#endif