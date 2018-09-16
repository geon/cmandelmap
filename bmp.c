// https://stackoverflow.com/a/47785639/446536

#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

const int bytesPerPixel = 3; /// red, green, blue
const int fileHeaderSize = 14;
const int infoHeaderSize = 40;

unsigned char* createBitmapFileHeader(int height, int width);
unsigned char* createBitmapInfoHeader(int height, int width);

void loadBitmapImage(unsigned char **image, int *height, int *width, char* imageFileName){

    unsigned char* fileHeader = malloc(fileHeaderSize);
    unsigned char* infoHeader = malloc(infoHeaderSize);

    FILE* imageFile = fopen(imageFileName, "rb");

    fread(fileHeader, 1, fileHeaderSize, imageFile);
    fread(infoHeader, 1, infoHeaderSize, imageFile);

    fprintf( stdout, "compression. %u, %u, %u, %u\n", infoHeader[16], infoHeader[17], infoHeader[18], infoHeader[19]);

    fprintf( stdout, "Width. %u, %u, %u, %u\n", infoHeader[4], infoHeader[5], infoHeader[6], infoHeader[7]);
    fprintf( stdout, "Height. %u, %u, %u, %u\n", infoHeader[8], infoHeader[9], infoHeader[10], infoHeader[11]);

    *width = 
        ((unsigned int) infoHeader[4]) |
        ((unsigned int) infoHeader[5]) << 8 |
        ((unsigned int) infoHeader[6]) << 16 |
        ((unsigned int) infoHeader[7]) << 24;
    
    *height = 
        ((unsigned int) infoHeader[8]) |
        ((unsigned int) infoHeader[9]) << 8 |
        ((unsigned int) infoHeader[10]) << 16 |
        ((unsigned int) infoHeader[11]) << 24;

    // Some images are stored upside-down, sigified by a negative height.
    if(*height < 0){
        *height = -*height;
    }

    const int  dataSize = *width * *height * 4;
    fprintf( stdout, "Size. %i\n", dataSize);

    *image = malloc(dataSize);
    fread(*image, dataSize, 1, imageFile);
    fclose(imageFile);
}

void generateBitmapImage(unsigned char *image, int height, int width, char* imageFileName){

    unsigned char* fileHeader = createBitmapFileHeader(height, width);
    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4-(width*bytesPerPixel)%4)%4;

    FILE* imageFile = fopen(imageFileName, "wb");

    fwrite(fileHeader, 1, fileHeaderSize, imageFile);
    fwrite(infoHeader, 1, infoHeaderSize, imageFile);

    int i;
    for(i=0; i<height; i++){
        fwrite(image+(i*width*bytesPerPixel), bytesPerPixel, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}

unsigned char* createBitmapFileHeader(int height, int width){
    int fileSize = fileHeaderSize + infoHeaderSize + bytesPerPixel*height*width;

    static unsigned char fileHeader[] = {
        0,0, /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize    );
    fileHeader[ 3] = (unsigned char)(fileSize>> 8);
    fileHeader[ 4] = (unsigned char)(fileSize>>16);
    fileHeader[ 5] = (unsigned char)(fileSize>>24);
    fileHeader[10] = (unsigned char)(fileHeaderSize + infoHeaderSize);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width){
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0, /// number of color planes
        0,0, /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[ 0] = (unsigned char)(infoHeaderSize);
    infoHeader[ 4] = (unsigned char)(width    );
    infoHeader[ 5] = (unsigned char)(width>> 8);
    infoHeader[ 6] = (unsigned char)(width>>16);
    infoHeader[ 7] = (unsigned char)(width>>24);
    infoHeader[ 8] = (unsigned char)(height    );
    infoHeader[ 9] = (unsigned char)(height>> 8);
    infoHeader[10] = (unsigned char)(height>>16);
    infoHeader[11] = (unsigned char)(height>>24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(bytesPerPixel*8);

    return infoHeader;
}