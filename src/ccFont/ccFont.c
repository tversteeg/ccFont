#include <ccFont/ccFont.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

void ccfPngToFont(ccfFont *font, const unsigned char *pngbin, unsigned binlen)
{

}

int ccfTtfToFont(ccfFont *font, const unsigned char *ttfbin, unsigned binlen, int firstchar, int numchars)
{
	stbtt_fontinfo stfont;
	stbtt_InitFont(&stfont, ttfbin, stbtt_GetFontOffsetForIndex(ttfbin, 0));

	// Find matching pixel height for font
	int pixelheight = 2;
	float scale;
	char isaliased;
	do{
		pixelheight++;
		scale = stbtt_ScaleForPixelHeight(&stfont, pixelheight);

		int w, h;
		unsigned char *bitmap = stbtt_GetCodepointBitmap(&stfont, 0, scale, 'w', &w, &h, 0, 0);

		isaliased = 1;
		int i, len = w * h;
		for(i = 0; i < len; i++){
			if(bitmap[i] > 0 && bitmap[i] < 255){
				isaliased = 0;
				printf("%f\n", scale);
				break;
			}
		}
	}while(pixelheight < 100 && !isaliased);

	if(!isaliased){
		return -1;
	}

	int c, endchar = firstchar + numchars;
	for(c = 0; c < endchar; c++){
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
	
	return 0;
}

void ccfGLRenderFont(const ccfFont *font, GLuint targettex, const char *string, ccfFontConfiguration *config)
{

}
