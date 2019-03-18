#include "SDLUtils.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Render.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

static SDL_Window *g_window = NULL;
static SDL_Renderer *g_renderer = NULL;

bool InitSDL()
{
	bool success = true;
	
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!\n");
		}

		g_window = SDL_CreateWindow(
			"2048",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
		if(g_window == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if(g_renderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			}
			g_render.init(g_renderer);

			int imgFlags = IMG_INIT_PNG;
			if(!(IMG_Init(imgFlags) & imgFlags)) {
				printf("SDL_image could not initialize! SDL Error: %s\n", SDL_GetError());
				success = false;
			}

			if(TTF_Init() == -1) {
				printf("SDL_ttf could not initialize! TTF Error: %s\n", TTF_GetError());
				success = false;
			}

			for (int i = 0; i < SDL_NumJoysticks(); i++) {
				if (SDL_JoystickOpen(i) == NULL) {
					printf("Failed to open joystick %s!\n", i);
					success = false;
				}
			}
		}
	}

	return success;
}

void CloseSDL()
{
	SDL_DestroyRenderer(g_renderer);
	g_renderer = NULL;
	SDL_DestroyWindow(g_window);
	g_window = NULL;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

const SDL_Color BLACK = { 0, 0, 0, 0 };
const SDL_Color WHITE = { 255, 255, 255, 255 };

SDL_Color Color_Mix(const SDL_Color &color1, const SDL_Color &color2, double weight)
{
	double p = weight;
	double w = p * 2 - 1;
	double a = color1.a / 255.0 - color2.a / 255.0;

	double w1 = ((w * a == -1 ? w : (w + a) / (1 + w * a)) + 1) / 2.0;
	double w2 = 1 - w1;

	SDL_Color result;
	result.r = static_cast<Uint8>(color1.r * w1 + color2.r * w2);
	result.g = static_cast<Uint8>(color1.g * w1 + color2.g * w2);
	result.b = static_cast<Uint8>(color1.b * w1 + color2.b * w2);
	result.a = static_cast<Uint8>(color1.a * w1 + color2.a * w2);

	return result;
}
