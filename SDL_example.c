#include "linguini.h"

int main(void){
	linguini_PixelArray canvas;
	linguini_createPixarr(&canvas, 800, 450);

	linguini_SDLContext sdlContext;
	linguini_useSDL(&sdlContext, &canvas, "Linguini SDL Test");

	SDL_Event e;

	int dx = 4;
	int dy = 4;

	int x = 100;
	int y = 100;

	linguini_startClock();
	while(sdlContext.isOpen){
		while(linguini_SDLPollEvents(&e)){
			switch(e.type){
				case SDL_QUIT:
				{
					sdlContext.isOpen = 0;
				}
				default: break;
			}
		}
		
		linguini_clearPixarr(&canvas, 0x202020FF);

		linguini_fillRect(&canvas, x, y, 10, 10, 0x20FF20FF);

		if(x+dx >= 0 && x+10+dx < canvas.width){
			x += dx;
		}
		else{
			dx = -dx;
		}

		if(y+dy >= 0 && y+10+dy < canvas.height){
			y += dy;
		}
		else{
			dy = -dy;
		}

		linguini_toSDL(&canvas, &sdlContext);

		linguini_limitFPS(60);
	}

	linguini_closeSDL(&sdlContext);
	return 0;
}
