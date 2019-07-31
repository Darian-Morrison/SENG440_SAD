#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "bmp_file.h"

unsigned char calculate_luminance(unsigned char red, unsigned char blue, unsigned char green);
char sum_of_difference(unsigned char reference, unsigned char current);

struct Motion_Vector
{
    int x, y;
};

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
    int sad, best_sad,current_y, current_x, reference_y, reference_x, displacement_y, displacement_x = 0;
    short int diff;
    struct Motion_Vector vector = {0,0};

    struct Motion_Vector motion_array[reference_bmp.height/16][reference_bmp.width/16];

//For each block of current 
    for(current_y = 0; current_y < reference_bmp.height; current_y+=16){
        for(current_x = 0; current_x <  reference_bmp.width; current_x+=16){
            best_sad = INT_MAX;
            //For every location in reference, find best_sad
            for(reference_y = 0; reference_y < reference_bmp.height - 15; reference_y++){
                for(reference_x = 0; reference_x <  reference_bmp.width - 15; reference_x++){
                    displacement_x = current_x - reference_x;
                    displacement_y = current_y - reference_y;
                    vector = motion_array[current_y/16][current_x/16];
                    //SAD
                    sad = 0;
                    for(i = 0; i < 16; i++){
                        for(j = 0; j < 16; j++){
                            diff = (short int) reference_luminance_pixles[reference_y + i][reference_x + j] - (short int) current_luminance_pixles[current_y + i][current_x + j];
                            if( diff < 0){
                                sad -= diff;
                            }else{
                                sad += diff;
                            }
                        }
                    }


                    //If best Sad replace motion array and update value
                    if(sad == best_sad && abs(displacement_x) + abs(displacement_y) < abs(vector.x) + abs(vector.y)){
                        motion_array[current_y/16][current_x/16].y = displacement_y;
                        motion_array[current_y/16][current_x/16].x = displacement_x;
                    }
                    if(sad < best_sad){
                        best_sad = sad;
                        motion_array[current_y/16][current_x/16].y = displacement_y;
                        motion_array[current_y/16][current_x/16].x = displacement_x;
                    }

                }
            }
        }
    }

    for(i = 0; i < reference_bmp.height/16; i++){
        for(j = 0; j < reference_bmp.width/16; j++){
            if(motion_array[i][j].x != 0 || motion_array[i][j].y != 0){
               printf("(Block (%d, %d) -> (%d, %d)\n", j, i, motion_array[i][j].x, motion_array[i][j].y ); 
            }  
        }
    }
//Sadd for a 16X16 block

    return 0;
}

//((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
unsigned char calculate_luminance(unsigned char red, unsigned char green, unsigned char blue){

    unsigned int luminance = ((299 * red + 587 * green + blue * 114))/1000;

    return (unsigned char) luminance;
}

