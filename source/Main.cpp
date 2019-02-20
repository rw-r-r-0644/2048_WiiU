#include <stdio.h>
#include <vector>
#include "SDLUtils.h"
#include "Tile.h"
#include "ScoreAddition.h"
#include "Render.h"
#include "Game.h"
#include "Button.h"

#include <romfs-wiiu.h>
#include <algorithm>
#include <unistd.h>
#include <math.h>

Game g;
Button *newGameBtn;

void Frame(int delta_ms)
{
	g_render.setDrawColor(WHITE);
	g_render.clear();

	g.render();
	newGameBtn->render();

	g_render.present();

	g.update(delta_ms);
	newGameBtn->update(delta_ms);
}

int main(int argc, char *argv[])
{
	romfsInit();
	chdir("romfs:/");

	if (!InitSDL())
		return 1;

	Tile_LoadMetaData();
	TileBoard_loadMetaData();
	ScoreAddition_loadMetaData();
	Button_loadMetaData();

	g.init(4);

	newGameBtn = new Button("New Game", [] { g.restart(); });
	newGameBtn->setXY((SCREEN_WIDTH - g.tileBoard()->width()) / 2, 35);

	g.setCopyrightXY(
		(SCREEN_WIDTH - g.copyrightTexture()->width()) / 2,
		(SCREEN_HEIGHT - g.copyrightTexture()->height()));
	g.tileBoard()->setXY((SCREEN_WIDTH - g.tileBoard()->width()) / 2 , 100);
	g.curScoreBoard()->setXY(
		(SCREEN_WIDTH - g.tileBoard()->width()) / 2 + g.tileBoard()->width() - g.curScoreBoard()->width() - g.bestScoreBoard()->width() - 20,
		20);
	g.bestScoreBoard()->setXY(
		(SCREEN_WIDTH - g.tileBoard()->width()) / 2 + g.tileBoard()->width() - g.curScoreBoard()->width(),
		20);

	SDL_Event e;
	Uint32 ticks = SDL_GetTicks();
	for(;;) {
		while(SDL_PollEvent(&e)) {
			newGameBtn->handleEvent(e);

			if(e.type == SDL_JOYBUTTONDOWN) {
				if(e.jbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) g.move(UP);
				if(e.jbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) g.move(RIGHT);
				if(e.jbutton.button == SDL_CONTROLLER_BUTTON_MAX) g.move(DOWN);
				if(e.jbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) g.move(LEFT);
				if(e.jbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP) goto quit;
			} else if(e.type == SDL_QUIT) {
				goto quit;
			} else if(e.type == SDL_KEYDOWN) {
				switch(e.key.keysym.sym) {
				case SDLK_UP:
					g.move(UP);
					break;
				case SDLK_RIGHT:
					g.move(RIGHT);
					break;
				case SDLK_DOWN:
					g.move(DOWN);
					break;
				case SDLK_LEFT:
					g.move(LEFT);
					break;
				}
			}
		}

		Uint32 new_ticks = SDL_GetTicks();
		int delta = new_ticks - ticks;
		ticks = new_ticks;
		Frame(delta);
	}

quit:

	g.quit();

	free(newGameBtn);

	Button_unloadMetaData();
	ScoreAddition_unloadMetaData();
	TileBoard_unloadMetaData();
	Tile_UnloadMetaData();

	CloseSDL();

	romfsExit();

	return 0;
}
