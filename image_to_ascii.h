#ifndef IMAGE_TO_ASCII_H
#define IMAGE_TO_ASCII_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the image structure
typedef struct {
    unsigned char* data;
    int width;
    int height;
    int channels;
} Image;

// Function declarations
Image* load_image(const char* filename);
Image* resize_image(Image* original, int new_width, int new_height);
char* image_to_ascii(Image* img, int target_width, int target_height);
void free_image(Image* img);
unsigned char rgb_to_grayscale(unsigned char r, unsigned char g, unsigned char b);

#endif // IMAGE_TO_ASCII_H
