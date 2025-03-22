#include "linguini.h"

int main(void){
	linguini_PixelArray canvas;
	linguini_createPixarr(&canvas, 800, 450);
	
	linguini_clearPixarr(&canvas, 0x202020FF);
	linguini_fillCircle(&canvas, 100, 100, 10, 0xFF202099);
	linguini_fillCircle(&canvas, 120, 100, 10, 0x2020FF99);

	linguini_waylandContext waylandContext;
	linguini_useWayland(&waylandContext, &canvas, "Linguini Wayland Test");

	while(linguini_waylandDisplayDispatch(&waylandContext)){
	}


	return 0;
}
