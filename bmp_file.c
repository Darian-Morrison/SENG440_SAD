
#include "bmp_file.h"
#define BLOCK_WIDTH 16

FILE * open_file(char* filename){
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

struct Bitmap_Headers get_header_values(FILE* fp){
    struct Bitmap_Headers bmp;

    fseek( fp, 0x02, SEEK_SET );
    bmp.size = read_int_from_file(fp, 4);

    fseek( fp, 0x0A, SEEK_SET );
    bmp.offset = read_int_from_file(fp, 4);

    fseek( fp, 0x12, SEEK_SET );
    bmp.width = read_int_from_file(fp, 4);

    fseek( fp, 0x16, SEEK_SET );
    bmp.height = read_int_from_file(fp, 4);

    return bmp;
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

