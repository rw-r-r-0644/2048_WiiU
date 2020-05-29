#include "Font.h"
#include <stdio.h>
#include "SDLUtils.h"
#include <coreinit/memory.h>

void Font::init(int fontSize)
{
	free();

	const void *ttfFontStandard = nullptr;
	uint32_t ttfFontStandardSize = 0;
	OSGetSharedData(OS_SHAREDDATATYPE_FONT_STANDARD, 0, (void**)&ttfFontStandard, &ttfFontStandardSize);

	SDL_RWops *m_fontrw = SDL_RWFromConstMem(ttfFontStandard, ttfFontStandardSize);
	if(!m_fontrw) {
		printf("Failed to create SDL_RWops for font!\n");
		return;
	}
	m_font = TTF_OpenFontRW(m_fontrw, 1, fontSize);
	if(!m_font) {
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
	}
}

void Font::free()
{
	if(m_font) {
		TTF_CloseFont(m_font);
		m_font = NULL;
	}
}

SDL_Surface *Font::renderLatin(const char *str, SDL_Color fgColor)
{
	return TTF_RenderText_Blended(m_font, str, fgColor);
}
