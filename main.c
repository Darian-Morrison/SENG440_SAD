#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_file.h"

unsigned char calculate_luminance(unsigned char red, unsigned char blue, unsigned char green);

int main(int argc, char *argv[]){

    int i, size;
    if(argc != 3){
        printf("Incorrect number of arguments\n");
        return 0;
    }
    char *filename = argv[1];

    FILE *fp;

    fp = open_file(filename);

    struct Bitmap_Headers bmp = get_header_values(fp);


    //Handle data
    fseek( fp, bmp.offset, SEEK_SET );

    unsigned char red = 0;
    unsigned char blue = 0;
    unsigned char green = 0;

    unsigned char luminance_pixles[bmp.pixels];

    
    for(i = 0; i < bmp.pixels; i++){
        
        unsigned char blue = fgetc(fp);
        unsigned char green = fgetc(fp);
        unsigned char red = fgetc(fp);
        unsigned char alpha = fgetc(fp);
        luminance_pixles[i] = calculate_luminance(red, green, blue);
        // int temp_pixel = 0;
        // temp_pixel += (unsigned int) fgetc(fp) << 24; 
        // temp_pixel += (unsigned int) fgetc(fp) << 16;
        // temp_pixel += (unsigned int) fgetc(fp) << 8;
        // temp_pixel += (unsigned int) fgetc(fp);
    }

    printf("%X\n", luminance_pixles[bmp.pixels - 1]);

    printf("Size: %d x %d\n", bmp.width, bmp.height);
    printf("Blocks: %d\n", bmp.number_of_chunks);
    printf("Count: %d\n", bmp.pixels);

    fclose(fp);

    return 0;
}

//((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
unsigned char calculate_luminance(unsigned char red, unsigned char green, unsigned char blue){

    unsigned int luminance = ((299 * red + 587 * green + blue * 114))/1000;
    return (unsigned char) luminance;
}   