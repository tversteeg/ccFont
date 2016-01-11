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
	stbtt_GetFontVMetrics(&stfont, &ascent, 0, 0);
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
	if(format != GL_RED && format != GL_RGB){
		return -1;
	}
	if(type != GL_UNSIGNED_BYTE){
		return -2;
	}

	// Horrible macro magic to make sure there is no 'if' inside the while loop
#define _CCF_LOOP_PIXELS(_CCF_PIXEL_FUNC, _CCF_PIXEL_TYPE) \
	{\
	int i, len = strlen(string); \
	for(i = 0; i < len; i++){ \
		if(string[i] == ' '){ \
			continue; \
		} \
		int c = string[i] - font->gstart; \
		if(c < 0 || c > font->gnum){ \
			return -3; \
		} \
		\
		int xtstart = config->x + i * font->gwidth; \
		int xfstart = c * font->gwidth; \
		int y; \
		for(y = 0; y < font->gheight; y++){ \
			int x; \
			for(x = 0; x < font->gwidth; x++){ \
				_CCF_PIXEL_FUNC(_CCF_PIXEL_TYPE); \
			} \
		} \
	} \
	}

#define _CCF_PIXEL_FUNC_RED(_CCF_PIXEL_TYPE) \
	_CCF_PIXEL_TYPE *target = (_CCF_PIXEL_TYPE*)data + (config->y + y) * width + x + xtstart; \
	*target = font->bits[y * font->width + x + xfstart] * config->color[0];

#define _CCF_PIXEL_FUNC_RG(_CCF_PIXEL_TYPE) \
	_CCF_PIXEL_TYPE *target = (_CCF_PIXEL_TYPE*)data + ((config->y + y) * width + x + xtstart) * 3; \
	*target = font->bits[y * font->width + x + xfstart] * config->color[0]; \
	*(target + 1) = font->bits[y * font->width + x + xfstart] * config->color[1];

#define _CCF_PIXEL_FUNC_RGB(_CCF_PIXEL_TYPE) \
	_CCF_PIXEL_TYPE *target = (_CCF_PIXEL_TYPE*)data + ((config->y + y) * width + x + xtstart) * 3; \
	*target = font->bits[y * font->width + x + xfstart] * config->color[0]; \
	*(target + 1) = font->bits[y * font->width + x + xfstart] * config->color[1]; \
	*(target + 2) = font->bits[y * font->width + x + xfstart] * config->color[2];

#define _CCF_PIXEL_FUNC_RGBA(_CCF_PIXEL_TYPE) \
	_CCF_PIXEL_TYPE *target = (_CCF_PIXEL_TYPE*)data + ((config->y + y) * width + x + xtstart) * 3; \
	*target = font->bits[y * font->width + x + xfstart] * config->color[0]; \
	*(target + 1) = font->bits[y * font->width + x + xfstart] * config->color[1]; \
	*(target + 2) = font->bits[y * font->width + x + xfstart] * config->color[2]; \
	*(target + 3) = font->bits[y * font->width + x + xfstart] * config->color[3];

#define _CCF_PIXEL_FUNC_BGR(_CCF_PIXEL_TYPE) \
	_CCF_PIXEL_TYPE *target = (_CCF_PIXEL_TYPE*)data + ((config->y + y) * width + x + xtstart) * 3; \
	*target = font->bits[y * font->width + x + xfstart] * config->color[2]; \
	*(target + 1) = font->bits[y * font->width + x + xfstart] * config->color[1]; \
	*(target + 2) = font->bits[y * font->width + x + xfstart] * config->color[0];

#define _CCF_PIXEL_FUNC_BGRA(_CCF_PIXEL_TYPE) \
	_CCF_PIXEL_TYPE *target = (_CCF_PIXEL_TYPE*)data + ((config->y + y) * width + x + xtstart) * 3; \
	*target = font->bits[y * font->width + x + xfstart] * config->color[2]; \
	*(target + 1) = font->bits[y * font->width + x + xfstart] * config->color[1]; \
	*(target + 2) = font->bits[y * font->width + x + xfstart] * config->color[0]; \
	*(target + 3) = font->bits[y * font->width + x + xfstart] * config->color[3];

#define _CCF_SWITCH_FORMATS(_CCF_PIXEL_TYPE) \
	switch(format){ \
		case GL_RED: \
			_CCF_LOOP_PIXELS(_CCF_PIXEL_FUNC_RED, _CCF_PIXEL_TYPE); \
			break; \
		case GL_RG: \
			_CCF_LOOP_PIXELS(_CCF_PIXEL_FUNC_RG, _CCF_PIXEL_TYPE); \
			break; \
		case GL_RGB: \
			_CCF_LOOP_PIXELS(_CCF_PIXEL_FUNC_RGB, _CCF_PIXEL_TYPE); \
			break; \
		case GL_RGBA: \
			_CCF_LOOP_PIXELS(_CCF_PIXEL_FUNC_RGBA, _CCF_PIXEL_TYPE); \
			break; \
		case GL_BGR: \
			_CCF_LOOP_PIXELS(_CCF_PIXEL_FUNC_BGR, _CCF_PIXEL_TYPE); \
			break; \
		case GL_BGRA: \
			_CCF_LOOP_PIXELS(_CCF_PIXEL_FUNC_BGRA, _CCF_PIXEL_TYPE); \
			break; \
	}

	switch(type){
		case GL_UNSIGNED_BYTE:
			_CCF_SWITCH_FORMATS(unsigned char);
			break;
		case GL_BYTE:
			_CCF_SWITCH_FORMATS(char);
			break;
		case GL_UNSIGNED_SHORT:
			_CCF_SWITCH_FORMATS(unsigned short);
			break;
		case GL_SHORT:
			_CCF_SWITCH_FORMATS(short);
			break;
		case GL_UNSIGNED_INT:
			_CCF_SWITCH_FORMATS(unsigned int);
			break;
		case GL_INT:
			_CCF_SWITCH_FORMATS(int);
			break;
		case GL_FLOAT:
			_CCF_SWITCH_FORMATS(float);
			break;
	}

	return 0;
}
