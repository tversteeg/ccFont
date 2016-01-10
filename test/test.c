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

#define WIDTH 400
#define HEIGHT 50

typedef struct {
	unsigned char r;
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.width, tex.height, 0, GL_RED, GL_UNSIGNED_BYTE, tex.pixels);

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
	ccfFontConfiguration conf = {.x = x, .y = y, .width = WIDTH, .wraptype = 0};
	int status = ccfGLTexBlitFont(font, text, &conf, tex.width, tex.height, GL_RED, GL_UNSIGNED_BYTE, (void*)tex.pixels);
	if(status < 0){
		fprintf(stderr, "ccfGLTexBlitFont failed with status code: %d\n", status);
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

	ccfFont ttffont;
	int size = ccfTtfGetPixelSize(&ttffont, ttf);
	if(size == -1){
		fprintf(stderr, "ccfTtfGetPixelSize failed: font %s is not a pixel font\n", file);
		exit(1);
	}
	ccfTtfToFont(&ttffont, ttf, size, '!', 128);

	blitText(&ttffont, "ABCDEFGHIJKLMOPQRSTUVWXYZ", 5, 0);
	blitText(&ttffont, "abcdefghijklmopqrstuvwxyz", 5, 15);
	blitText(&ttffont, "!@#$%^&*()_-+=[]{}'\";:,<.>/?", 5, 30);
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
