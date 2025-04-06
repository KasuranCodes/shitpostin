#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Add the stb_image implementation (only in one .c file)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Define the existing image structure
typedef struct {
    unsigned char* data;
    int width;
    int height;
    int channels;
} Image;

// ASCII character set from darkest to lightest
const char* ASCII_CHARS = " .:-=+*#%@";
const int ASCII_CHARS_LENGTH = 10;

// Function declarations
Image* load_image(const char* filename);
Image* resize_image(Image* original, int new_width, int new_height);
char* image_to_ascii(Image* img, int target_width, int target_height);
void free_image(Image* img);
unsigned char rgb_to_grayscale(unsigned char r, unsigned char g, unsigned char b);

// Implementation of image loading using stb_image
Image* load_image(const char* filename) {
    Image* img = (Image*)malloc(sizeof(Image));
    if (!img) {
        return NULL;
    }
    
    int width, height, channels;
    img->data = stbi_load(filename, &width, &height, &channels, 0);
    
    if (!img->data) {
        free(img);
        return NULL;
    }
    
    img->width = width;
    img->height = height;
    img->channels = channels;
    
    return img;
}

// Implementation of resize_image (from your original code)
Image* resize_image(Image* original, int new_width, int new_height) {
    if (!original || new_width <= 0 || new_height <= 0) {
        return NULL;
    }
    
    Image* resized = (Image*)malloc(sizeof(Image));
    if (!resized) {
        return NULL;
    }
    
    resized->width = new_width;
    resized->height = new_height;
    resized->channels = original->channels;
    resized->data = (unsigned char*)malloc(new_width * new_height * original->channels);
    
    if (!resized->data) {
        free(resized);
        return NULL;
    }
    
    // Simple nearest neighbor resizing algorithm
    double x_ratio = (double)original->width / new_width;
    double y_ratio = (double)original->height / new_height;
    
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int px = (int)(x * x_ratio);
            int py = (int)(y * y_ratio);
            
            for (int c = 0; c < original->channels; c++) {
                int new_idx = (y * new_width + x) * original->channels + c;
                int orig_idx = (py * original->width + px) * original->channels + c;
                resized->data[new_idx] = original->data[orig_idx];
            }
        }
    }
    
    return resized;
}

// Function to convert RGB to grayscale
unsigned char rgb_to_grayscale(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
}

// Implementation of image_to_ascii (from your original code)
char* image_to_ascii(Image* img, int target_width, int target_height) {
    if (!img || target_width <= 0) {
        return NULL;
    }
    
    // If target_height is not specified, calculate it based on aspect ratio
    if (target_height <= 0) {
        double aspect_ratio = (double)img->height / img->width;
        // ASCII characters are taller than they are wide, adjust by factor of 2
        target_height = (int)(target_width * aspect_ratio * 0.5);
        if (target_height <= 0) target_height = 1;
    }
    
    // Resize the image
    Image* resized = resize_image(img, target_width, target_height);
    if (!resized) {
        return NULL;
    }
    
    // Allocate memory for ASCII result (including newlines and null terminator)
    char* ascii_art = (char*)malloc((target_width + 1) * target_height + 1);
    if (!ascii_art) {
        free(resized->data);
        free(resized);
        return NULL;
    }
    
    int ascii_index = 0;
    
    for (int y = 0; y < resized->height; y++) {
        for (int x = 0; x < resized->width; x++) {
            // Get pixel value
            unsigned char pixel_value;
            int pixel_index = (y * resized->width + x) * resized->channels;
            
            if (resized->channels == 1) {
                // Grayscale image
                pixel_value = resized->data[pixel_index];
            } else {
                // RGB or RGBA image, convert to grayscale
                unsigned char r = resized->data[pixel_index];
                unsigned char g = resized->data[pixel_index + 1];
                unsigned char b = resized->data[pixel_index + 2];
                pixel_value = rgb_to_grayscale(r, g, b);
            }
            
            // Map pixel value to ASCII character
            int char_index = pixel_value * ASCII_CHARS_LENGTH / 256;
            if (char_index >= ASCII_CHARS_LENGTH) char_index = ASCII_CHARS_LENGTH - 1;
            
            ascii_art[ascii_index++] = ASCII_CHARS[char_index];
        }
        ascii_art[ascii_index++] = '\n';
    }
    
    ascii_art[ascii_index] = '\0';
    
    // Clean up
    free(resized->data);
    free(resized);
    
    return ascii_art;
}

// Function to free image memory
void free_image(Image* img) {
    if (img) {
        if (img->data) {
            free(img->data);
        }
        free(img);
    }
}
