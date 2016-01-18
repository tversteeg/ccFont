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

#include <stdint.h>

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
	uint8_t gwidth, gheight, gstart, gnum;

	uint32_t width, len;
	uint8_t *bits;
} ccfFont;

void ccfFreeFont(ccfFont *font);

/* Get the binary format of the font for saving
Returns the size of the binary format, or 0 when it failed
*/
size_t ccfFontToBin(ccfFont *font, uint8_t **fontbin);

/* Create font from a binary format as supplied by ccfFontToBin
Return codes:
	-1 Font version not supported
*/
int ccfBinToFont(ccfFont *font, const void *fontbin, size_t binlen);

void ccfPngToFont(ccfFont *font, const void *pngbin, size_t binlen);

/* Find the correct aliased size for a pixel font
Return codes:
	-1	Font is not a pixel font, could not find right scale without anti aliasing artifacts
*/
int ccfTtfGetPixelSize(const void *ttfbin);

// Create a bitmap font from a TTF font
void ccfTtfToFont(ccfFont *font, const void *ttfbin, int size, unsigned firstchar, unsigned numchars);

/* Blit a texture to a OpenGL style texture, last parameters are the same as in glTexImage2D
Return codes:
	-1	Format is not supported (yet)
	-2	Type is not supported (yet)
	-3	Glyph is not in bitmap
*/
int ccfGLTexBlitText(const ccfFont *font, const char *string, const ccfFontConfiguration *config, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data);

#ifdef __cplusplus
}
#endif
