#include <ccore/core.h>

#ifdef LINUX

int main(void)
{
	return 0;
}

#else
#include <ccFont/ccFont.h>

#include <ccore/display.h>
#include <ccore/window.h>
#include <ccore/opengl.h>
#include <ccore/time.h>
#include <ccore/file.h>

#include <stdio.h>

#ifdef WINDOWS
#include <gl/GL.h>
#else
#include <GL/glew.h>
#endif

#define WIDTH 500
#define HEIGHT 70

#define TEST_FORMAT GL_RGB
#define TEST_TYPE GL_UNSIGNED_BYTE

typedef struct {
	unsigned char r, g, b;
} pixel_t;

typedef struct {
	unsigned int width, height;
	pixel_t *pixels;
} texture_t;

GLuint gltex;
texture_t tex;

void renderTexture(texture_t tex)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, gltex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.width, tex.height, 0, TEST_FORMAT, TEST_TYPE, tex.pixels);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void blitText(ccfFont *font, const char *text, int x, int y)
{
	ccfFontConfiguration conf = {.x = x, .y = y, .width = WIDTH, .wraptype = 0, .color = {1.0, 1.0, 0.0}};
	int status = ccfGLTexBlitText(font, text, &conf, tex.width, tex.height, TEST_FORMAT, TEST_TYPE, (void*)tex.pixels);
	if(status < 0){
		fprintf(stderr, "ccfGLTexBlitText failed with status code: %d\n", status);
		exit(1);
	}
}

void testTTFFile(const char *file)
{
	unsigned flen = ccFileInfoGet(file).size;

	FILE *fp = fopen(file, "rb");
	if(!fp){
		fprintf(stderr, "Can not open file: %s\n", file);
		exit(1);
	}

	unsigned char *ttf = (unsigned char*)malloc(flen + 1);
	fread(ttf, 1, flen, fp);

	fclose(fp);

	int size = ccfTtfGetPixelSize(ttf);
	if(size == -1){
		fprintf(stderr, "ccfTtfGetPixelSize failed: font %s is not a pixel font\n", file);
		exit(1);
	}
	ccfFont ttffont;
	ccfTtfToFont(&ttffont, ttf, size, '!', 128, 0);

	uint8_t *bin;
	size_t len = ccfFontToBin(&ttffont, &bin);
	if(ccfBinToFont(&ttffont, bin, len) == -1){
		fprintf(stderr, "Binary font failed: invalid version\n");
		exit(1);
	}

	blitText(&ttffont, "ABCDEFGHIJKLMOPQRSTUVWXYZ", 5, 0);

	ccfFont ttffont2;
	ccfTtfToFont(&ttffont2, ttf, size, '!', 128, -1);

	blitText(&ttffont2, "abcdefghijklmopqrstuvwxyz", 5, 15);

	ccfFont ttffont3;
	ccfTtfToFont(&ttffont3, ttf, size, '!', 128, 1);
	blitText(&ttffont3, "!@#$%^&*()_-+=[]{}'\";:,<.>/?", 5, 30);

	blitText(&ttffont, "Testing the pixel fonts!", 5, 45);
}

int main(int argc, char **argv)
{
	ccDisplayInitialize();

	ccWindowCreate((ccRect){0, 0, WIDTH, HEIGHT}, "ccFont test", 0);
	ccWindowMouseSetCursor(CC_CURSOR_NONE);

	ccGLContextBind();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glBindTexture(GL_TEXTURE_2D, 0);
	glGenTextures(1, &gltex);
	glBindTexture(GL_TEXTURE_2D, gltex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	tex.width = WIDTH;
	tex.height = HEIGHT;
	tex.pixels = (pixel_t*)malloc(tex.width * tex.height * sizeof(pixel_t));
	testTTFFile("Pixerif.ttf");

	bool loop = true;
	while(loop){
		while(ccWindowEventPoll()){
			ccEvent event = ccWindowEventGet();
			switch(event.type){
				case CC_EVENT_WINDOW_QUIT:
					loop = false;
					break;
				case CC_EVENT_KEY_DOWN:
					if(event.keyCode == CC_KEY_ESCAPE){
						loop = false;
					}
					break;
				default: break;
			}
		}

		renderTexture(tex);

		ccGLBuffersSwap();

		ccTimeDelay(6);
	}

	ccFree();
	return 0;
}

#endif
