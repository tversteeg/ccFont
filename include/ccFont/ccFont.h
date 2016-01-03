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
	float color[4];
} ccfFontConfiguration;

typedef struct {
	// Glyph parameters
	unsigned gwidth, gheight, gstart, gnum;

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
void ccfTtfToFont(ccfFont *font, const unsigned char *ttfbin, int size, unsigned firstchar, unsigned numchars);

// Blit a texture to a OpenGL style texture, last parameters are the same as in glTexImage2D
// Return codes:
// 	-1	Format is not supported (yet)
// 	-2	Type is not supported (yet)
// 	-3	Glyph is not in bitmap
int ccfGLTexBlitFont(const ccfFont *font, const char *string, const ccfFontConfiguration *config, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data);

#ifdef __cplusplus
}
#endif
