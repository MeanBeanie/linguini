#ifndef LINGUINI_H
#define LINGUINI_H
#include <stddef.h>
#include <time.h>
#include <stdint.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef USE_SDL
#include <SDL2/SDL.h>
#endif

#ifdef USE_X11
#include <X11/Xlib.h>
#endif

#ifdef USE_WAYLAND
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#endif

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

#ifdef USE_SDL
typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* windowTexture;
	size_t width;
	size_t height;
	int isOpen;
} linguini_SDLContext;
#endif

#ifdef USE_X11
typedef struct {
	Display* display;
	Window window;
	GC gc;
	int isOpen;
} linguini_X11Context;
#endif

#ifdef USE_WAYLAND
typedef struct {
	struct wl_compositor* compositor;
	struct wl_shm* shm;

	struct xdg_wm_base* wmBase;

	struct wl_display* display;
	struct wl_registry* registry;
	struct wl_registry_listener registryListener;

	struct wl_surface* surface;
	struct xdg_surface* xdgSurface;
	struct xdg_toplevel* toplevel;

	unsigned char* data;

	linguini_PixelArray* pixarr;

	int isOpen;
} linguini_waylandContext;
#endif

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

#ifdef USE_SDL
void linguini_useSDL(linguini_SDLContext* sdlContext, linguini_PixelArray* canvas, const char* title);
void linguini_closeSDL(linguini_SDLContext* sdlContext);

int linguini_SDLPollEvents(SDL_Event* e);

void linguini_toSDL(linguini_PixelArray* canvas, linguini_SDLContext* sdlContext);
#endif // USE_SDL

#ifdef USE_X11
void linguini_useX11(linguini_X11Context* x11Context, linguini_PixelArray* canvas, const char* title);
void linguini_closeX11(linguini_X11Context* x11Context);

int linguini_X11NextEvent(XEvent* e, linguini_X11Context* x11Context);

void linguini_toX11(linguini_PixelArray* canvas, linguini_X11Context* x11Context);

#endif // USE_X11

#ifdef USE_WAYLAND
void linguini_useWayland(linguini_waylandContext* waylandContext, linguini_PixelArray* canvas, const char* title);
void linguini_closeWayland(linguini_waylandContext* waylandContext);

int linguini_waylandDisplayDispatch(linguini_waylandContext* waylandContext);

static struct wl_buffer* linguini_toWayland(linguini_PixelArray* canvas, linguini_waylandContext* waylandContext);

#endif // USE_WAYLAND

#endif //LINGUINI_H
