//#include "vlc_player.h"
//
//#include <iostream>
//
//#include <QTime>
//
//void vlc_callback(const struct libvlc_event_t* p_event, void* p_data)
//{
//    VLCPlayer* manager = static_cast<VLCPlayer*>(p_data);
//
//    if (manager)
//    {
//        switch (p_event->type)
//        {
//        case libvlc_MediaPlayerPositionChanged:
//        {
//            float pos = libvlc_media_player_get_position(manager->getMediaPlayer());
//            //manager->updateTimePos(pos * 100);
//
//            qint64 sec = libvlc_media_player_get_time(manager->getMediaPlayer()) / 1000;
//
//            int cur_hour = sec / 3600;
//            int cur_minutes = (sec - cur_hour * 3600) / 60;
//            int cur_sec = sec - cur_hour * 3600 - cur_minutes * 60;
//
//            QTime time(cur_hour, cur_minutes, cur_sec);
//
//            manager->updateTimePos(time);
//            break;
//        }
//        
//        case libvlc_MediaPlayerAudioVolume:
//        {
//            int volume = libvlc_audio_get_volume(manager->getMediaPlayer());
//            manager->updateSoundVolume(volume);
//            break;
//        }
//
//        case libvlc_MediaPlayerEndReached:
//        {
//            manager->setStop();
//            manager->mediaEndReached();
//            break;
//        }
//
//        case libvlc_MediaPlayerStopped:
//        {
//            manager->setStop();
//            manager->mediaEndReached();
//            break;
//        }
//        }
//    }
//}
//VLCPlayer::VLCPlayer()
//{
//}
//
//VLCPlayer::~VLCPlayer()
//{
//    if (vlc_media_player_)
//    {
//        libvlc_media_player_release(vlc_media_player_);
//        vlc_media_player_ = nullptr;
//    }
//
//    if (vlc_instance_)
//    {
//        libvlc_release(vlc_instance_);
//        vlc_instance_ = nullptr;
//    }
//}
//
//libvlc_media_player_t* VLCPlayer::getMediaPlayer()
//{
//    return vlc_media_player_;
//}
//
//libvlc_time_t VLCPlayer::getTotalDuration()
//{
//    return total_duration_;
//}
//
//int VLCPlayer::initPlayer()
//{
//    // vlc≥ı ºªØ
//    vlc_instance_ = libvlc_new(0, nullptr);
//
//    if (vlc_instance_)
//    {
//        vlc_media_player_ = libvlc_media_player_new(vlc_instance_);
//
//        if (vlc_media_player_)
//        {
//            vlc_event_manager_ = libvlc_media_player_event_manager(vlc_media_player_);
//
//            libvlc_event_attach(vlc_event_manager_, libvlc_MediaPlayerPositionChanged, vlc_callback, this);
//
//            libvlc_event_attach(vlc_event_manager_, libvlc_MediaPlayerAudioVolume, vlc_callback, this);
//        }
//        else
//        {
//            libvlc_release(vlc_instance_);
//            return -2;
//        }
//    }
//    else
//    {
//        return -1;
//    }
//    return 0;
//}
//
//void VLCPlayer::updateTimePos(QTime value)
//{
//    emit sigUpdateTimePos(value);
//}
//
//void VLCPlayer::updateSoundVolume(int value)
//{
//    emit sigUpdateSoundVolume(value);
//}
//
//void VLCPlayer::mediaEndReached()
//{
//    emit sigMediaEndReached();
//}
//
//void VLCPlayer::updateTotalDuration(QTime value)
//{
//    emit sigUpdateTotalDuration(value);
//}
//
//int VLCPlayer::openMediaFile(QString file_path, void* hwnd)
//{
//    //QString path = QDir::toNativeSeparators(file_path);
//
//    // set path
//    vlc_media_ = libvlc_media_new_path(vlc_instance_, file_path.toStdString().c_str());
//
//    if (vlc_media_)
//    {
//        // parse media
//        libvlc_media_parse(vlc_media_);
//
//        // set media
//        libvlc_media_player_set_media(vlc_media_player_, vlc_media_);
//
//        // set the window's handle on which the media is played
//        libvlc_media_player_set_hwnd(vlc_media_player_, hwnd);
//
//        // set total duration
//        total_duration_ = libvlc_media_get_duration(vlc_media_) / 1000;
//        qint64 sec = total_duration_;
//        int cur_hour = sec / 3600;
//        int cur_minutes = (sec - cur_hour * 3600) / 60;
//        int cur_sec = sec - cur_hour * 3600 - cur_minutes * 60;
//
//        QTime time(cur_hour, cur_minutes, cur_sec);
//        emit sigUpdateTotalDuration(time);
//
//        // release media
//        libvlc_media_release(vlc_media_);
//        vlc_media_ = nullptr;
//
//        // play 
//        libvlc_media_player_play(vlc_media_player_);
//
//        return 0;
//    }
//    else
//    {
//        if (vlc_media_player_)
//        {
//            libvlc_media_player_release(vlc_media_player_);
//            vlc_media_player_ = nullptr;
//        }
//
//        if (vlc_instance_)
//        {
//            libvlc_release(vlc_instance_);
//            vlc_instance_ = nullptr;
//        }
//        return -1;
//    }
//}
//
//void VLCPlayer::setPlaying()
//{
//    if (libvlc_media_player_get_state(vlc_media_player_) == libvlc_state_t::libvlc_Paused)
//    {
//        libvlc_media_player_play(vlc_media_player_);
//    }
//}
//
//void VLCPlayer::setPause()
//{
//    if (libvlc_media_player_get_state(vlc_media_player_) == libvlc_state_t::libvlc_Playing)
//    {
//        libvlc_media_player_pause(vlc_media_player_);
//    }
//}
//
//void VLCPlayer::setStop()
//{
//    if (libvlc_media_player_get_state(vlc_media_player_) == libvlc_state_t::libvlc_Playing ||
//        libvlc_media_player_get_state(vlc_media_player_) == libvlc_state_t::libvlc_Paused)
//    {
//        libvlc_media_player_stop(vlc_media_player_);
//    }
//}
//
//void VLCPlayer::setTimePos(double value)
//{
//    if (!vlc_media_player_) return;
//    libvlc_media_player_set_position(vlc_media_player_, value / total_duration_);
//    //libvlc_media_player_set_position(vlc_media_player_, value / 100.0);
//}
//
//void VLCPlayer::setSoundVolume(int value)
//{
//    if (!vlc_media_player_) return;
//    libvlc_audio_set_volume(vlc_media_player_, value);
//}
