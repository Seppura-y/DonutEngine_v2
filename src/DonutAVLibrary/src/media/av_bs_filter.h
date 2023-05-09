#ifndef AV_BS_FILTER_H
#define AV_BS_FILTER_H

#include "core.h"

#include <mutex>
struct AVBSFContext;
struct AVCodecParameters;
struct AVPacket;
struct AVFrame;

namespace Donut
{
	class DONUT_API AVBSFilter
	{
	public:
		AVBSFilter();
		~AVBSFilter();
		static AVBSFContext* createBSFContext(AVCodecParameters* param);
		int setBSFContext(AVBSFContext* ctx);
		int sendPacket(AVPacket* pkt);
		int recvPacket(AVPacket* pkt);

	protected:
		std::mutex mtx_;
	private:
		AVBSFContext* bsf_ctx_ = nullptr;
	};

}

#endif
