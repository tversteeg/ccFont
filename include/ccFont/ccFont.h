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
	unsigned int gwidth, gheight, gspacing, gstart, gnum;

	unsigned int len;
	unsigned char *bits;
} ccfFont;

void ccfPngToFont(ccfFont *font, const char *pngbin, unsigned int binlen);
void ccfTtfToFont(ccfFont *font, const char *ttfbin, unsigned int binlen, unsigned int pixelheight, int firstchar, int numchars);

void ccfGLRenderFont(ccfFont *font, GLuint targettex, const char *string, ccfFontConfiguration *config);

#ifdef __cplusplus
}
#endif
