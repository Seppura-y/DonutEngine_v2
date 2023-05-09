//#ifndef VLC_PLAYER_H
//#define VLC_PLAYER_H
//
//#include <vlc/vlc.h>
//#include <vlc/libvlc.h>
//
//#include "ivideo_player.h"
//
//class VLCPlayer : public IVideoPlayer
//{
//	Q_OBJECT
//
//public:
//	VLCPlayer();
//	~VLCPlayer();
//
//	libvlc_media_player_t* getMediaPlayer();
//	libvlc_time_t getTotalDuration();
//
//	int initPlayer();
//	void updateTimePos(QTime value);
//	void updateTotalDuration(QTime value);
//	void updateSoundVolume(int value);
//	void mediaEndReached();
//
//	int openMediaFile(QString file_path, void* hwnd);
//	void setPlaying();
//	void setPause();
//	void setStop();
//
//	void setTimePos(double value);
//	void setSoundVolume(int value);
//
//protected:
//
//private:
//	libvlc_instance_t* vlc_instance_ = nullptr;
//	libvlc_media_player_t* vlc_media_player_ = nullptr;
//	libvlc_media_t* vlc_media_ = nullptr;
//	libvlc_event_manager_t* vlc_event_manager_ = nullptr;
//
//	// seconds
//	libvlc_time_t total_duration_ = -1;
//};
//
//#endif