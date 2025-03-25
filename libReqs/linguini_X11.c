#include "../linguini.h"
#include <stdlib.h>
#include <X11/Xlib.h>

void linguini_useX11(linguini_X11Context* x11Context, linguini_PixelArray* canvas, const char* title){
	x11Context->display = XOpenDisplay(NULL);
	if(x11Context->display){
		linguini_log("X11 ERROR", "Failed to open display at the DISPLAY env variable");
		exit(1);
	}
	int screen = DefaultScreen(x11Context->display);
	Window rootWindow = DefaultRootWindow(x11Context->display);

	x11Context->window = XCreateSimpleWindow(x11Context->display, rootWindow, 0, 0, canvas->width, canvas->height, 1, BlackPixel(x11Context->display, screen), WhitePixel(x11Context->display, screen));
	XMapWindow(x11Context->display, x11Context->window);
	XStoreName(x11Context->display, x11Context->window, title);

	x11Context->gc = XCreateGC(x11Context->display, rootWindow, 0, NULL);

	// TODO add other masks as features are added
	XSelectInput(x11Context->display, x11Context->window, ExposureMask);
	x11Context->isOpen = 1;
}
void linguini_closeX11(linguini_X11Context* x11Context){
	XFreeGC(x11Context->display, x11Context->gc);
	XDestroyWindow(x11Context->display, x11Context->window);
	XCloseDisplay(x11Context->display);
}

int linguini_X11NextEvent(XEvent* e, linguini_X11Context* x11Context){
	return XNextEvent(x11Context->display, e);
}

void linguini_toX11(linguini_PixelArray* canvas, linguini_X11Context* x11Context){
	if(canvas->changed == 0){ return; }
	for(int y = 0; y < canvas->height; y++){
		for(int x = 0; x < canvas->width; x++){
			uint32_t pixel = canvas->pixels[y*canvas->width + x];
			int color = (pixel>>24) | (pixel>>16) | (pixel>>8);
			XSetForeground(x11Context->display, x11Context->gc, color);
			XDrawPoint(x11Context->display, x11Context->window, x11Context->gc, x, y);
		}
	}
	canvas->changed = 0;
}
