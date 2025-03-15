#include "linguini.h"

int main(void){
	PixelArray pixarr;
	linguini_createPixarr(&pixarr, 800, 450);
	linguini_clearPixarr(&pixarr, 0x202020FF);

	linguini_fillRect(&pixarr, 10, 10, 100, 100, 0xFF000099);
	linguini_fillRect(&pixarr, 60, 60, 100, 100, 0x0000FF99);

	linguini_pixarrToPPM(&pixarr, "output.ppm");

	return 0;
}
