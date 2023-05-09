#ifndef SDL_RENDERER_H
#define SDL_RENDERER_H

#include "ivideo_renderer.h"

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class SDLRenderer : public IVideoRenderer
{
public:
	virtual int initRenderer(int width, int height, PixFormat fmt, void* win_id) override;
	virtual void destroyRenderer() override;
	virtual bool isRendererExit() override;


	virtual int renderPackedData(const uint8_t* data, int linesize = 0) override;
	virtual int renderYuvPlanarData(const uint8_t* y_buffer, int y_size, const uint8_t* u_buffer, int u_size, const uint8_t* v_buffer, int v_size) override;
protected:
	int initSdlLibrary();
	void resetRenderer();
protected:

private:
	bool is_lib_init_ = false;
	SDL_Window* window_ = nullptr;
	SDL_Texture* texture_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;
};

#endif