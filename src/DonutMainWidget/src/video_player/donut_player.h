#ifndef DONUT_PLAYER_H
#define DONUT_PLAYER_H

#include "ivideo_player.h"

class AVDemuxHandler;
class AVVideoDecodeHandler;

class DonutPlayer : public IVideoPlayer
{
	Q_OBJECT

public:
	DonutPlayer();
	~DonutPlayer();

	virtual int initPlayer() override;
	virtual void updateTimePos(QTime value) override;
	virtual void updateTotalDuration(QTime value) override;
	virtual void updateSoundVolume(int value) override;
	virtual void mediaEndReached() override;

	virtual int openMediaFile(QString file_path, void* hwnd) override;
	virtual void setPlaying() override;
	virtual void setPause() override;
	virtual void setStop() override;

	virtual void setTimePos(double value) override;
	virtual void setSoundVolume(int value) override;

protected:
	AVDemuxHandler* demux_handler_ = nullptr;
	AVVideoDecodeHandler* video_decode_handler_ = nullptr;

private:

};

#endif