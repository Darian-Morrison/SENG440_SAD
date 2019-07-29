#ifndef MAIN_H
#define MAIN_H

struct Bitmap_Headers
{
    unsigned int size;
    unsigned int offset;
    unsigned int height;
    unsigned int width;
    unsigned int pixels;
    unsigned int number_of_chunks;
};

FILE * open_file(char* filename);
struct Bitmap_Headers get_header_values(FILE* fp);
int read_int_from_file(FILE * fp, int len);

#endif