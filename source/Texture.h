#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL.h>
#include <string>
#include "SDLUtils.h"
#include "Font.h"

class Texture
{
public:
	Texture();
	~Texture();

	bool loadFromFile(std::string path);
	bool loadFromLatinText(Font *font, const char *text, SDL_Color fgColor);
	bool createBlank(int width, int height, SDL_TextureAccess access);
	void free();

	void setBlendMode(SDL_BlendMode blending);
	void enableAlpha() { setBlendMode(SDL_BLENDMODE_BLEND); }
	void setAlpha(Uint8 alpha);

	void render(int x, int y, SDL_Rect *clip = NULL);
	void renderScaled(int x, int y, double ratio);

	SDL_Texture *sdl_texture() { return m_texture; }
	int width() { return m_width; }
	int height() { return m_height; }

private:
	SDL_Texture *m_texture;

	int m_width;
	int m_height;
};

#endif
