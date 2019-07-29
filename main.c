#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_file.h"

unsigned char calculate_luminance(unsigned char red, unsigned char blue, unsigned char green);
char sum_of_difference(unsigned char reference, unsigned char current);

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
    unsigned char reference_luminance_pixles[reference_bmp.height][reference_bmp.width];
    unsigned char current_luminance_pixles[current_bmp.height][current_bmp.width];

    fseek( reference_fp, reference_bmp.offset, SEEK_SET );
    fseek( current_fp, current_bmp.offset, SEEK_SET );

    unsigned char red, blue, green = 0;
    int j;

    for(i = 0; i < reference_bmp.height; i++){
        for(j = 0; j < reference_bmp.width; j++){
        //reference
            blue = fgetc(reference_fp);
            green = fgetc(reference_fp);
            red = fgetc(reference_fp);
            fgetc(reference_fp);
            reference_luminance_pixles[i][j] = calculate_luminance(red, green, blue);
        //current
            blue = fgetc(current_fp);
            green = fgetc(current_fp);
            red = fgetc(current_fp);
            fgetc(current_fp);
            current_luminance_pixles[i][j] = calculate_luminance(red, green, blue);
        }
    }
//Close Files
    fclose(reference_fp);
    fclose(current_fp);

//Perform algorithm
    int sad= 0;
    char diff;

    for(i = 0; i < 16; i++){
        for(j = 0; j < 16; j++){
            diff = reference_luminance_pixles[i][j] - current_luminance_pixles[i][j];
            if( diff < 0){
                sad -= diff;
            }else{
                sad += diff;
            }
        }
    }

    printf("SAD Block 1=%d\n", sad);

    return 0;
}

//((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
unsigned char calculate_luminance(unsigned char red, unsigned char green, unsigned char blue){

    unsigned int luminance = ((299 * red + 587 * green + blue * 114))/1000;
    return (unsigned char) luminance;
}

