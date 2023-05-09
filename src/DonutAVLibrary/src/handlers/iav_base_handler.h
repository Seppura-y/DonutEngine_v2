#ifndef IAV_BASE_HANDLER_H
#define IAV_BASE_HANDLER_H

#include <thread>
#include <mutex>
#include <functional>

#include "core.h"
#include "av_data_tools.h"

struct AVPacket;

namespace Donut
{
enum class AVHandlerPackageType
{
	AVHANDLER_PACKAGE_TYPE_PACKET = 0,
	AVHANDLER_PACKAGE_TYPE_FRAME
};

enum class AVHandlerPackageAVType
{
	AVHANDLER_PACKAGE_AV_TYPE_AUDIO = 0,
	AVHANDLER_PACKAGE_AV_TYPE_VIDEO
};

typedef struct AVHandlerPackage
{
	AVHandlerPackageAVType av_type_;
	AVHandlerPackageType type_;
	union AVHandlerPackagePayload
	{
		AVPacket* packet_;
		AVFrame* frame_;
	} payload_;
} AVHandlerPackage;


class DONUT_API IAVBaseHandler
{
public:
	IAVBaseHandler() {}
	virtual ~IAVBaseHandler() {}

	virtual void start();
	virtual void stop();
	virtual void pause();
	virtual void handle(void* package) = 0;

	void setNextHandler(IAVBaseHandler* node);

	void setAudioCallback(std::function<void(AVPacket*)> audio_callback_);
	void setVideoCallback(std::function<void(AVPacket*)> video_callback_);
	void setAudioCallbackEnable(bool status);
	void setVideoCallbackEnable(bool status);

	bool isExit() { return is_exit_; };
protected:
	virtual void threadLoop() = 0;
	IAVBaseHandler* getNextHandler();
	int64_t scaleToMsec(int64_t duration, AVRational src_timebase);

protected:
	std::mutex mtx_;
	std::thread worker_;
	std::function<void(AVPacket*)> video_callback_ = nullptr;
	std::function<void(AVPacket*)> audio_callback_ = nullptr;
	bool is_video_callback_enabled_ = false;
	bool is_audio_callback_enabled_ = false;
	IAVBaseHandler* next_ = nullptr;

	bool is_exit_ = true;
	bool is_pause_ = false;
	int thread_index_ = 0;

	int64_t last_proc_time_ = 0;
private:
};

}

#endif