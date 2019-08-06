
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Bitmap_Headers
{
    unsigned int size;
    unsigned int offset;
    unsigned int height;
    unsigned int width;
};

FILE * open_file(char* filename);
struct Bitmap_Headers get_header_values(FILE* fp);
int read_int_from_file(FILE * fp, int len);
