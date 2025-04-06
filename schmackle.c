/* +----------------------------+
 * | Title:	    BitMapGen	    |
 * | Name: 	    Max McGill	    |
 * | Date:	    02.04.25	    |
 * +----------------------------+
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "bitmaps.h"
#include "image_to_ascii.h"

/* Colours */
#define BLACK           0
#define RED             1
#define GREEN           2
#define YELLOW          3
#define BLUE            4
#define MAGENTA         5
#define CYAN            6
#define WHITE           7
#define BRIGHTBLACK     60
#define BRIGHTRED       61
#define BRIGHTGREEN     62
#define BRIGHTYELLOW    63
#define BRIGHTBLUE      64
#define BRIGHTMAGENTA   65
#define BRIGHTCYAN      66
#define BRIGHTWHITE     67

/* Definitions */
#define BUFFER 64
#define IMAGEHEIGHT 38
#define IMAGEWIDTH 63


/* Global Assignables */
int height;
int width;


/* Function Prototypes */
void clear();
void setBack(int bg);
void setColr(int fg, int bg);
void setCsr(int x, int y);
void saveCsr();
void restoreCsr();
int colrGrb(const char* colr);


int main(int argc, char *argv[])
{

	struct winsize w;	
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	height = w.ws_row + 1;
	width = w.ws_col + 1;


	char text[BUFFER] = "";
	
	int xStart = 20;
	int yStart = 10;

	int xImage = width / 2 - (int)(IMAGEWIDTH / 2);
	int yImage = height / 2 - (int)(IMAGEHEIGHT / 2);
	
	int bk = BLACK;
	int fg = WHITE;
	int bg = BLACK;

	const char* input_file = "images/greenit.png";

	if (argc < 3)
	{
		printf("Error: No commands passed\n");
		printf("ix");
		return -1;
	}
	else
	{
		printf("argc = %d\n", argc);
	}

	for (int i = 1; i < argc; i += 2)
	{
		printf("i.%d: '%s' -> '%s'\n", i,  argv[i], argv[i + 1]);
			
		if (i == argc)
		{
			printf("Error: %s has no value\n", argv[i]);
			return -1;
		}
		else if (strcmp(argv[i], "ix") == 0)
		{	
			xImage = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i],"iy") == 0)
		{
			yImage = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "tx") == 0)
		{
			xStart = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "ty") == 0)
		{
			yStart = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "t") == 0)
		{
			for (int j = 0; j < strlen(argv[i + 1]); j++)
			{
				text[j] = argv[i + 1][j];
			}
		}
		else if (strcmp(argv[i], "bk") == 0)
		{
			bk = colrGrb(argv[i + 1]);
		}
		else if (strcmp(argv[i], "fg") == 0)
		{
			fg = colrGrb(argv[i + 1]);
		}
		else if (strcmp(argv[i], "bg") == 0)
		{
			bg = colrGrb(argv[i + 1]);
		}
		else if (strcmp(argv[i], "img") == 0)
		{
			input_file = argv[i + 1];
		}


	}
	


	setBack(bk);
	setColr(fg, bg);



    // Load the image
    Image* img = load_image(input_file);
    if (!img) 
	{
        fprintf(stderr, "Failed to load image: %s\n", input_file);
        return 1;
    }
    
	img = resize_image(img, IMAGEWIDTH, IMAGEHEIGHT);

    // Convert image to ASCII
    char* ascii_art = image_to_ascii(img, img->width, img->height);
    if (!ascii_art) 
	{
        fprintf(stderr, "Failed to convert image to ASCII\n");
        free_image(img);
        return 1;
    }
	
	
	// Output to file
	FILE* file = fopen("output.txt", "w");
	if (!file) 
	{
		fprintf(stderr, "Failed to open output file: %s\n", "output");
		free(ascii_art);
		free_image(img);
		return 1;
	}

	fprintf(file, "%s", ascii_art);
	fclose(file);



	file = fopen("output.txt", "r");
	
	char line[256];
	int i = 0;

    while (fgets(line, sizeof(line), file)) {
		setCsr(xImage, yImage + i);
        printf("%s", line); 
		i++;
    }	

	int offset = 0;
	int start = 0;
 	for (int i = 0; i < ASCIIHEIGHT; i++)
 	{	
		offset = 0;
		start = 0;
 		setCsr(xStart, yStart + i + (offset * ASCIIHEIGHT));
 		for (int j = start; j < strlen(text); j++) 
 		{
			if (text[j] == '\\' && text[j + 1] == 'n')
			{
				offset += 1;
				setCsr(xStart, yStart + i + (offset * ASCIIHEIGHT));
				start = j++;
			}
			else
			{
				printf("%s", letters[(lookupChar(text[j]))] [i]);
			}
 		}
 	}

	setCsr(width, height);
	putchar(' ');
	
	fclose(file);
	
	getchar();
	return 0;
}

void clear()
{
	printf("\e[1;1H\e[2J");
}

void setBack(int bg)
{
	saveCsr();
	clear();
	setColr(bg, bg);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			setCsr(x, y);
			printf(" ");
		}
	}
	restoreCsr();
}

void setColr(int fg, int bg)
{
	printf("\033[%i;%im", fg + 30, bg + 40);
}	

void setCsr(int x, int y)
{
	printf("\x1b[%i;%iH", y, x);
}

void saveCsr()
{
	/* Save Cursor */
	printf("\x1b[s");
}

void restoreCsr()
{
	/* Return Cursor */
	printf("\x1b[u");
}

int colrGrb(const char* colr)
{
    if (strcmp(colr, "black") == 0)
    {
        return BLACK;
    }
    if (strcmp(colr, "red") == 0)
    {
        return RED;
    }
    if (strcmp(colr, "green") == 0)
    {
        return GREEN;
    }
    if (strcmp(colr, "yellow") == 0)
    {
        return YELLOW;
    }
    if (strcmp(colr, "blue") == 0)
    {
        return BLUE;
    }
    if (strcmp(colr, "magenta") == 0)
    {
        return MAGENTA;
    }
    if (strcmp(colr, "cyan") == 0)
    {
        return CYAN;
    }
    if (strcmp(colr, "white") == 0)
    {
        return WHITE;
    }
    if (strcmp(colr, "brightblack") == 0)
    {
        return BRIGHTBLACK;
    }
    if (strcmp(colr, "brightred") == 0)
    {
        return BRIGHTRED;
    }
    if (strcmp(colr, "brightgreen") == 0)
    {
        return BRIGHTGREEN;
    }
    if (strcmp(colr, "brightyellow") == 0)
    {
        return BRIGHTYELLOW;
    }
    if (strcmp(colr, "brightblue") == 0)
    {
        return BRIGHTBLUE;
    }
    if (strcmp(colr, "brightmagenta") == 0)
    {
        return BRIGHTMAGENTA;
    }
    if (strcmp(colr, "brightcyan") == 0)
    {
        return BRIGHTCYAN;
    }
    if (strcmp(colr, "brightwhite") == 0)
    {
        return BRIGHTWHITE;
    }
    
    // Default return value if no color matches
    return BRIGHTMAGENTA;
}
