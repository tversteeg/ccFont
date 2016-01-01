#include <ccFont/ccFont.h>

void ccfPngToFont(ccfFont *font, const char *pngbin, unsigned int binlen)
{

}

void ccfTtfToFont(ccfFont *font, const char *ttfbin, unsigned int binlen, unsigned int pixelheight, int firstchar, int numchars)
{
	font->gheight = pixelheight;
	font->gstart = firstchar;
	font->gnum = numchars;
}

void ccfGLRenderFont(ccfFont *font, GLuint targettex, const char *string, ccfFontConfiguration *config)
{

}
