#ifndef Font_h
#define Font_h

#include <SDL.h>
#include <SDL_ttf.h>

class Font
{
public:
	Font() { m_font = NULL; }
	~Font() { free(); }

	void init(int fontSize);
	void free();
	SDL_Surface *renderLatin(const char *str, SDL_Color fgColor);

private:
	TTF_Font *m_font;
};


#endif
