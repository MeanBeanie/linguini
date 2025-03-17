#ifndef LINGUINI_H
#define LINGUINI_H
#include <stddef.h>
#include <time.h>
#include <stdint.h>
#include <SDL2/SDL.h>

// -- STRUCTS/VARS --

// channels is currently only utilised for image processing
typedef struct {
	uint32_t* pixels;
	size_t width;
	size_t height;
	int channels;
	int changed;
} linguini_PixelArray;

typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	size_t width;
	size_t height;
} linguini_SDLContext;

extern clock_t lastLimitFPS;

// -- Management --
void linguini_createPixarr(linguini_PixelArray* canvas, size_t width, size_t height);
void linguini_loadImage(linguini_PixelArray* image, const char* filepath);
void linguini_useSDL(linguini_SDLContext* sdlContext, linguini_PixelArray* canvas, const char* title);

void linguini_startClock(void);
int linguini_limitFPS(int targetFPS);

// -- SDL --
int linguini_SDLPollEvents(SDL_Event* e);
void linguini_closeSDL(linguini_SDLContext* sdlContext);

void linguini_toSDL(linguini_PixelArray* canvas, linguini_SDLContext* sdlContext);

// -- GRAPHICS --
void linguini_clearPixarr(linguini_PixelArray* canvas, uint32_t color);

void linguini_drawPixel(linguini_PixelArray* canvas, int x, int y, uint32_t color);

void linguini_drawLine(linguini_PixelArray* canvas, int x1, int y1, int x2, int y2, uint32_t color);

void linguini_drawRect(linguini_PixelArray* canvas, int x, int y, int w, int h, uint32_t color);
void linguini_fillRect(linguini_PixelArray* canvas, int x, int y, int w, int h, uint32_t color);

void linguini_drawCircle(linguini_PixelArray* canvas, int cx, int cy, size_t r, uint32_t color);
void linguini_fillCircle(linguini_PixelArray* canvas, int cx, int cy, size_t r, uint32_t color);

void linguini_drawEllipse(linguini_PixelArray* canvas, int cx, int cy, size_t a, size_t b, uint32_t color);

void linguini_drawTriangle(linguini_PixelArray* canvas, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
void linguini_fillTriangle(linguini_PixelArray* canvas, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

void linguini_drawImage(linguini_PixelArray* canvas, linguini_PixelArray* image, int x, int y);
// linguini_drawRotatedImage uses the top-left corner as the point of rotation
void linguini_drawRotatedImage(linguini_PixelArray* canvas, linguini_PixelArray* image, int x, int y, float theta);

// -- EXPORT/DEBUG --
void linguini_pixarrToPPM(linguini_PixelArray* canvas, const char* filepath);
int linguini_delayMillis(long msec);

void linguini_log(const char* tag, const char* message, ...);
void linguini_notImplemented();

#endif //LINGUINI_H
