#include <ccFont/ccFont.h>

#include <ccore/file.h>

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <getopt.h>

#define CCF_VERSION_MAJOR 1
#define CCF_VERSION_MINOR 0

char source[256], target[256];
int offset;

void printVersion()
{
	printf("ccfconv - version %d.%d\n", CCF_VERSION_MAJOR, CCF_VERSION_MINOR);
}

void printHelp()
{
	printf("Usage:\t ccfconv [OPTION...] [-t...|-p...|OUTPUTFILE...]\n\n"
			"Convert to CCF binary file as specified by *FILE*:\n"
			"\t-t,--ttf=FILE\tConvert a TTF font\n"
			"\t-p,--png=FILE\tConvert a packed PNG texture\n\n"
			"Options:\n"
			"\t-o,--offset=INT\tSet the texture packing offset, default is 0\n\n"
			"General information:\n"
			"\t-h,--help\tGive this help list\n"
			"\t-v,--version\tShow the current version\n");
}

void convertTTFFile()
{
	unsigned flen = ccFileInfoGet(source).size;

	FILE *fp = fopen(source, "rb");
	if(!fp){
		fprintf(stderr, "Can not open file: \"%s\"\n", source);
		exit(1);
	}

	unsigned char *ttf = (unsigned char*)malloc(flen + 1);
	fread(ttf, 1, flen, fp);

	fclose(fp);

	int size = ccfTtfGetPixelSize(ttf);
	if(size == -1){
		fprintf(stderr, "ccfTtfGetPixelSize failed: font \"%s\" is not a pixel font\n", source);
		exit(1);
	}

	ccfFont ttffont;
	ccfTtfToFont(&ttffont, ttf, size, '!', 128, offset);

	uint8_t *bin;
	size_t len = ccfFontToBin(&ttffont, &bin);

	fp = fopen(target, "wb");
	if(!fp){
		fprintf(stderr, "Can not create file: \"%s\"\n", target);
		exit(1);
	}

	fwrite(bin, 1, len, fp);
	fclose(fp);
}

static struct option opts[] = {
	{"help", no_argument, 0, 'h'},
	{"version", no_argument, 0, 'v'},
	{"png", required_argument, 0, 'p'},
	{"ttf", required_argument, 0, 't'},
	{"offset", required_argument, 0, 'o'},
	{NULL, 0, NULL, 0}
};

int main(int argc, char **argv)
{
	offset = 0;

	char sourcetype = 0;
	while(1){
		int index;
		int c = getopt_long(argc, argv, "hvp:t:o:", opts, &index);
		if(c == -1){
			break;
		}
		switch (c) {
			case 'v':
				printVersion();
				return 0;
			case 'h':
				printHelp();
				return 0;
			case 'p':
				sourcetype = 'p';
				strcpy(source, optarg);
				break;
			case 't':
				sourcetype = 't';
				strcpy(source, optarg);
				break;
			case 'o':
				offset = atoi(optarg);
				break;
			default:
				return 1;
		}
	}

	if(optind < argc) {
		strcpy(target, argv[optind]);
	}else{
		if(sourcetype == 0){
			fprintf(stderr, "No output file supplied!\n");
			exit(1);
		}else{
			strcpy(target, source);

			int i = strlen(target) - 1;
			for(; i >= 0; i--){
				if(target[i] == '.'){
					strcpy(target + i, ".ccf");
					break;
				}
			}
			if(i < 0){
				strcpy(target + strlen(target), ".ccf");
			}
		}
	}

	switch(sourcetype){
		case 't':
			convertTTFFile();
			break;
		default:
			fprintf(stderr, "File type not implemented yet!\n");
			return 1;
			break;
	}

	printf("File \"%s\" successfully converted to \"%s\"\n", source, target);

	return 0;
}
