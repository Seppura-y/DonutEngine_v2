#ifndef AV_MUX_HANDLER_H
#define AV_MUX_HANDLER_H

#include "iav_base_handler.h"
#include "av_muxer.h"


namespace Donut
{
class DONUT_API AVMuxHandler : public IAVBaseHandler
{
public:
	AVMuxHandler();
	~AVMuxHandler();

	int openAVMuxer(const char* url,
		AVCodecParameters* video_param,
		AVRational* video_timebase,
		AVCodecParameters* audio_param,
		AVRational* audio_timebase,
		AVMuxType type = AVMuxType::AV_MUX_TYPE_FILE,
		uint8_t* extra_data = nullptr,
		int extra_data_size = 0
	);

	int resetMuxer();
	virtual void threadLoop() override;
	virtual void handle(void* package) override;

	std::shared_ptr<AVParamWarpper> copyVideoParameters();
	std::shared_ptr<AVParamWarpper> copyAudioParameters();

private:
	AVMuxer muxer_;

	std::string url_;
	AVMuxType mux_type_ = AVMuxType::AV_MUX_TYPE_FILE;

	int video_index_ = -1;
	int audio_index_ = -1;

	bool has_video_ = false;
	bool has_audio_ = false;

	AVParamWarpper* audio_params_ = nullptr;
	AVParamWarpper* video_params_ = nullptr;

	uint8_t* extradata_ = nullptr;
	int extradata_size_ = 0;

	AVPacketDataList pkt_list_;

};

}


#endif