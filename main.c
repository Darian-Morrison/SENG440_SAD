#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_file.h"

unsigned char calculate_luminance(unsigned char red, unsigned char blue, unsigned char green);
void populate_luminance_arrays(unsigned char* ,FILE* fp, struct Bitmap_Headers bmp);

int main(int argc, char *argv[]){

    int i, size;
    if(argc != 3){
        printf("Incorrect number of arguments\n");
        return 0;
    }
    char *filename = argv[1];

    FILE *reference_fp;
    FILE *current_fp;
//Open Files
    reference_fp = open_file(argv[1]);
    current_fp = open_file(argv[2]);

//Get Headers
    struct Bitmap_Headers reference_bmp = get_header_values(reference_fp);
    struct Bitmap_Headers current_bmp = get_header_values(current_fp);

//Convert Data to Luminance Pixels
    unsigned char reference_luminance_pixles[reference_bmp.pixels];
    unsigned char current_luminance_pixles[current_bmp.pixels];

    populate_luminance_arrays(reference_luminance_pixles, reference_fp, reference_bmp);
    populate_luminance_arrays(current_luminance_pixles, current_fp, current_bmp);

//Perform algorithm

//Close Files
    fclose(reference_fp);
    fclose(current_fp);

    return 0;
}

//((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
unsigned char calculate_luminance(unsigned char red, unsigned char green, unsigned char blue){

    unsigned int luminance = ((299 * red + 587 * green + blue * 114))/1000;
    return (unsigned char) luminance;
}

void populate_luminance_arrays(unsigned char* luminance_pixles,FILE* fp, struct Bitmap_Headers bmp){
    fseek( fp, bmp.offset, SEEK_SET );

    unsigned char red = 0;
    unsigned char blue = 0;
    unsigned char green = 0;

    int i;

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
}