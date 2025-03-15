#ifndef LINGUINI_H
#define LINGUINI_H
#include <stddef.h>
#include <stdint.h>

// -- STRUCTS --

// channels is currently only utilised for image processing
typedef struct {
	uint32_t* pixels;
	size_t width;
	size_t height;
	int channels;
} PixelArray;

// -- INIT --
void linguini_createPixarr(PixelArray* canvas, size_t width, size_t height);

// -- GRAPHICS --
void linguini_clearPixarr(PixelArray* canvas, uint32_t color);

void linguini_drawPixel(PixelArray* canvas, int x, int y, uint32_t color);

void linguini_drawLine(PixelArray* canvas, int x1, int y1, int x2, int y2, uint32_t color);

void linguini_drawRect(PixelArray* canvas, int x, int y, int w, int h, uint32_t color);
void linguini_fillRect(PixelArray* canvas, int x, int y, int w, int h, uint32_t color);

void linguini_drawCircle(PixelArray* canvas, int cx, int cy, size_t r, uint32_t color);
void linguini_fillCircle(PixelArray* canvas, int cx, int cy, size_t r, uint32_t color);

void linguini_drawTriangle(PixelArray* canvas, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
void linguini_fillTriangle(PixelArray* canvas, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

void linguini_loadImage(PixelArray* image, const char* filepath);
void linguini_drawImage(PixelArray* canvas, PixelArray* image, int x, int y);

// -- EXPORT/DEBUG --
void linguini_pixarrToPPM(PixelArray* canvas, const char* filepath);

void linguini_log(const char* tag, const char* message, ...);
void linguini_notImplemented();

#endif //LINGUINI_H
