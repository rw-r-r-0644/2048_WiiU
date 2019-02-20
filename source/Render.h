#ifndef Render_h
#define Render_h

#include <SDL.h>

/***
 * we need an abstract class/interface RenderTarget
 */

class Render
{
public:
	void init(SDL_Renderer *renderer);

	void setDrawColor(int r, int g, int b, int a = 255);
	void setDrawColor(const SDL_Color &color);
	void clear();
	void drawRect(int x, int y, int w, int h);
	void fillRect(int x, int y, int w, int h);
	void drawLine(int x1, int y1, int x2, int y2);
	void present();

	SDL_Texture *createTexture(Uint32 format, int access, int width, int height);
	SDL_Texture *createTextureFromSurface(SDL_Surface *surface);
	int copy(SDL_Texture *texture, const SDL_Rect *srcRect, const SDL_Rect *dstRect);

	void setTarget(SDL_Texture *texture);
private:
	SDL_Renderer *m_renderer;
};

extern Render g_render;

#endif
