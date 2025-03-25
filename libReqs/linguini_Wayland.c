#include "../linguini.h"
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <syscall.h>

// ERROR!!!, im using wl_shell when i need to implement and use xdg_shell
// wl_shell is deprecated/no longer supported by modern compositors
// old tutorials, while helpful, always have downfalls
// good luck n all that
// 25-03-17 22:45

void releaseBuffer(void* data, struct wl_buffer* wl_buffer){
	wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener bufferListener = {
	.release = releaseBuffer,
};

static void xdg_surface_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial){
	linguini_waylandContext* context = data;
	xdg_surface_ack_configure(xdg_surface, serial);

	struct wl_buffer* buffer = linguini_toWayland(context->pixarr, context);
	wl_surface_attach(context->surface, buffer, 0, 0);
	wl_surface_commit(context->surface);
}

static const struct xdg_surface_listener xdgSurfaceListener = {
	.configure = xdg_surface_configure,
};

static void xdg_wm_base_ping(void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial){
	xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdgWmBaseListener = {
	.ping = xdg_wm_base_ping,
};

void registryGlobalHandler(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version){
	linguini_waylandContext* context = (linguini_waylandContext*)data;

	if(strcmp(interface, "wl_compositor") == 0){
		context->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
	}
	else if(strcmp(interface, "wl_shm") == 0){
		context->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
	}
	else if(strcmp(interface, xdg_wm_base_interface.name) == 0){
		context->wmBase = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(context->wmBase, &xdgWmBaseListener, context);
	}
}
void registryGlobalRemoveHandler(void* data, struct wl_registry* registry, uint32_t name){}

void linguini_useWayland(linguini_waylandContext* waylandContext, linguini_PixelArray* canvas, const char* title){
	waylandContext->isOpen = 1;
	waylandContext->pixarr = canvas;
	waylandContext->display = wl_display_connect(NULL);
	if(!waylandContext->display){
		linguini_log("Wayland ERROR", "Failed to connect display");
		exit(1);
	}
	waylandContext->registry = wl_display_get_registry(waylandContext->display);
 
	waylandContext->registryListener.global = registryGlobalHandler;
	waylandContext->registryListener.global_remove = registryGlobalRemoveHandler;
	wl_registry_add_listener(waylandContext->registry, &waylandContext->registryListener, waylandContext);

	wl_display_roundtrip(waylandContext->display);

	waylandContext->surface = wl_compositor_create_surface(waylandContext->compositor);
	waylandContext->xdgSurface = xdg_wm_base_get_xdg_surface(waylandContext->wmBase, waylandContext->surface);
	xdg_surface_add_listener(waylandContext->xdgSurface, &xdgSurfaceListener, waylandContext);
	waylandContext->toplevel = xdg_surface_get_toplevel(waylandContext->xdgSurface);
	xdg_toplevel_set_title(waylandContext->toplevel, title);
	wl_surface_commit(waylandContext->surface);
}

static struct wl_buffer* linguini_toWayland(linguini_PixelArray* canvas, linguini_waylandContext* waylandContext){
	int stride = canvas->width * 4;
	int size = stride * canvas->height;

	int fd = syscall(SYS_memfd_create, "buffer", 0);
	ftruncate(fd, size);

	waylandContext->data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(waylandContext->data == MAP_FAILED){
		close(fd);
		linguini_log("Wayland ERROR", "Failed to map data");
		exit(1);
	}
	struct wl_shm_pool* shmPool = wl_shm_create_pool(waylandContext->shm, fd, size);

	struct wl_buffer* buffer = wl_shm_pool_create_buffer(shmPool, 0, canvas->width, canvas->height, stride, WL_SHM_FORMAT_ARGB8888);
	wl_shm_pool_destroy(shmPool);
	close(fd);

	for(int y = 0; y < canvas->height; y++){
		for(int x = 0; x < canvas->width; x++){
			struct pixel {
				uint8_t blue;
				uint8_t red;
				uint8_t green;
				uint8_t alpha;
			} *px = (struct pixel*)(waylandContext->data + y * stride + x * 4);
			uint32_t canvasPixel = canvas->pixels[y*canvas->width + x];
			px->red = (canvasPixel>>24)&0xFF;
			px->green = (canvasPixel>>16)&0xFF;
			px->blue = (canvasPixel>>8)&0xFF;
			px->alpha = canvasPixel&0xFF;
		}
	}

	munmap(waylandContext->data, size);
	wl_buffer_add_listener(buffer, &bufferListener, NULL);

	return buffer;
}

int linguini_waylandDisplayDispatch(linguini_waylandContext* waylandContext){
	return wl_display_dispatch(waylandContext->display);
}
