#ifndef AV_DEMUXER_H
#define AV_DEMUXER_H

#include "core.h"
#include "av_format_base.h"


struct AVPacket;
struct AVStream;

namespace Donut
{

	class DONUT_API AVDemuxer : public AVFormatBase
	{
	public:
		AVFormatContext* openContext(const char* url);
		int readPacket(AVPacket* pkt);

		int seekByPts(long long pts, int stream_index);


	};
}

#endif
