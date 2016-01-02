#include <ccFont/ccFont.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

void ccfPngToFont(ccfFont *font, const unsigned char *pngbin, unsigned int binlen)
{

}

void ccfTtfToFont(ccfFont *font, const unsigned char *ttfbin, unsigned int binlen, unsigned int pixelheight, int firstchar, int numchars)
{
	stbtt_fontinfo stfont;
	stbtt_InitFont(&stfont, ttfbin, stbtt_GetFontOffsetForIndex(ttfbin, 0));

	int w, h;
	unsigned char *bitmap = stbtt_GetCodepointBitmap(&stfont, 0, stbtt_ScaleForPixelHeight(&stfont, pixelheight), 'a', &w, &h, 0,0);

	int j, i;
 for (j=0; j < h; ++j) {
      for (i=0; i < w; ++i)
         putchar(" .:ioVM@"[bitmap[j*w+i]>>5]);
      putchar('\n');
   }
}

void ccfGLRenderFont(const ccfFont *font, GLuint targettex, const char *string, ccfFontConfiguration *config)
{

}
