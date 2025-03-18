#include "linguini.h"
#include <SDL2/SDL.h>

void linguini_useSDL(linguini_SDLContext* sdlContext, linguini_PixelArray* canvas, const char* title){
	sdlContext->window = NULL;
	sdlContext->renderer = NULL;
	sdlContext->windowTexture = NULL;
	sdlContext->width = canvas->width;
	sdlContext->height = canvas->height;

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		linguini_log("SDL ERROR", "Failed to initalize SDL with error: %s", SDL_GetError());
		exit(1);
	}

	sdlContext->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, canvas->width, canvas->height, SDL_WINDOW_SHOWN);
	if(sdlContext->window == NULL){
		linguini_log("SDL ERROR", "Failed to create SDL_Window with error: %s", SDL_GetError());
		exit(1);
	}

	sdlContext->renderer = SDL_CreateRenderer(sdlContext->window, -1, SDL_RENDERER_ACCELERATED);
	if(sdlContext->renderer == NULL){
		linguini_log("SDL ERROR", "Failed to create SDL_Renderer with error: %s", SDL_GetError());
		exit(1);
	}

	sdlContext->windowTexture = SDL_CreateTexture(sdlContext->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, canvas->width, canvas->height);
	sdlContext->isOpen = 1;
}

void linguini_toSDL(linguini_PixelArray* canvas, linguini_SDLContext* sdlContext){
	if(canvas->changed == 0){ return; }
	SDL_UpdateTexture(sdlContext->windowTexture, NULL, canvas->pixels, canvas->width * sizeof(uint32_t));
	SDL_RenderClear(sdlContext->renderer);
	SDL_RenderCopy(sdlContext->renderer, sdlContext->windowTexture, NULL, NULL);
	SDL_RenderPresent(sdlContext->renderer);
	canvas->changed = 0;
}

int linguini_SDLPollEvents(SDL_Event* e){
	return SDL_PollEvent(e);
}

void linguini_closeSDL(linguini_SDLContext* sdlContext){
	SDL_DestroyTexture(sdlContext->windowTexture);
	SDL_DestroyRenderer(sdlContext->renderer);
	SDL_DestroyWindow(sdlContext->window);
	SDL_Quit();
}
