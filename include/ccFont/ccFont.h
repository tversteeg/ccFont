//__________________________________________________________________________________//
//                                                                                  //
//      This program is free software: you can redistribute it and/or modify        //
//      it under the terms of the 3-clause BSD license.                             //
//                                                                                  //
//      You should have received a copy of the 3-clause BSD License along with      //
//      this program. If not, see <http://opensource.org/licenses/>.                //
//__________________________________________________________________________________//

#pragma once

#ifdef WINDOWS
#include <gl/GL.h>
#else
#include <GL/glew.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int x, y;
	int width;
	int wraptype;
} ccfFontConfiguration;

typedef struct {
	// Glyph parameters
	unsigned gwidth, gheight, gspacing, gstart, gnum;

	unsigned len;
	unsigned char *bits;
} ccfFont;

void ccfFreeFont(ccfFont *font);

void ccfPngToFont(ccfFont *font, const unsigned char *pngbin, unsigned binlen);
// Find the correct aliased size for a pixel font
// Return codes:
// 	-1	Font is not a pixel font, could not find right scale without anti aliasing artifacts
int ccfTtfGetPixelSize(ccfFont *font, const unsigned char *ttfbin);
// Create a bitmap font from a TTF font
void ccfTtfToFont(ccfFont *font, const unsigned char *ttfbin, int size, int firstchar, int numchars);

void ccfGLRenderFont(const ccfFont *font, GLuint targettex, const char *string, ccfFontConfiguration *config);

#ifdef __cplusplus
}
#endif
