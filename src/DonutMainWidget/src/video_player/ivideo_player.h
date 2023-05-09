#ifndef IVIDEO_PLAYER_H
#define IVIDEO_PLAYER_H

#include <QObject>

#include "media_types.h"

class IVideoPlayer : public QObject
{
	Q_OBJECT

public:
	static IVideoPlayer* createVideoPlayer(PlayerType type = PlayerType::PLAYER_TYPE_DONUT);

	virtual int initPlayer() = 0;
	virtual void updateTimePos(QTime value) = 0;
	virtual void updateTotalDuration(QTime value) = 0;
	virtual void updateSoundVolume(int value) = 0;
	virtual void mediaEndReached() = 0;

	virtual int openMediaFile(QString file_path, void* hwnd = nullptr) = 0;
	virtual void setPlaying() = 0;
	virtual void setPause() = 0;
	virtual void setStop() = 0;
	
	virtual void setTimePos(double value) = 0;
	virtual void setSoundVolume(int value) = 0;
signals:
	void sigMediaEndReached();
	void sigUpdateTimePos(QTime value);
	void sigUpdateTotalDuration(QTime value);
	void sigUpdateSoundVolume(int value);

protected:

private:

};
#endif