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
#define HEIGHT 32

typedef struct {
	unsigned char r;
} pixel_t;

typedef struct {
	unsigned int width, height;
	pixel_t *pixels;
} texture_t;

GLuint gltex;
texture_t tex;

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
		fprintf(stderr, "Font %s is not a pixel font\n", file);
		exit(1);
	}
	ccfTtfToFont(&ttffont, ttf, size, '!', 32);

	ccfFontConfiguration conf = {.x = 0, .y = 0, .width = WIDTH, .wraptype = 0};
	ccfGLTexBlitFont(&ttffont, "Test", &conf, tex.width, tex.height, GL_RED, GL_UNSIGNED_BYTE, (void*)tex.pixels);
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

		ccGLBuffersSwap();

		ccTimeDelay(6);
	}

	ccFree();
	return 0;
}
