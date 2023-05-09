#ifndef AV_VIDEO_SCALER_H
#define AV_VIDEO_SCALER_H
#include <mutex>

#include "core.h"

struct SwsContext;
struct AVFrame;

namespace Donut
{
	class DONUT_API AVVideoScaler
	{
	public:
		AVVideoScaler();
		virtual ~AVVideoScaler();

		int initScaler(int in_fmt, int out_fmt);
		int initScaler(int input_width, int input_height, int input_fmt, int output_width, int output_height, int output_fmt);

		void setInputParam(int input_width, int input_height, int input_fmt);
		void setoutputParam(int output_width, int output_height, int output_fmt);
		AVFrame* getScaledFrame(void* data, int* linesize);
		int getScaledFrame(AVFrame* input, AVFrame* output);
		int scaleRawData(const uint8_t* const* src, const int* src_linesize, uint8_t* const* dst, const int* dst_linesize);
	protected:

	private:
		int input_width_ = -1;
		int input_height_ = -1;
		int input_fmt_ = -1;
		
		int output_width_ = -1;
		int output_height_ = -1;
		int output_fmt_ = -1;

		int* input_linesize_ = nullptr;
		int* output_linesize_ = nullptr;
		SwsContext* sws_ctx_ = nullptr;

		std::mutex mtx_;
	};

}

#endif