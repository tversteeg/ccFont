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
	
void ccfTtfToFont(ccfFont *font, const unsigned char *ttfbin, int size, unsigned firstchar, unsigned numchars){
	stbtt_fontinfo stfont;
	stbtt_InitFont(&stfont, ttfbin, stbtt_GetFontOffsetForIndex(ttfbin, 0));

	int xmin, ymin, xmax, ymax;
	stbtt_GetFontBoundingBox(&stfont, &xmin, &ymin, &xmax, &ymax);
	float scale = stbtt_ScaleForPixelHeight(&stfont, size);
	font->gwidth = (xmax - xmin) * scale;
	font->gheight = (ymax - ymin) * scale;
	font->gstart = firstchar;
	font->gnum = numchars;

	font->len = font->gwidth * font->gheight * numchars;
	font->bits = (unsigned char*)malloc(font->len);

	unsigned c, endchar = firstchar + numchars;
	for(c = firstchar; c < endchar; c++){
		int w, h, xoff, yoff;
		unsigned char *bitmap = stbtt_GetCodepointBitmap(&stfont, 0, scale, c, &w, &h, &xoff, &yoff);

		int startx = c * font->gwidth + xoff;

		int i;
		for (i = 0; i < h; i++) {
			int j;
			for (j = 0; j < w; j++){
				font->bits[(i + yoff) * font->gwidth + j + startx] = (bitmap[i * w + j] >> 7) * 255;
			}
		}
	}
}

int ccfGLTexBlitFont(const ccfFont *font, const char *string, const ccfFontConfiguration *config, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data)
{
	//TODO multiple formats & types, wrap around, colors
	if(format != GL_RGB){
		return -1;
	}
	if(type != GL_UNSIGNED_BYTE){
		return -2;
	}

	int i, len = strlen(string);
	for(i = 0; i < len; i++){
		int c = string[i] - font->gstart;
		if(c < 0 || c > font->gnum){
			return -3;
		}

		int xstart = c * font->gwidth;
		int xend = xstart + font->gwidth;
		int y;
		for(y = 0; y < font->gheight; y++){
			int x;
			for(x = xstart; x < xend; x++){
				((unsigned char*)data)[(config->y + y) * width +  x + config->x] |= font->bits[y * font->gwidth + x];
			}
		}
	}

	return 0;
}
