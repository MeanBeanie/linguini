#ifndef LINGUINI_H
#define LINGUINI_H
#include <stddef.h>
#include <time.h>
#include <stdint.h>
#include <ft2build.h>
#include FT_FREETYPE_H

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
	int x;
	int y;
	int w;
	int h;
} linguini_Rect;

typedef struct {
	FT_Library library;
	FT_Face face;
	int error;
} linguini_freetypeContext;

extern struct timespec lastLimitFPS;

// -- Management --
void linguini_createPixarr(linguini_PixelArray* canvas, size_t width, size_t height);
void linguini_loadImage(linguini_PixelArray* image, const char* filepath);

void linguini_startClock(void);
int linguini_limitFPS(int targetFPS);

// -- GRAPHICS --
void linguini_clearPixarr(linguini_PixelArray* canvas, uint32_t color);

void linguini_drawPixel(linguini_PixelArray* canvas, int x, int y, uint32_t color);

void linguini_drawLine(linguini_PixelArray* canvas, int x1, int y1, int x2, int y2, uint32_t color);

void linguini_drawRect(linguini_PixelArray* canvas, linguini_Rect rect, uint32_t color);
void linguini_fillRect(linguini_PixelArray* canvas, linguini_Rect rect, uint32_t color);
int linguini_pointInRect(linguini_Rect rect, int x, int y);
int linguini_rectInRect(linguini_Rect rect1, linguini_Rect rect2);

void linguini_drawCircle(linguini_PixelArray* canvas, int cx, int cy, size_t r, uint32_t color);
void linguini_fillCircle(linguini_PixelArray* canvas, int cx, int cy, size_t r, uint32_t color);
int linguini_pointInCircle(int cx, int cy, size_t r, int x, int y);

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

// -- LIBRARIES --

void linguini_useFreetype(linguini_freetypeContext* freetypeContext, const char* filepath, int charSize);
void linguini_drawText(linguini_PixelArray* canvas, linguini_freetypeContext* freetypeContext, int x, int y, uint32_t color, const char* unformatted, ...);

#endif //LINGUINI_H
