#include "../linguini.h"
#include <string.h>
#include FT_FREETYPE_H

void linguini_useFreetype(linguini_freetypeContext* freetypeContext, const char* filepath, int charSize){
	freetypeContext->error = FT_Init_FreeType(&freetypeContext->library);
	if(freetypeContext->error){
		linguini_log("Freetype2 ERROR", "Error %i occurred when loading freetype2");
		exit(1);
	}

	freetypeContext->error = FT_New_Face(freetypeContext->library, filepath, 0, &freetypeContext->face);
	if(freetypeContext->error == FT_Err_Unknown_File_Format){
		linguini_log("Freetype2 ERROR", "File %s is not supported by the freetype2", filepath);
		exit(1);
	}
	else if(freetypeContext->error){
		linguini_log("Freetype2 ERROR", "Error %i occurred when loading file %s", freetypeContext->error, filepath);
		exit(1);
	}

	// assumes 72 dpi, thats what the last 2 zeros are
	freetypeContext->error = FT_Set_Char_Size(freetypeContext->face, 0, charSize*64, 0, 0);
	if(freetypeContext->error){
		linguini_log("Freetype2 ERROR", "Failed to set character size to %i", charSize);
	}
}

void linguini_drawText(linguini_PixelArray* canvas, linguini_freetypeContext* freetypeContext, int x, int y, uint32_t color, const char* unformatted, ...){
	int n = 0;
	char* text = NULL;
	size_t nChars = 0;

	va_list args;
	va_start(args, unformatted);
	n = vsnprintf(text, nChars, unformatted, args);
	va_end(args);

	if(n < 0){
		return;
	}

	nChars = (size_t)n + 1;
	text = malloc(nChars);
	if(text == NULL){
		return;
	}

	va_start(args, unformatted);
	n = vsnprintf(text, nChars, unformatted, args);
	va_end(args);

	if(n < 0){
		free(text);
		return;
	}

	nChars = nChars-1;

	for(int i = 0; i < nChars; i++){
		FT_UInt index;
		index = FT_Get_Char_Index(freetypeContext->face, text[i]);

		freetypeContext->error = FT_Load_Glyph(freetypeContext->face, index, FT_LOAD_DEFAULT);
		if(freetypeContext->error){
			linguini_log("Freetype2 ERROR", "Error %i loading %c from %s", freetypeContext->error, text[i], text);
			continue;
		}

		freetypeContext->error = FT_Render_Glyph(freetypeContext->face->glyph, FT_RENDER_MODE_NORMAL);
		if(freetypeContext->error){
			linguini_log("Freetype2 ERROR", "Error %i rendering glyph");
			continue;
		}

		int targetX = x + freetypeContext->face->glyph->bitmap_left;
		int targetY = y - freetypeContext->face->glyph->bitmap_top;

		if(targetX >= canvas->width || targetY >= canvas->height){
			break;
		}
		if(targetX < 0 || targetY < 0){
			continue;
		}

		for(int dy = 0; dy < freetypeContext->face->glyph->bitmap.rows; dy++){
			for(int dx = 0; dx < freetypeContext->face->glyph->bitmap.pitch; dx++){
				int pixel = freetypeContext->face->glyph->bitmap.buffer[dy*freetypeContext->face->glyph->bitmap.pitch + dx];
				if(pixel != 0){
					linguini_drawPixel(canvas, targetX+dx, targetY+dy, color);
				}
			}
		}

		x += freetypeContext->face->glyph->advance.x >> 6;
		y += freetypeContext->face->glyph->advance.y >> 6;
	}
}
