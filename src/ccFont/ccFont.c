#include <ccFont/ccFont.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

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

	int c, endchar = firstchar + numchars;
	for(c = firstchar; c < endchar; c++){
		int w, h;
		unsigned char *bitmap = stbtt_GetCodepointBitmap(&stfont, 0, scale, c, &w, &h, 0, 0);

		int j, i;
		for (j=0; j < h; ++j) {
			for (i=0; i < w; ++i){
				putchar(" .:ioVM@"[bitmap[j*w+i]>>5]);
			}
			putchar('\n');
		}
		putchar('\n');
	}
}

void ccfGLRenderFont(const ccfFont *font, GLuint targettex, const char *string, ccfFontConfiguration *config)
{

}
