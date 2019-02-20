#include "Render.h"

Render g_render;

void Render::init(SDL_Renderer *renderer)
{
	m_renderer = renderer; 
}

void Render::setDrawColor(int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
}
void Render::setDrawColor(const SDL_Color &color)
{
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
}

void Render::clear()
{
	SDL_RenderClear(m_renderer);
}

void Render::drawRect(int x, int y, int w, int h)
{
	SDL_Rect rect = { x, y, w, h };
	SDL_RenderDrawRect(m_renderer, &rect);
}

void Render::fillRect(int x, int y, int w, int h)
{
	SDL_Rect rect = { x, y, w, h };
	SDL_RenderFillRect(m_renderer, &rect);
}

void Render::drawLine(int x1, int y1, int x2, int y2)
{
	SDL_RenderDrawLine(m_renderer, x1, y1, x2, y2);
}

void Render::present()
{
	SDL_RenderPresent(m_renderer); 
}

SDL_Texture *Render::createTexture(Uint32 format, int access, int width, int height)
{
	return SDL_CreateTexture(m_renderer, format, access, width, height);
}

SDL_Texture *Render::createTextureFromSurface(SDL_Surface *surface)
{
	return SDL_CreateTextureFromSurface(m_renderer, surface);
}

int Render::copy(SDL_Texture *texture, const SDL_Rect *srcRect, const SDL_Rect *dstRect)
{
	return SDL_RenderCopy(m_renderer, texture, srcRect, dstRect);
}

void Render::setTarget(SDL_Texture *texture)
{
	SDL_SetRenderTarget(m_renderer, texture);
}
