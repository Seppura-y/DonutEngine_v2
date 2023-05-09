#ifndef AV_DATA_LIST_H
#define AV_DATA_LIST_H

extern"C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#include <list>
#include <iostream>
#include <string>

#include <mutex>

#include "core.h"

namespace Donut
{
template<typename T>
class DONUT_API AVDataList
{
public:
	template<typename T>
	void pushData(T* data) {};

	template<typename T>
	T* popFront() { return nullptr; }

	void clearList()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		data_list_.clear();
	}

	int getSize()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		return data_list_.size();
	}
protected:
	std::mutex mtx_;
	std::list<T*> data_list_;

	int max_list_ = 500;
};

template<>
class DONUT_API AVDataList<AVPacket>
{
public:
	void pushData(AVPacket* packet)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		AVPacket* pkt = av_packet_alloc();
		av_packet_ref(pkt, packet);
		data_list_.push_back(pkt);
		if (data_list_.size() > max_list_)
		{
			if (data_list_.front()->flags & AV_PKT_FLAG_KEY)
			{
				AVPacket* tmp = data_list_.front();
				av_packet_unref(tmp);
				av_packet_free(&tmp);
				data_list_.pop_front();
				//return;
			}
			while (!data_list_.empty())
			{
				if (data_list_.front()->flags & AV_PKT_FLAG_KEY)
				{
					return;
				}
				AVPacket* tmp = data_list_.front();
				av_packet_unref(tmp);
				av_packet_free(&tmp);
				data_list_.pop_front();
			}
		}
	}

	AVPacket* popFront()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (data_list_.empty())
		{
			return nullptr;
		}
		AVPacket* pkt = data_list_.front();
		data_list_.pop_front();
		return pkt;
	}

protected:
	std::mutex mtx_;
	std::list<AVPacket*> data_list_;
	int max_list_ = 500;
};
}

#endif