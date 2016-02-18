#include <ccFont/ccFont.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

static inline uint8_t boolToByte(const uint8_t *b, size_t len)
{
	uint8_t c = 0;
	int i;
	for(i = 0; i < len; i++){
		if(b[i]){
			c |= 1 << i;
		}
	}
	return c;
}

static inline void byteToBools(uint8_t byte, uint8_t *b, size_t len)
{
	int i;
	for(i = 0; i < len; i++){
		b[i] = (byte >> i) & 1;
	}
}

static int _ccfBlitChar(const ccfFont *font, char ch, int x, int y, const float *color, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data) 
{
	int c = ch - font->gstart;
	if(c < 0 || c > font->gnum){
		return -3;
	}

	int xstart = c * font->gwidth;

	char bits = 0;
	switch(format){
		case GL_RGBA: 
		case GL_BGRA: 
			bits = 4;
			break;
		case GL_RGB: 
		case GL_BGR: 
			bits = 3;
			break;
		case GL_RG: 
			bits = 2;
			break;
		case GL_RED: 
			bits = 1;
			break; 
	}

#define _CCF_SET_CHANNEL(target, bit, ti, ci) \
	target[ti] = target[ti] * (1 - bit) + color[ci] * (bit << 7);

#define _CCF_LOOP_PIXELS(_CCF_PIXEL_TYPE) \
	{\
		for(int i = 0; i < font->gheight; i++){ \
			for(int j = 0; j < font->gwidth; j++){ \
				_CCF_PIXEL_TYPE *target = (_CCF_PIXEL_TYPE*)data + (j + x + (i + y) * width) * bits; \
				uint8_t bit = font->bits[xstart + j + i * font->width]; \
				switch(format){ \
					case GL_RGBA: \
												_CCF_SET_CHANNEL(target, bit, 3, 3); \
					case GL_RGB: \
											 _CCF_SET_CHANNEL(target, bit, 2, 2); \
					case GL_RG: \
											_CCF_SET_CHANNEL(target, bit, 1, 1); \
					case GL_RED: \
											 _CCF_SET_CHANNEL(target, bit, 0, 0); \
					break; \
					case GL_BGRA: \
												_CCF_SET_CHANNEL(target, bit, 3, 3); \
					case GL_BGR: \
											 _CCF_SET_CHANNEL(target, bit, 0, 2); \
					_CCF_SET_CHANNEL(target, bit, 1, 1); \
					_CCF_SET_CHANNEL(target, bit, 2, 0); \
					break; \
				}\
			} \
		} \
	}

	switch(type){
		case GL_UNSIGNED_BYTE:
			_CCF_LOOP_PIXELS(unsigned char);
			break;
		case GL_BYTE:
			_CCF_LOOP_PIXELS(char);
			break;
		case GL_UNSIGNED_SHORT:
			_CCF_LOOP_PIXELS(unsigned short);
			break;
		case GL_SHORT:
			_CCF_LOOP_PIXELS(short);
			break;
		case GL_UNSIGNED_INT:
			_CCF_LOOP_PIXELS(unsigned int);
			break;
		case GL_INT:
			_CCF_LOOP_PIXELS(int);
			break;
		case GL_FLOAT:
			_CCF_LOOP_PIXELS(float);
			break;
	}

#undef _CCF_LOOP_PIXELS

	return 0;
}

void ccfFreeFont(ccfFont *font)
{
	free(font->bits);
}

#define _CCF_UNPACK32TO8(T, b, c, i) \
	(c)[i] = (T)(b >> 24); (c)[i + 1] = (T)(b >> 16); (c)[i + 2] = (T)(b >> 8); (c)[i + 3] = (T)b;
size_t ccfFontToBin(ccfFont *font, uint8_t **fontbin)
{
	size_t len = font->len >> 3;
	uint8_t reminder = font->len % 8;
	size_t totallen = 13 + len;
	if(reminder > 0){
		totallen++;
	}

	(*fontbin) = (uint8_t*)calloc(1, totallen);
	(*fontbin)[0] = 1; // Version
	(*fontbin)[1] = font->gwidth;
	(*fontbin)[2] = font->gheight;
	(*fontbin)[3] = font->gstart;
	(*fontbin)[4] = font->gnum;
	_CCF_UNPACK32TO8(uint8_t, font->width, *fontbin, 5);
	_CCF_UNPACK32TO8(uint8_t, font->len, *fontbin, 9);

	size_t i;
	for(i = 0; i < len; i++){
		(*fontbin)[i + 13] = boolToByte(font->bits + i * 8, 8);
	}

	if(reminder > 0){
		(*fontbin)[len + 14] = boolToByte(font->bits + len * 8, reminder);
	}

	return totallen;
}

#define _CCF_PACK8TO32(b, c, i) \
	b = (c[i] << 24) | (c[i + 1] << 16) | (c[i + 2] << 8) | c[i + 3];
int ccfBinToFont(ccfFont *font, const void *fontbin, size_t binlen)
{
	uint8_t *bin = (uint8_t*)fontbin;
	if(bin[0] != 1){
		return -1;
	}
	font->gwidth = bin[1];
	font->gheight = bin[2];
	font->gstart = bin[3];
	font->gnum = bin[4];
	_CCF_PACK8TO32(font->width, bin, 5);
	_CCF_PACK8TO32(font->len, bin, 9);

	font->bits = (uint8_t*)malloc(font->len);

	size_t len = font->len >> 3;
	uint8_t reminder = font->len % 8;

	size_t i;
	for(i = 0; i < len; i++){
		byteToBools(bin[i + 13], font->bits + i * 8, 8);
	}

	if(reminder > 0){
		byteToBools(bin[len + 13], font->bits + i * 8, reminder);
	}

	return 0;
}

void ccfPngToFont(ccfFont *font, const void *pngbin, size_t binlen)
{

}

int ccfTtfGetPixelSize(const void *ttfbin)
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

void ccfTtfToFont(ccfFont *font, const void *ttfbin, int size, unsigned firstchar, unsigned numchars)
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

int ccfGLTexBlitChar(const ccfFont *font, char c, const ccfFontConfiguration *config, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data)
{
	//TODO wrap around
	if(format != GL_RED && format != GL_RG && format != GL_RGB && format != GL_RGBA && format != GL_BGR && format != GL_BGRA){
		return -1;
	}
	if(type != GL_UNSIGNED_BYTE && type != GL_BYTE && type != GL_UNSIGNED_SHORT && type != GL_SHORT && type != GL_UNSIGNED_INT && type != GL_INT && type != GL_FLOAT){
		return -2;
	}	

	return _ccfBlitChar(font, c, config->x, config->y, config->color, width, height, format, type, data); 
}

int ccfGLTexBlitText(const ccfFont *font, const char *string, const ccfFontConfiguration *config, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data)
{
	//TODO wrap around
	if(format != GL_RED && format != GL_RG && format != GL_RGB && format != GL_RGBA && format != GL_BGR && format != GL_BGRA){
		return -1;
	}
	if(type != GL_UNSIGNED_BYTE && type != GL_BYTE && type != GL_UNSIGNED_SHORT && type != GL_SHORT && type != GL_UNSIGNED_INT && type != GL_INT && type != GL_FLOAT){
		return -2;
	}	

	int x = config->x;
	int y = config->y;
	for(const char *c = string; *c != '\0'; c++){
		x += font->gwidth;
		switch(*c){
			case '\n': 
				x = config->x;
				y += font->gheight;
				break;
			case ' ': 
				x += font->gwidth;
				break;
			default:;
				int err = _ccfBlitChar(font, *c, x, y, config->color, width, height, format, type, data); 
				if(err < 0){
					return err;
				}
				break;
		}
	}

	return 0;
}
