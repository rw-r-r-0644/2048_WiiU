#ifndef SdlUtils_h
#define SdlUtils_h

#include <functional>
#include <SDL.h>
#include <SDL_ttf.h>
#include "Texture.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern bool InitSDL();
extern void CloseSDL();

extern const SDL_Color BLACK;
extern const SDL_Color WHITE;

extern SDL_Color Color_Mix(const SDL_Color &color1, const SDL_Color &color2, double weight);

#endif
