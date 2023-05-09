#ifndef AV_DEMUX_HANDLER_H
#define AV_DEMUX_HANDLER_H

#include "iav_base_handler.h"
#include "av_demuxer.h"

namespace Donut
{
class DONUT_API AVDemuxHandler : public IAVBaseHandler
{
public:
	AVDemuxHandler();
	~AVDemuxHandler();
	int openAVSource(const char* url, int timeout = 1000);

	virtual void threadLoop() override;
	virtual void handle(void* package) override;

	std::shared_ptr<AVParamWarpper> copyVideoParameters();
	std::shared_ptr<AVParamWarpper> copyAudioParameters();

	bool isHasVideo() { return has_video_; }
	bool isHasAudio() { return has_audio_; }

	void setVideoHandler(IAVBaseHandler* handler);
	void setAudioHandler(IAVBaseHandler* handler);
private:
	AVDemuxer demuxer_;
	std::string url_;
	int timeout_threshold_ = 0; // ms

	bool has_video_ = false;
	bool has_audio_ = false;
	int video_index_ = -1;
	int audio_index_ = -1;

	IAVBaseHandler* video_handler_ = nullptr;
	IAVBaseHandler* audio_handler_ = nullptr;
};

}

#endif