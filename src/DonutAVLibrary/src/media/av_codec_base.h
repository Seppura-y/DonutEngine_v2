#ifndef AV_CODEC_BASE_H
#define AV_CODEC_BASE_H


#include <mutex>

#include "core.h"
#include "av_data_tools.h"

struct AVCodec;
struct AVCodecContext;
struct AVDictionary;
struct AVCodecParameters;

namespace Donut
{
	class DONUT_API AVCodecBase
	{
	public:
		static AVCodecContext* createContext(int codec_id, bool is_decode);

		AVCodecBase();
		virtual ~AVCodecBase();

		void setCodecContext(AVCodecContext* codec_ctx);

		virtual int openContext() = 0;
		virtual int flushCodec() = 0;


		int setOption(const char* key, const char* value);
		int setOption(const char* key, const int value);
		int setOption(const AVDictionary* dict);

		virtual AVCodecContext* get_codec_ctx();
		std::shared_ptr<AVParamWarpper> copyCodecParam();
		std::shared_ptr<AVCodecParameters> copyCodecParameter();

		int getCodecExtraData(uint8_t* buffer, int& size);

	protected:
		std::mutex mtx_;
		AVCodecContext* codec_ctx_ = nullptr;
		AVDictionary* options_ = nullptr;

	private:

	};

}

#endif