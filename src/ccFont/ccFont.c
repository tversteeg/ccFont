#include <ccFont/ccFont.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

void ccfFreeFont(ccfFont *font)
{
	free(font->bits);
}

void ccfPngToFont(ccfFont *font, const unsigned char *pngbin, unsigned binlen)
{

}

int ccfTtfGetPixelSize(ccfFont *font, const unsigned char *ttfbin)
{
	stbtt_fontinfo stfont;
	stbtt_InitFont(&stfont, ttfbin, stbtt_GetFontOffsetForIndex(ttfbin, 0));

	// Find matching pixel height for font
	int pixelheight = 2;
	char isaliased = 1;
	float scale;
	do{
		pixelheight++;
		scale = stbtt_ScaleForPixelHeight(&stfont, pixelheight);

		int w, h;
		unsigned char *bitmap = stbtt_GetCodepointBitmap(&stfont, 0, scale, 'A', &w, &h, 0, 0);

		isaliased = 1;
		int i, len = w * h;
		for(i = 0; i < len; i++){
			if(bitmap[i] > 0 && bitmap[i] < 255){
				isaliased = 0;
				break;
			}
		}
	}while(pixelheight < 100 && !isaliased);

	if(!isaliased){
		return -1;
	}

	return pixelheight;
}
	
void ccfTtfToFont(ccfFont *font, const unsigned char *ttfbin, int size, int firstchar, int numchars){
	stbtt_fontinfo stfont;
	stbtt_InitFont(&stfont, ttfbin, stbtt_GetFontOffsetForIndex(ttfbin, 0));

	int xmin, ymin, xmax, ymax;
	stbtt_GetFontBoundingBox(&stfont, &xmin, &ymin, &xmax, &ymax);
	float scale = stbtt_ScaleForPixelHeight(&stfont, size);
	font->gwidth = (xmax - xmin) * scale;
	font->gheight = (ymax - ymin) * scale;

	font->len = font->gwidth * font->gheight * numchars;
	font->bits = (unsigned char*)malloc(font->len);

	int c, endchar = firstchar + numchars;
	for(c = firstchar; c < endchar; c++){
		int w, h, xoff, yoff;
		unsigned char *bitmap = stbtt_GetCodepointBitmap(&stfont, 0, scale, c, &w, &h, &xoff, &yoff);

		int startx = c * font->gwidth + xoff;
		int starty = c * font->gheight + yoff;

		int j, i;
		for (i = 0; i < h; i++) {
			for (j = 0; j < w; j++){
				font->bits[(i + starty) * font->gwidth + j + startx] = bitmap[i * w + j] >> 7;
			}
		}
	}
}

void ccfGLRenderFont(const ccfFont *font, GLuint targettex, const char *string, ccfFontConfiguration *config)
{

}
