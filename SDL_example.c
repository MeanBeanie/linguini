#include "linguini.h"

int main(void){
	linguini_PixelArray canvas;
	linguini_createPixarr(&canvas, 800, 450);

	linguini_SDLContext sdlContext;
	linguini_useSDL(&sdlContext, &canvas, "Linguini SDL Test");

	linguini_freetypeContext freetypeContext;
	linguini_useFreetype(&freetypeContext, "/home/shucai/.local/share/fonts/DepartureMonoNerdFont-Regular.otf", 32);

	SDL_Event e;

	int dx = 4;
	int dy = 4;

	linguini_Rect ball;
	ball.x = 100;
	ball.y = 100;
	ball.w = 10;
	ball.h = 10;

	linguini_Rect paddle1;
	paddle1.x = 10;
	paddle1.y = 200;
	paddle1.w = 10;
	paddle1.h = 50;

	int score = 0;

	linguini_startClock();
	while(sdlContext.isOpen){
		while(linguini_SDLPollEvents(&e)){
			switch(e.type){
				case SDL_QUIT:
				{
					sdlContext.isOpen = 0;
				}
				case SDL_KEYDOWN:
				{
					if(e.key.state == SDL_PRESSED){
						switch(e.key.keysym.sym){
							case SDLK_UP:
							{
								paddle1.y -= 20;
								if(paddle1.y < 0){
									paddle1.y = 0;
								}
								break;
							}
							case SDLK_DOWN:
							{
								paddle1.y += 20;
								if(paddle1.y+paddle1.h > 450){
									paddle1.y = 450-paddle1.h;
								}
								break;
							}
							default: break;
						}
					}
				}
				default: break;
			}
		}
		
		linguini_clearPixarr(&canvas, 0x202020FF);

		linguini_fillRect(&canvas, ball, 0x20FF20FF);
		linguini_fillRect(&canvas, paddle1, 0xCFCFCFFF);
		linguini_drawText(&canvas, &freetypeContext, 100, 200, 0xCFCFCFFF, "Score %i", score);

		if(ball.x+dx >= 0 && ball.x+10+dx < canvas.width){
			ball.x += dx;
		}
		else{
			if(ball.x+dx < 0){
				score++;
			}
			dx = -dx;
		}

		if(ball.y+dy >= 0 && ball.y+10+dy < canvas.height){
			ball.y += dy;
		}
		else{
			dy = -dy;
		}

		if(linguini_rectInRect(ball, paddle1)){
			dx = -dx;
			ball.x += ball.w+1;
		}

		linguini_toSDL(&canvas, &sdlContext);

		linguini_limitFPS(60);
	}

	linguini_closeSDL(&sdlContext);
	return 0;
}
