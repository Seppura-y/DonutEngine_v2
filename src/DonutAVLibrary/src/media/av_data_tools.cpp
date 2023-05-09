#include "av_data_tools.h"

extern"C"
{
#include <libavcodec/avcodec.h>
}
#ifdef _WIN32
#include <Windows.h>
#include <sysinfoapi.h>
#endif

namespace Donut
{
int64_t GetCurrentTimeMsec()
{
#ifdef _WIN32
	return (int64_t)GetTickCount();

#else
	return 0;
#endif
}

void SleepForMsec(int ms)
{
	int64_t begin = (int64_t)GetTickCount();
	while (GetTickCount() - begin < ms)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
}

void FreeFrame(AVFrame** frame)
{
	if (!frame)return;
	av_frame_free(frame);
}

AVParamWarpper* AVParamWarpper::create()
{
	AVParamWarpper* para = new AVParamWarpper();
	return para;
}
AVParamWarpper::~AVParamWarpper()
{
	if (para)
	{
		avcodec_parameters_free(&para);
	}
	if (time_base)
	{
		delete time_base;
		time_base = nullptr;
	}
}

AVParamWarpper::AVParamWarpper()
{
	para = avcodec_parameters_alloc();
	time_base = new AVRational();
}


void AVPacketDataList::push(AVPacket* packet)
{
	std::unique_lock<std::mutex> lock(mtx_);
	AVPacket* pkt = av_packet_alloc();
	av_packet_ref(pkt, packet);
	pkt_list_.push_back(pkt);
	if (pkt_list_.size() > max_list_)
	{
		if (pkt_list_.front()->flags & AV_PKT_FLAG_KEY)
		{
			AVPacket* tmp = pkt_list_.front();
			av_packet_unref(tmp);
			av_packet_free(&tmp);
			pkt_list_.pop_front();
			//return;
		}
		while (!pkt_list_.empty())
		{
			if (pkt_list_.front()->flags & AV_PKT_FLAG_KEY)
			{
				return;
			}
			AVPacket* tmp = pkt_list_.front();
			av_packet_unref(tmp);
			av_packet_free(&tmp);
			pkt_list_.pop_front();
		}
	}
}

AVPacket* AVPacketDataList::pop()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (pkt_list_.empty())
	{
		return nullptr;
	}
	AVPacket* pkt = pkt_list_.front();
	pkt_list_.pop_front();
	return pkt;
}

void AVPacketDataList::clear()
{
	std::unique_lock<std::mutex> lock(mtx_);
	while (!pkt_list_.empty())
	{
		AVPacket* tmp = pkt_list_.front();
		av_packet_unref(tmp);
		av_packet_free(&tmp);
		pkt_list_.pop_front();
	}
	return;
}



void AVFrameDataList::push(AVFrame* frm)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (frm_list_.size() > max_list_)
	{
		AVFrame* tmp = frm_list_.front();
		av_frame_unref(tmp);
		av_frame_free(&tmp);
		frm_list_.pop_front();
	}
	AVFrame* frame = av_frame_alloc();
	av_frame_ref(frame, frm);
	frm_list_.push_back(frame);
}

AVFrame* AVFrameDataList::pop()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (frm_list_.empty())
	{
		return nullptr;
	}
	AVFrame* frame = frm_list_.front();
	frm_list_.pop_front();
	return frame;
}

void AVFrameDataList::clear()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!frm_list_.empty())
	{
		AVFrame* tmp = frm_list_.front();
		av_frame_unref(tmp);
		av_frame_free(&tmp);
		frm_list_.pop_front();
	}
}


void AVCachedPacketDataList::push(AVPacket* packet)
{
	std::unique_lock<std::mutex> lock(mtx_);
	AVPacket* pkt = av_packet_alloc();
	av_packet_ref(pkt, packet);
	pkt_list_.push_back(pkt);
	if (pkt_list_.size() > max_list_)
	{
		if (pkt_list_.front()->flags & AV_PKT_FLAG_KEY)
		{
			AVPacket* tmp = pkt_list_.front();
			av_packet_unref(tmp);
			av_packet_free(&tmp);
			pkt_list_.pop_front();
			//return;
		}
		while (!pkt_list_.empty())
		{
			if (pkt_list_.front()->flags & AV_PKT_FLAG_KEY)
			{
				return;
			}
			AVPacket* tmp = pkt_list_.front();
			av_packet_unref(tmp);
			av_packet_free(&tmp);
			pkt_list_.pop_front();
		}
	}
}

AVPacket* AVCachedPacketDataList::pop()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (pkt_list_.empty())
	{
		return nullptr;
	}
	AVPacket* pkt = pkt_list_.front();
	pkt_list_.pop_front();
	return pkt;
}

void AVCachedPacketDataList::clear()
{
	std::unique_lock<std::mutex> lock(mtx_);
	while (!pkt_list_.empty())
	{
		AVPacket* tmp = pkt_list_.front();
		av_packet_unref(tmp);
		av_packet_free(&tmp);
		pkt_list_.pop_front();
	}
	return;
}

int AVCachedPacketDataList::size()
{
	std::unique_lock<std::mutex> lock(mtx_);
	return pkt_list_.size();
}
}

