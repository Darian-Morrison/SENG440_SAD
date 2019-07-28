#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE * read_file(char* filename);
int read_int_from_file(FILE * fp, int len);
unsigned char calculate_luminance(unsigned char red, unsigned char blue, unsigned char green);

int main(int argc, char *argv[]){

    int i, size;
    if(argc != 3){
        printf("Incorrect number of arguments\n");
        return 0;
    }
    char *filename = argv[1];

    FILE *fp;

    fp = read_file(filename);

    //find bmp info
    fseek( fp, 0x02, SEEK_SET );
    int size_of_file = read_int_from_file(fp, 4);

    fseek( fp, 0x0A, SEEK_SET );
    int offset = read_int_from_file(fp, 4);

    fseek( fp, 0x12, SEEK_SET );
    int width = read_int_from_file(fp, 4);

    fseek( fp, 0x16, SEEK_SET );
    int height = read_int_from_file(fp, 4);

    //Divide by bytes per pixel(4)
    unsigned int pixels = (size_of_file - offset) >> 2;
    unsigned int chunk_size = 16*16;
    unsigned int number_of_chunks = pixels/chunk_size;

    
    fseek( fp, offset, SEEK_SET );

    unsigned char red = 0;
    unsigned char blue = 0;
    unsigned char green = 0;

    unsigned char luminance_pixles[pixels];

    
    for(i = 0; i < 1; i++){
        
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
//function
    printf("%X\n", luminance_pixles[0]);

    printf("Size: %d x %d\n", width, height);
    printf("Blocks: %d\n", number_of_chunks);
    printf("Count: %d\n", pixels);

    fclose(fp);

    return 0;
}

//((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
unsigned char calculate_luminance(unsigned char red, unsigned char green, unsigned char blue){

    unsigned int luminance = ((299 * red + 587 * green + blue * 114))/1000;
    return (unsigned char) luminance;
}   


int read_int_from_file(FILE * fp, int len){
    if(len > 4){
        fprintf(stderr,"Too many bytes to store in intger (1-4)\n");
        exit(1);
    }
    int i;

    unsigned char data[len + 1];

    fread(data, sizeof(unsigned char), len, fp);

    unsigned int num = 0;
    unsigned int temp = 0;

    for(i = 0; i < len; i++ ){
        temp = (unsigned int) data[i] << (i * 8);
        num += temp;
    }

    return num;
}

FILE * read_file(char* filename){
    FILE *fp;
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr,"Failed to fopen %s\n", filename);
        exit(1);
    }
    //Read Filetype
    char inNums[3];
    fread(inNums,sizeof(char),2,fp);

    //Confirm filetype is bitmap
    if((strncmp(inNums,"BM",2)!=0)){
        fprintf(stderr,"Wrong file type for transformation\n");
        exit(1);
    }

    return fp;
}