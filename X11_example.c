#include "linguini.h"

int main(void){
	linguini_PixelArray canvas;
	linguini_createPixarr(&canvas, 800, 450);
	linguini_X11Context x11Context;
	linguini_useX11(&x11Context, &canvas, "Linguini X11 Example");

	linguini_clearPixarr(&canvas, 0x202020FF);
	linguini_fillCircle(&canvas, 100, 100, 10, 0xFF202099);
	linguini_fillCircle(&canvas, 120, 100, 10, 0x2020FF99);

	XEvent e;
	while(x11Context.isOpen){
		int a = linguini_X11NextEvent(&e, &x11Context);
		switch(e.type){
			case Expose:
			{
				linguini_toX11(&canvas, &x11Context);
			}
			default: break;
		}
	}

	linguini_closeX11(&x11Context);
	return 0;
}
