#include "donut_player.h"

#include "av_demux_handler.h"
#include "av_video_decode_handler.h"

#include <QTime>


DonutPlayer::DonutPlayer()
{
}

DonutPlayer::~DonutPlayer()
{
}

int DonutPlayer::initPlayer()
{
    return 0;
}

void DonutPlayer::updateTimePos(QTime value)
{
}

void DonutPlayer::updateTotalDuration(QTime value)
{
}

void DonutPlayer::updateSoundVolume(int value)
{
}

void DonutPlayer::mediaEndReached()
{
}

int DonutPlayer::openMediaFile(QString file_path, void* hwnd)
{
    return 0;
}

void DonutPlayer::setPlaying()
{
}

void DonutPlayer::setPause()
{
}

void DonutPlayer::setStop()
{
}

void DonutPlayer::setTimePos(double value)
{
}

void DonutPlayer::setSoundVolume(int value)
{
}
