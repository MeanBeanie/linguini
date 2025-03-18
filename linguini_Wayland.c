#include "linguini.h"
#include <wayland-client.h>
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

void linguini_internal_registryGlobalHandler(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version){
	linguini_waylandContext* context = (linguini_waylandContext*)data;

	if(strcmp(interface, "wl_compositor") == 0){
		context->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 3);
	}
	else if(strcmp(interface, "wl_shm") == 0){
		context->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
	}
	else if(strcmp(interface, "wl_shell") == 0){
		context->shell = wl_registry_bind(registry, name, &wl_shell_interface, 1);
	}
}
void linguini_internal_registryGlobalRemoveHandler(void* data, struct wl_registry* registry, uint32_t name){}

void linguini_useWayland(linguini_waylandContext* waylandContext, linguini_PixelArray* canvas, const char* title){
	waylandContext->isOpen = 1;
	waylandContext->display = wl_display_connect(NULL);
	if(!waylandContext->display){
		linguini_log("Wayland ERROR", "Failed to connect display");
		exit(1);
	}
	waylandContext->registry = wl_display_get_registry(waylandContext->display);
 
	waylandContext->registryListener.global = linguini_internal_registryGlobalHandler;
	waylandContext->registryListener.global_remove = linguini_internal_registryGlobalRemoveHandler;
	wl_registry_add_listener(waylandContext->registry, &waylandContext->registryListener, waylandContext);
	 
	wl_display_roundtrip(waylandContext->display);

	waylandContext->surface = wl_compositor_create_surface(waylandContext->compositor);
	waylandContext->shellSurface = wl_shell_get_shell_surface(waylandContext->shell, waylandContext->surface);
	wl_shell_surface_set_toplevel(waylandContext->shellSurface);

	int stride = canvas->width * 4;
	int size = stride * canvas->height;

	int fd = syscall(SYS_memfd_create, "buffer", 0);
	ftruncate(fd, size);

	waylandContext->data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	waylandContext->shmPool = wl_shm_create_pool(waylandContext->shm, fd, size);

	waylandContext->buffer = wl_shm_pool_create_buffer(waylandContext->shmPool, 0, canvas->width, canvas->height, stride, WL_SHM_FORMAT_ARGB8888);

	wl_surface_attach(waylandContext->surface, waylandContext->buffer, 0, 0);
	wl_surface_commit(waylandContext->surface);

	wl_display_dispatch(waylandContext->display);
}

void linguini_toWayland(linguini_PixelArray* canvas, linguini_waylandContext* waylandContext){
	if(canvas->changed == 0){ return; }
	int stride = canvas->width * 4;
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
	wl_display_dispatch(waylandContext->display);
	canvas->changed = 0;
}
