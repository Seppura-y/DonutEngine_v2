#ifndef MEDIA_TYPES_H
#define MEDIA_TYPES_H

enum class PixFormat
{
    PIX_FORMAT_YUV420P = 0,
    PIX_FORMAT_NV12 = 23,
    PIX_FORMAT_ARGB = 25,
    PIX_FORMAT_RGBA = 26,
    PIX_FORMAT_BGRA = 28
};

enum class RenderType
{
    RENDER_TYPE_SDL = 0,
    RENDER_TYPE_OPENGL
};

enum class PlayerType
{
    PLAYER_TYPE_DONUT = 0,
    PLAYER_TYPE_VLC
};

#endif