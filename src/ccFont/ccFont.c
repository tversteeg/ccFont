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

void ccfTtfToFont(ccfFont *font, const unsigned char *ttfbin, int size, unsigned firstchar, unsigned numchars)
{
	stbtt_fontinfo stfont;
	stbtt_InitFont(&stfont, ttfbin, stbtt_GetFontOffsetForIndex(ttfbin, 0));

	int xmin, ymin, xmax, ymax;
	stbtt_GetFontBoundingBox(&stfont, &xmin, &ymin, &xmax, &ymax);
	float scale = stbtt_ScaleForPixelHeight(&stfont, size);
	font->gwidth = (xmax - xmin) * scale;
	font->gheight = (ymax - ymin) * scale;
	font->gstart = firstchar;
	font->gnum = numchars;
	font->width = font->gwidth * numchars;

	font->len = font->width * font->gheight;
	font->bits = (unsigned char*)calloc(font->len, 1);

	int ascent;
	stbtt_GetFontVMetrics(&stfont, &ascent,0,0);
	int baseline = ascent * scale;

	int i;
	for(i = 0; i < numchars; i++){
		int c = firstchar + i;

		int x0, y0, x1, y1;
		stbtt_GetCodepointBitmapBoxSubpixel(&stfont, c, scale, scale, 0, 0, &x0, &y0, &x1, &y1);
		int w, h, xoff, yoff;
		unsigned char *bitmap = stbtt_GetCodepointBitmap(&stfont, 0, scale, c, &w, &h, &xoff, &yoff);

		int starty = baseline + y0;
		int startx = i * font->gwidth + xoff;

		int y;
		for(y = 0; y < h; y++){
			int x, yr = y + starty;
			for(x = 0; x < w; x++){
				int xr = x + startx;
				font->bits[yr * font->width + xr] = (bitmap[y * w + x] >> 7) * 255;
			}
		}

		stbtt_FreeBitmap(bitmap, 0);
	}
}

int ccfGLTexBlitFont(const ccfFont *font, const char *string, const ccfFontConfiguration *config, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data)
{
	//TODO multiple formats & types, wrap around, colors
	if(format != GL_RED){
		return -1;
	}
	if(type != GL_UNSIGNED_BYTE){
		return -2;
	}

	int i, len = strlen(string);
	for(i = 0; i < len; i++){
		if(string[i] == ' '){
			continue;
		}
		int c = string[i] - font->gstart;
		if(c < 0 || c > font->gnum){
			return -3;
		}

		int xtstart = config->x + i * font->gwidth;
		int xfstart = c * font->gwidth;
		int y;
		for(y = 0; y < font->gheight; y++){
			int x;
			for(x = 0; x < font->gwidth; x++){
				unsigned char *target = (unsigned char*)data + (config->y + y) * width + x + xtstart;
				*target = font->bits[y * font->width + x + xfstart];
			}
		}
	}

	return 0;
}
