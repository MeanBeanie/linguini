#include "linguini.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

LINGUINIDEF void linguini_createPixarr(linguini_Pixarr* pixarr, size_t width, size_t height){
	pixarr->width = width;
	pixarr->height = height;
	pixarr->channels = 4;

	pixarr->pixels = (uint32_t*)malloc(width*height*sizeof(uint32_t));
}
LINGUINIDEF void linguini_destroyPixarr(linguini_Pixarr* pixarr){
	pixarr->width = 0;
	pixarr->height = 0;
	pixarr->channels = 0;
	free(pixarr->pixels);
	pixarr->pixels = NULL;
}

LINGUINIDEF void linguini_clearPixarr(linguini_Pixarr pixarr, uint32_t color){
	for(int y = 0; y < pixarr.height; y++){
		for(int x = 0; x < pixarr.width; x++){
			pixarr.pixels[y*pixarr.width + x] = color;
		}
	}
}
LINGUINIDEF void linguini_drawPixel(linguini_Pixarr pixarr, int x, int y, uint32_t color){
	if(x < 0 || x >= pixarr.width || y < 0 || y >= pixarr.height){ return; }
	uint32_t bgColor = pixarr.pixels[y*pixarr.width + x];
	int opacity = (color & 0xFF);
	uint8_t bytes[6] = {
		(color >> 24),
		(color >> 16),
		(color >> 8),
		(bgColor >> 24),
		(bgColor >> 16),
		(bgColor >> 8),
	};
	uint32_t pixel = 0x00000000;
	pixel |= ( (bytes[3]*(1-opacity) + bytes[0]*opacity) << 24);
	pixel |= ( (bytes[4]*(1-opacity) + bytes[1]*opacity) << 16);
	pixel |= ( (bytes[5]*(1-opacity) + bytes[2]*opacity) << 8);
	pixel |= opacity; 
	pixarr.pixels[y*pixarr.width + x] = color;
}

LINGUINIDEF void linguini_drawRect(linguini_Pixarr pixarr, int x, int y, int w, int h, uint32_t color){
	for(int dx = x; dx < x+w; dx++){
		if(dx < 0 || dx >= pixarr.width){ continue; }
		pixarr.pixels[y*pixarr.width + dx] = color;
		pixarr.pixels[(y+h)*pixarr.width + dx] = color;
	}
	for(int dy = x; dy < y+h; dy++){
		if(dy < 0 || dy >= pixarr.height){ continue; }
		pixarr.pixels[dy*pixarr.width + x] = color;
		pixarr.pixels[dy*pixarr.width + x+w] = color;
	}
}
LINGUINIDEF void linguini_fillRect(linguini_Pixarr pixarr, int x, int y, int w, int h, uint32_t color){
	for(int dy = y; dy < y+h; dy++){
		if(dy < 0 || dy >= pixarr.height){ continue; }
		for(int dx = x; dx < x+w; dx++){
			if(dx < 0 || dx >= pixarr.width){ continue; }
			pixarr.pixels[dy*pixarr.width + dx] = color;
		}
	}
}

LINGUINIDEF void linguini_drawCircle(linguini_Pixarr pixarr, int cx, int cy, size_t radius, uint32_t color){
	int sqRadius = radius*radius;
	for(int y = cy-radius; y < cy+radius; y++){
		if(y < 0 || y >= pixarr.height){ continue; }
		for(int x = cx-radius; x < cx+radius; x++){
			if(x < 0 || x >= pixarr.width){ continue; }
			int dist = (cx-x)*(cx-x) + (cy-y)*(cy-y);
			if(sqRadius-dist >= 0 && sqRadius-dist < 10){
				pixarr.pixels[y*pixarr.width + x] = color;
			}
		}
	}
}
LINGUINIDEF void linguini_fillCircle(linguini_Pixarr pixarr, int cx, int cy, size_t radius, uint32_t color){
	int sqRadius = radius*radius;
	for(int y = cy-radius; y < cy+radius; y++){
		if(y < 0 || y >= pixarr.height){ continue; }
		for(int x = cx-radius; x < cx+radius; x++){
			if(x < 0 || x >= pixarr.width){ continue; }
			int dist = (cx-x)*(cx-x) + (cy-y)*(cy-y);
			if(dist <= sqRadius){
				pixarr.pixels[y*pixarr.width + x] = color;
			}
		}
	}
}

LINGUINIDEF int linguini_edgeFunction(int x1, int y1, int x2, int y2, int x3, int y3){
	return (x2-x1)*(y3-y1) - (y2-y1)*(x3-x1);
}
LINGUINIDEF void linguini_drawTriangle(linguini_Pixarr pixarr, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color){
	int edge = linguini_edgeFunction(x1, y1, x2, y2, x3, y3);
	if(edge < 0){
		int tx = x1, ty = y1;
		x1 = x3;
		y1 = y3;
		x3 = tx;
		y3 = ty;
	}
	int lowestY = y1 < y2 ? (y1 < y3 ? y1 : y3) : (y2 < y3 ? y2 : y3);
	int lowestX = x1 < x2 ? (x1 < x3 ? x1 : x3) : (x2 < x3 ? x2 : x3);
	int highestY = y1 > y2 ? (y1 > y3 ? y1 : y3) : (y2 > y3 ? y2 : y3);
	int highestX = x1 > x2 ? (x1 > x3 ? x1 : x3) : (x2 > x3 ? x2 : x3);
	for(int y = lowestY; y <= highestY; y++){
		for(int x = lowestX; x <= highestX; x++){
			int e1 = linguini_edgeFunction(x1, y1, x2, y2, x, y);
			int e2 = linguini_edgeFunction(x2, y2, x3, y3, x, y);
			int e3 = linguini_edgeFunction(x3, y3, x1, y2, x, y);
			if(e1 == 0 || e2 == 0 || e3 == 0){
				pixarr.pixels[y*pixarr.width + x] = color;
			}
		}
	}
}
LINGUINIDEF void linguini_fillTriangle(linguini_Pixarr pixarr, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color){
	int edge = linguini_edgeFunction(x1, y1, x2, y2, x3, y3);
	if(edge < 0){
		int tx = x1, ty = y1;
		x1 = x3;
		y1 = y3;
		x3 = tx;
		y3 = ty;
	}
	int lowestY = y1 < y2 ? (y1 < y3 ? y1 : y3) : (y2 < y3 ? y2 : y3);
	int lowestX = x1 < x2 ? (x1 < x3 ? x1 : x3) : (x2 < x3 ? x2 : x3);
	int highestY = y1 > y2 ? (y1 > y3 ? y1 : y3) : (y2 > y3 ? y2 : y3);
	int highestX = x1 > x2 ? (x1 > x3 ? x1 : x3) : (x2 > x3 ? x2 : x3);
	for(int y = lowestY; y <= highestY; y++){
		for(int x = lowestX; x <= highestX; x++){
			int e1 = linguini_edgeFunction(x1, y1, x2, y2, x, y);
			int e2 = linguini_edgeFunction(x2, y2, x3, y3, x, y);
			int e3 = linguini_edgeFunction(x3, y3, x1, y2, x, y);
			if(e1 >= 0 && e2 >= 0 && e3 >= 0){
				pixarr.pixels[y*pixarr.width + x] = color;
			}
		}
	}
}

LINGUINIDEF void linguini_loadImage(linguini_Pixarr* image, const char* filepath){
	int width, height, n, ok;
	ok = stbi_info(filepath, &width, &height, &n);
	if(ok == 0){
		ego_log("Linguini Image ERROR", "Error with loading image at %s", filepath);
		exit(1);
	}
	unsigned char* data = stbi_load(filepath, &width, &height, &n, 0);
	linguini_createPixarr(image, width, height);
	image->channels = n;

	for(int i = 0; i < width*height*n; i += n){
		uint32_t color = 0x00000000;
		switch(n){
			case 1:
			{
				color |= (data[i] << 24);
				color |= (data[i] << 16);
				color |= (data[i] << 8);
				color |= 0xFF;
				image->pixels[i] = color;
				break;
			}
			case 2:
			{
				color |= (data[i] << 24);
				color |= (data[i] << 16);
				color |= (data[i] << 8);
				color |= data[i+1];
				image->pixels[i/2] = color;
				break;
			}
			case 3:
			{
				color |= (data[i] << 24);
				color |= (data[i+1] << 16);
				color |= (data[i+2] << 8);
				color |= 0xFF;
				image->pixels[i/n] = color;
				break;
			}
			case 4:
			{
				color |= (data[i] << 24);
				color |= (data[i+1] << 16);
				color |= (data[i+2] << 8);
				color |= data[i+3];
				image->pixels[i/n] = color;
				break;
			}
			default: ego_log("Linguini Image ERROR", "Expected channels from 1-4, instead got %i", n); exit(1); break;
		}
	}

	stbi_image_free(data);
}
LINGUINIDEF void linguini_drawImage(linguini_Pixarr pixarr, linguini_Pixarr image, int x, int y){
	for(int dy = y; dy < y+image.height; dy++){
		if(dy < 0 || dy >= pixarr.height){ continue; }
		for(int dx = x; dx < x+image.width; dx++){
			if(dx < 0 || dx >= pixarr.width){ continue; }
			linguini_drawPixel(pixarr, dx, dy, image.pixels[(dy-y)*image.width + (dx-x)]);
		}
	}
}

void linguini_drawText(linguini_Pixarr pixarr, const char* str, int x, int y, int scale){
	size_t len = strlen(str);
	int horizontalShift = 0;
	for(int i = 0; i < (int)len; i++){
		char c = str[i];
		int letterIndex = -1;
		if(c >= 'A' && c <= 'Z'){
			letterIndex = c-'A';
		}
		else if(c >= 'a' && c <= 'z'){
			letterIndex = c-'a';
			letterIndex += 26;
		}
		else if(c == ' '){
			horizontalShift += scale*9;
			continue;
		}
		else{
			ego_log("Linguini Text ERROR", "Given unsupported letter \'%c\'", c);
			break;
		}
		int px = 0, py = 0;
		int verticalShift = 0;
		if(c == 'p' || c == 'g' || c == 'q' || c == 'y'){
			verticalShift = 4*scale;
		}
		
		for(int dy = 0; dy < 9; dy++){
			for(int dx = 0; dx < 9; dx++){
				int pix = LINGUINI_LETTERS[letterIndex][dy][dx];
				if(pix == 1){
					int fx = x+px+horizontalShift;
					int fy = y+py+verticalShift;
					linguini_fillRect(pixarr, fx, fy, scale, scale, 0xFFFFFFFF);
				}
				px += scale;
			}
			py += scale;
			px = 0;
		}
		horizontalShift += scale*9;
	}
}
