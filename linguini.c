#include "linguini.h"
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

void linguini_createPixarr(PixelArray* canvas, size_t width, size_t height){
	canvas->width = width;
	canvas->height = height;
	canvas->channels = 4; // RGBA by default, can be manually changed

	canvas->pixels = (uint32_t*)malloc(width*height*sizeof(uint32_t));
	linguini_clearPixarr(canvas, 0x000000FF);
}

void linguini_clearPixarr(PixelArray* canvas, uint32_t color){
	for(int y = 0; y < canvas->height; y++){
		for(int x = 0; x < canvas->width; x++){
			canvas->pixels[y*canvas->width + x] = color;
		}
	}
}

void linguini_log(const char* tag, const char* message, ...){
	va_list args;
	va_start(args, message);

	time_t now;
	time(&now);
	
	printf("(%s) %s\t", tag, ctime(&now));
	vprintf(message, args);
	printf("\n");

	va_end(args);
}

void linguini_pixarrToPPM(PixelArray* canvas, const char* filepath){
	FILE* file = fopen(filepath, "wb");
	if(file == NULL){
		printf("Failed to open file %s with error %s\n", filepath, errno);
		return;
	}

	fprintf(file, "P6\n%zu %zu 255\n", canvas->width, canvas->height);
	for(int i = 0; i < canvas->width*canvas->height; i++){
		uint32_t pixel = canvas->pixels[i];
		uint8_t bytes[3] = {
			(pixel >> 24)&0xFF,
			(pixel >> 16)&0xFF,
			(pixel >> 8)&0xFF,
		};
		fwrite(bytes, 3*sizeof(uint8_t), 1, file);
		int err = ferror(file);
		if(err > 0){
			printf("Failed to write data to file with error: %d\n", err);
			break;
		}
	}

	fclose(file);
}

void linguini_notImplemented(){
	linguini_log("ERROR", "Attempted to use function that hasn't been implemented, try again later");
}

void linguini_drawPixel(PixelArray* canvas, int x, int y, uint32_t color){
	uint32_t pixel = canvas->pixels[y*canvas->width + x];
	uint8_t pixelBytes[3] = {
		(pixel>>24)&0xFF,
		(pixel>>16)&0xFF,
		(pixel>>8)&0xFF,
	};
	uint8_t colorBytes[3] = {
		(color>>24)&0xFF,
		(color>>16)&0xFF,
		(color>>8)&0xFF,
	};
	uint32_t blendedColor = 0x00000000;
	uint8_t opacityInt = color&0xFF;
	float opacity = (float)opacityInt/255.f;

	blendedColor |= (uint32_t)((pixelBytes[0]*(1-opacity)+colorBytes[0]*opacity))<<24;
	blendedColor |= (uint32_t)((pixelBytes[1]*(1-opacity)+colorBytes[1]*opacity))<<16;
	blendedColor |= (uint32_t)((pixelBytes[2]*(1-opacity)+colorBytes[2]*opacity))<<8;
	blendedColor |= (color&0xFF);

	canvas->pixels[y*canvas->width + x] = blendedColor;
}

void linguini_drawLine(PixelArray* canvas, int x1, int y1, int x2, int y2, uint32_t color){	
	if(x1 == x2 && x1 >= 0 && x1 < canvas->width){
		// vertical line
		int sy = y1 < y2 ? y1 : y2;
		int ey = y1 > y2 ? y1 : y2;
		for(int y = sy; y <= ey; y++){
			if(y < 0){ continue; }
			if(y >= canvas->height){ break; }
			linguini_drawPixel(canvas, x1, y, color);
		}
		return;
	}
	if(y1 == y2 && y1 >= 0 && y1 < canvas->height){
		// horizontal line
		int sx = x1 < x2 ? x1 : x2;
		int ex = x1 < x2 ? x1 : x2;
		for(int x = sx; x <= ex; x++){
			if(x < 0){ continue; }
			if(x >= canvas->width){ break; }
			linguini_drawPixel(canvas, x, y1, color);
		}
		return;
	}

	// sloped line
	if(x1 > x2){
		int tx = x1, ty = y1;
		x1 = x2;
		y1 = y2;
		x2 = tx;
		y2 = ty;
	}

	int dx = x2 - x1;
	int dy = y2 - y1;
	float m = dy/dx;

	for(int x = x1; x < x2; x++){
		if(x < 0){ continue; }
		if(x >= canvas->width){ break; }

		int y = m*(x-x1) + y1;
		if(y >= 0 && y < canvas->height){
			int ny = m*(x+1-x1) + y1;
			int sy = ny > y ? y : ny;
			int ey = ny < y ? y : ny;
			for(int dy = sy; dy < ey; dy++){
				linguini_drawPixel(canvas, x, dy, color);
			}
		}
	}
}

void linguini_drawRect(PixelArray* canvas, int x, int y, int w, int h, uint32_t color){
	for(int dx = x; dx < x+w; dx++){
		if(dx < 0){ continue; }
		if(dx >= canvas->width){ break; }

		if(y >= 0 && y < canvas->height){
			linguini_drawPixel(canvas, dx, y, color);
		}
		if(y+h >= 0 && y+h < canvas->height){
			linguini_drawPixel(canvas, dx, y+h, color);
		}
	}

	for(int dy = y; dy < y+h; dy++){
		if(dy < 0){ continue; }
		if(dy >= canvas->height){ break; }

		if(x >= 0 && x < canvas->width){
			linguini_drawPixel(canvas, x, dy, color);
		}
		if(x+w >= 0 && x+w < canvas->width){
			linguini_drawPixel(canvas, x+w, dy, color);
		}
	}
}
void linguini_fillRect(PixelArray* canvas, int x, int y, int w, int h, uint32_t color){
	for(int dy = y; dy < y+h; dy++){
		if(dy < 0){ continue; }
		if(dy >= canvas->height){ break; }
		for(int dx = x; dx < x+w; dx++){
			if(dx < 0){ continue; }
			if(dx >= canvas->width){ break; }

			linguini_drawPixel(canvas, dx, dy, color);
		}
	}
}

void linguini_drawCircle(PixelArray* canvas, int cx, int cy, size_t r, uint32_t color){
	for(int y = cy-r; y < cy+r; y++){
		if(y < 0){ continue; }
		if(y >= canvas->height){ break; }
		for(int x = cx-r; x < cx+r; x++){
			if(x < 0){ continue; }
			if(x >= canvas->width){ break; }
			int dist = (cx-x)*(cx-x) + (cy-y)*(cy-y);
			if(r*r-dist > 0 && r*r-dist < r*2){
				linguini_drawPixel(canvas, x, y, color);
			}
		}
	}
}
void linguini_fillCircle(PixelArray* canvas, int cx, int cy, size_t r, uint32_t color){
	for(int y = cy-r; y < cy+r; y++){
		if(y < 0){ continue; }
		if(y >= canvas->height){ break; }
		for(int x = cx-r; x < cx+r; x++){
			if(x < 0){ continue; }
			if(x >= canvas->width){ break; }
			int dist = (cx-x)*(cx-x) + (cy-y)*(cy-y);
			if(dist <= r*r){
				linguini_drawPixel(canvas, x, y, color);
			}
		}
	}
}

void linguini_drawTriangle(PixelArray* canvas, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color){
	linguini_drawLine(canvas, x1, y1, x2, y2, color);
	linguini_drawLine(canvas, x2, x2, x3, y3, color);
	linguini_drawLine(canvas, x3, y3, x1, y1, color);
}

int edgeFunction(int x1, int y1, int x2, int y2, int x3, int y3){
	return (x2-x1)*(y3-y1) - (y2-y1)*(x3-x1);
}

void linguini_fillTriangle(PixelArray* canvas, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color){
	if(edgeFunction(x1, y1, x2, y2, x3, y3) < 0){
		int tx = x1, ty = x1;
		x1 = x3;
		y1 = y3;
		x3 = x1;
		y3 = y1;
	}

	int sx = x1 < x2 ? (x1 < x3 ? x1 : x3) : (x2 < x3 ? x2 : x3);
	int ex = x1 > x2 ? (x1 > x3 ? x1 : x3) : (x2 > x3 ? x2 : x3);
	int sy = y1 < y2 ? (y1 < y3 ? y1 : y3) : (y2 < y3 ? y2 : y3);
	int ey = y1 > y2 ? (y1 > y3 ? y1 : y3) : (y2 > y3 ? y2 : y3);

	for(int y = sy; y < ey; y++){
		if(y < 0){ continue; }
		if(y >= canvas->height){ break; }
		for(int x = sy; x < ey; x++){
			if(x < 0){ continue; }
			if(x >= canvas->height){ break; }

			int ABP = edgeFunction(x1, y1, x2, y2, x, y);
			int BCP = edgeFunction(x2, y2, x3, y3, x, y);
			int CAP = edgeFunction(x3, y3, x1, y1, x, y);
			if(ABP >= 0 && BCP >= 0 && CAP >= 0){
				linguini_drawPixel(canvas, x, y, color);
			}
		}
	}
}

void linguini_loadImage(PixelArray* image, const char* filepath){
	int width, height, n;
	uint8_t* data = stbi_load(filepath, &width, &height, &n, 0);
	linguini_createPixarr(image, width, height);
	image->channels = n;

	int pixelIndex = 0;
	for(int i = 0; i < width*height*n; i += n){
		uint32_t pixel = 0x00000000;
		if(n == 3){
			pixel |= data[i]<<24;
			pixel |= data[i+1]<<16;
			pixel |= data[i+2]<<8;
			pixel |= 0xFF;
			image->pixels[pixelIndex] = pixel;
		}
		else if(n == 4){
			pixel |= data[i]<<24;
			pixel |= data[i+1]<<16;
			pixel |= data[i+2]<<8;
			pixel |= data[i+3];
			image->pixels[pixelIndex] = pixel;
		}
		else{
			printf("Error loading image at %s: had unsupported number of channels %d\n", filepath, n);
			linguini_clearPixarr(image, 0xABCDEF);
			break;
		}
		pixelIndex++;
	}
	
	stbi_image_free(data);
}

void linguini_drawImage(PixelArray* canvas, PixelArray* image, int x, int y){
	if(image->channels > canvas->channels || image->channels < 3){
		linguini_log("ERROR", "Attmpted to draw image with %d channels, onto canvas with %d channels", image->channels, canvas->channels);
		return;
	}
	for(int dy = y; dy < y+image->height; dy++){
		if(dy < 0){ continue; }
		if(dy >= canvas->height){ break; }
		for(int dx = x; dx < x+image->width; dx++){
			if(dx < 0){ continue; }
			if(dx >= canvas->width){ break; }
			linguini_drawPixel(canvas, dx, dy, image->pixels[(dy-y)*image->width + (dx-x)]);
		}
	}

