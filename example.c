#include "linguini.h"

int main(void){
	linguini_PixelArray canvas;
	linguini_createPixarr(&canvas, 800, 450);

	linguini_clearPixarr(&canvas, 0x202020FF);
	linguini_fillRect(&canvas, 10, 10, 100, 100, 0xFF202099);
	linguini_fillRect(&canvas, 90, 90, 100, 100, 0x2020FF99);

	linguini_SDLContext sdlContext;
	linguini_useSDL(&sdlContext, &canvas, "Linguini SDL Test");

	SDL_Event e;

	linguini_startClock();
	int running = 1;
	while(running){
		while(linguini_SDLPollEvents(&e)){
			if(e.type == SDL_QUIT){
				running = 0;
			}
		}

		linguini_toSDL(&canvas, &sdlContext);

		linguini_limitFPS(60);
	}

	linguini_closeSDL(&sdlContext);
	return 0;
}
