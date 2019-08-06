#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "bmp_file.h"

unsigned char calculate_luminance(unsigned char red, unsigned char blue, unsigned char green);
void calculate_row_sad(unsigned char *reference_row, unsigned char *current_row, int *sad, int reference_x , int current_x);

struct Motion_Vector
{
    int x, y, sad;
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

    fseek(reference_fp, reference_bmp.offset, SEEK_SET);
    fseek(current_fp, current_bmp.offset, SEEK_SET);

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
    int sad, temp_x, temp_y, reference_y, reference_x, displacement_y, displacement_x = 0;
    int diff_a, diff_b;
    int step = 8;
    struct Motion_Vector home_vector;
    struct Motion_Vector temp_vector;

    struct Motion_Vector motion_array[reference_bmp.height/16][reference_bmp.width/16];


//For each block of reference 
    for(reference_y = 0; reference_y < reference_bmp.height; reference_y+=16){
        for(reference_x = 0; reference_x <  reference_bmp.width; reference_x+=16){
            home_vector = motion_array[reference_y/16][reference_x/16];
            home_vector.sad = INT_MAX;
          

            //home_sad
            home_vector.sad = 0;
            home_vector.x = reference_x;
            home_vector.y = reference_y;
            for(i = 0; i < 16; i++){
                calculate_row_sad(reference_luminance_pixles[reference_y + i], current_luminance_pixles[home_vector.y + i], &home_vector.sad, reference_x, home_vector.x);
            }
            step = 8;

            //Find best local sad
            while(step > 0){
                temp_vector = home_vector;

                //left sad
                temp_x = home_vector.x - step;
                if(temp_x >= 0){
                    sad = 0;
                    for(i = 0; i < 16; i++){
                        calculate_row_sad(reference_luminance_pixles[reference_y + i], current_luminance_pixles[home_vector.y + i], &sad, reference_x, temp_x);
                    }
                    if(sad < temp_vector.sad ){
                        temp_vector.sad = sad;
                        temp_vector.x = temp_x;
                        temp_vector.y = home_vector.y;
                    }   
                }
                
                //right sad
                temp_x = home_vector.x + step;
                if(temp_x <= reference_bmp.width){
                    sad = 0;
                    for(i = 0; i < 16; i++){
                        calculate_row_sad(reference_luminance_pixles[reference_y + i], current_luminance_pixles[home_vector.y + i], &sad, reference_x, temp_x);
                    }
                    if(sad < temp_vector.sad ){
                        temp_vector.sad = sad;
                        temp_vector.x = temp_x;
                        temp_vector.y = home_vector.y;
                    }      
                }
                //down sad
                temp_y = home_vector.y - step;
                if(temp_y >= 0){
                    sad = 0;
                    for(i = 0; i < 16; i++){
                        calculate_row_sad(reference_luminance_pixles[reference_y + i], current_luminance_pixles[temp_y + i], &sad, reference_x, home_vector.x);
                    }
                    if(sad < temp_vector.sad ){
                        temp_vector.sad = sad;
                        temp_vector.x = home_vector.x;
                        temp_vector.y = temp_y;
                    }   
                }
                
                //up sad
                temp_y = home_vector.y + step;
                if(temp_y <= reference_bmp.height){
                    sad = 0;
                    for(i = 0; i < 16; i++){
                        calculate_row_sad(reference_luminance_pixles[reference_y + i], current_luminance_pixles[temp_y + i], &sad, reference_x, home_vector.x);
                    }
                    if( sad < temp_vector.sad){
                        temp_vector.sad = sad;
                        temp_vector.x = home_vector.x;
                        temp_vector.y = temp_y;
                    }   
                }


                if(temp_vector.sad < home_vector.sad){
                    home_vector = temp_vector;
                }else{
                    step = step >> 1;   
                }
            }

            home_vector.x = home_vector.x - reference_x;
            home_vector.y = home_vector.y - reference_y;

            motion_array[reference_y/16][reference_x/16] = home_vector;
        }   
    }

    FILE *out_fp;
    out_fp = fopen("motion_vectors.txt","w");
    

    for(i = reference_bmp.height/16 - 1; i >= 0; i--){
        for(j = 0; j < reference_bmp.width/16; j++){
            if(motion_array[i][j].x != 0 || motion_array[i][j].y != 0){
               printf("Block (%d, %d) -> (%d, %d, %d)\n", j, i, motion_array[i][j].x, motion_array[i][j].y, motion_array[i][j].sad ); 
            }
            fprintf (out_fp, "(%2d, %2d, %d) ", motion_array[i][j].x, motion_array[i][j].y, motion_array[i][j].sad);  
        }
         fprintf (out_fp, "\n");  
    }
    fclose(out_fp);

    return 0;
}

//((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
unsigned char calculate_luminance(unsigned char red, unsigned char green, unsigned char blue){

    unsigned int luminance = ((299 * red + 587 * green + blue * 114))/1000;

    return (unsigned char) luminance;
}

void calculate_row_sad(unsigned char *reference_row, unsigned char *current_row, int *sad, int reference_x , int current_x){
    int diff_a, diff_b,j;
    for(j = 0; j < 16; j++){
    // for(j = 0; j < 16; j+=2){
        diff_a = (int) reference_row[reference_x + j] - (int) current_row[current_x + j];
        // diff_b = (int) reference_row[reference_x + j + 1] - (int) current_row[current_x + j + 1];
        if( diff_a < 0){
            (*sad) -= diff_a;
        }else{
            (*sad) += diff_a;
        }
        // if( diff_b < 0){
        //     (*sad) -= diff_b;
        // }else{
        //     (*sad) += diff_b;
        // }
    }
}

