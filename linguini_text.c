#include "linguini_text.h"
#include <string.h>

void linguini_text_drawString(linguini_PixelArray* canvas, const char* str, int x, int y, int scale){
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
			linguini_log("Text ERROR", "Given unsupported letter \'%c\'", c);
			break;
		}
		int px = 0, py = 0;
		int verticalShift = 0;
		if(c == 'p' || c == 'g' || c == 'q' || c == 'y'){
			verticalShift = 4*scale;
		}
		linguini_Rect rect;
		rect.w = scale;
		rect.h = scale;
		
		for(int dy = 0; dy < 9; dy++){
			for(int dx = 0; dx < 9; dx++){
				int pix = LINGUINI_LETTERS[letterIndex][dy][dx];
				if(pix == 1){
					rect.x = x+px+horizontalShift;
					rect.y = y+py+verticalShift;
					linguini_fillRect(canvas, rect, 0xFFFFFFFF);
				}
				px += scale;
			}
			py += scale;
			px = 0;
		}
		horizontalShift += scale*9;
	}
}
