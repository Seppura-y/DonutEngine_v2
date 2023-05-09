#include "iav_base_handler.h"
#include <iostream>
#include <sstream>

extern"C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#include "log.h"

namespace Donut
{
void IAVBaseHandler::start()
{
	static int i = 0;
	std::unique_lock<std::mutex> lock(mtx_);

	this->thread_index_ = i++;
	is_exit_ = false;

	worker_ = std::thread(&IAVBaseHandler::threadLoop, this);
	//std::cout << "thread "<< std::this_thread::get_id() <<" : start" << std::endl;

	std::stringstream stream;
	stream << "thread " << std::this_thread::get_id() << " : start";
	DN_CORE_INFO(stream.str());
}


void IAVBaseHandler::stop()
{
	//unique_lock<mutex> lock(mtx_);
	std::stringstream stream;
	stream << "thread " << std::this_thread::get_id() << " : request stop";
	DN_CORE_INFO(stream.str());
	//std::cout << "thread " << std::this_thread::get_id() << " : request stop" << std::endl;
	is_exit_ = true;
	if (worker_.joinable())
	{
		worker_.join();
	}
	stream.clear();
	stream << "thread " << std::this_thread::get_id() << " : stop";
	DN_CORE_INFO(stream.str());

	//std::cout << "thread " << std::this_thread::get_id() << " : stop" << std::endl;
}

void IAVBaseHandler::pause()
{
	std::unique_lock<std::mutex> lock(mtx_);
	this->is_pause_ = !this->is_pause_;
}


void IAVBaseHandler::setNextHandler(IAVBaseHandler* node)
{
	std::unique_lock<std::mutex> lock(mtx_);
	this->next_ = node;
}


IAVBaseHandler* IAVBaseHandler::getNextHandler()
{
	std::unique_lock<std::mutex> lock(mtx_);
	return next_;
}


void IAVBaseHandler::setVideoCallbackEnable(bool status)
{
	std::unique_lock<std::mutex> lock(mtx_);
	is_video_callback_enabled_ = status;
}

void IAVBaseHandler::setAudioCallbackEnable(bool status)
{
	std::unique_lock<std::mutex> lock(mtx_);
	is_audio_callback_enabled_ = status;
}

void IAVBaseHandler::setAudioCallback(std::function<void(AVPacket*)> audio_callback)
{
	std::unique_lock<std::mutex> lock(mtx_);
	audio_callback_ = audio_callback;
}
void IAVBaseHandler::setVideoCallback(std::function<void(AVPacket*)> video_callback)
{
	std::unique_lock<std::mutex> lock(mtx_);
	video_callback_ = video_callback;
}

int64_t IAVBaseHandler::scaleToMsec(int64_t duration, AVRational src_timebase)
{
	AVRational dst = { 1,1000 };
	return av_rescale_q(duration, src_timebase, dst);
}
}
