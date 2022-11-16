#ifndef BMP_C
#define BMP_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "BmpProcessor.h"
#include "PixelProcessor.h"

/**
 *
 * read BMP header of a file. Useful for converting files from PPM to BMP.
 * 14 BYTES
 * @param  file: A pointer to the file being read or written
 * @param  header: Pointer to the destination BMP header
 */
void readBMPHeader(FILE* file, struct BMP_Header* header){
    fread(&(header->signature[0]), sizeof(char), 1, file);
    fread(&(header->signature[1]), sizeof(char), 1, file);
    fread(&(header->size), sizeof(int), 1, file);
    fread(&(header->reserved1), sizeof(short), 1, file);
    fread(&(header->reserved2), sizeof(short), 1, file);
    fread(&(header->offset), sizeof(int), 1, file);
}
void printBMPHeader(FILE* file, struct BMP_Header* header){
    printf("\nBMP_HEADER INFORMATION:\n");
    printf("signature: %c%c\n", header->signature[0], header->signature[1]);
    printf("size: %d\n", header->size);
    printf("reserved1: %d\n", header->reserved1);
    printf("reserved2: %d\n", header->reserved2);
    printf("offset_pixel_array: %d\n", header->offset);
}

/**
 * read DIB header from a file. Useful for converting files from PPM to BMP.
 *40 BYTES
 * @param  file: A pointer to the file being read or written
 * @param  header: Pointer to the destination DIB header
 * int size;
	int width;
	int height;
	short planes;
	short bpp;
	int compression;
	int imageSize;
	int xppm;
	int yppm;
	int colors;
	int icc;
 */
void readDIBHeader(FILE* file, struct DIB_Header* header){
    fread(&(header->size), sizeof(int), 1, file);
    fread(&(header->width), sizeof(int), 1, file);
    fread(&(header->height), sizeof(int), 1, file);
    fread(&(header->planes), sizeof(short), 1, file);
    fread(&(header->bpp), sizeof(short), 1, file);
    fread(&(header->compression), sizeof(int), 1, file);
    fread(&(header->imageSize), sizeof(int), 1, file);
    fread(&(header->xppm), sizeof(int), 1, file);
    fread(&(header->yppm), sizeof(int), 1, file);
    fread(&(header->colors), sizeof(int), 1, file);
    fread(&(header->icc), sizeof(int), 1, file);
}
void printDIBHeader(FILE* file, struct DIB_Header* header){
    printf("\nDIB_HEADER INFORMATION:\n");
    printf("size: %d\n", header->size);
    printf("width: %d\n", header->width);
    printf("height: %d\n", header->height);
    printf("planes: %d\n", header->planes);
    printf("bpp: %d\n", header->bpp);
    printf("compression: %d\n", header->compression);
    printf("imageSize: %d\n", header->imageSize);
    printf("x pixels per meter: %d\n", header->xppm);
    printf("y pixels per meter: %d\n", header->yppm);
    printf("colors: %d\n", header->colors);
    printf("icc: %d\n", header->icc);
}
/**
 * helper function to calcualte the padding
 * @param bitCounter
 * @return padding size in bits
 */
/**
 *
 * read Pixels from BMP file based on width and height.
 * Were going to treat it like a 1D representation of a 2D array.
 * Assuming that we`re already in the correct position in the file.
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */

void readPixelsBMP(FILE* file, struct Pixel** pArr, int width, int height){
    //Assuming FSEEK was already offset to the correct point in the file
    //somewhere else in the program
    int sz = width * 3;
    int padding = 0;
    while(sz %4 != 0){
        padding++;
        sz++;
    }
    printf("The padding is:%d\n",padding);
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            fread(&((*pArr)[i*width + j].b), sizeof(unsigned char), 1, file);
            fread(&((*pArr)[i*width + j].g), sizeof(unsigned char), 1, file);
            fread(&((*pArr)[i*width + j].r), sizeof(unsigned char), 1, file);
        }
        //returns the # of bits to move forward in the file
        fseek(file,padding*(int)sizeof(char),SEEK_CUR);
        //printf("\nPad amount is %d",padAmount);
    }
}//ends readPixelsBMP


/**
 * write Pixels from BMP file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void writePixelsBMP(FILE* file, struct Pixel** pArr, int width, int height){
    //Assuming FSEEK was already offset to the correct point in the file
    //somewhere else in the program
    int pixSize = width * 3;
    int sz = pixSize;
    int padding = 0;
    char nullChar = '\0';
    while(sz %4 != 0){
        padding++;
        sz++;
    }
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            fwrite(&((*pArr)[i*width + j].b), sizeof(unsigned char), 1, file);
            fwrite(&((*pArr)[i*width + j].g), sizeof(unsigned char), 1, file);
            fwrite(&((*pArr)[i*width + j].r), sizeof(unsigned char), 1, file);
        }
        //returns the # of bits to move forward in the file
        //writing in padding
        for(int i = 0; i < padding; i++)
            fwrite(&(nullChar), sizeof(unsigned char), 1, file);
    }
}

/**
 * write BMP header of a file. Useful for converting files from PPM to BMP.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: The header made by makeBMPHeader function
 */
void writeBMPHeader(FILE* file, struct BMP_Header* header){
    fwrite(&(header->signature[0]), sizeof(char), 1, file);
    fwrite(&(header->signature[1]), sizeof(char), 1, file);
    fwrite(&(header->size), sizeof(int), 1, file);
    fwrite(&(header->reserved1), sizeof(short), 1, file);
    fwrite(&(header->reserved2), sizeof(short), 1, file);
    fwrite(&(header->offset), sizeof(int), 1, file);
}

/**
 * write DIB header of a file. Useful for converting files from PPM to BMP.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: The header made by makeDIBHeader function
 */
void writeDIBHeader(FILE* file, struct DIB_Header* header){
    fwrite(&(header->size), sizeof(int), 1, file);
    fwrite(&(header->width), sizeof(int), 1, file);
    fwrite(&(header->height), sizeof(int), 1, file);
    fwrite(&(header->planes), sizeof(short), 1, file);
    fwrite(&(header->bpp), sizeof(short), 1, file);
    fwrite(&(header->compression), sizeof(int), 1, file);
    fwrite(&(header->imageSize), sizeof(int), 1, file);
    fwrite(&(header->xppm), sizeof(int), 1, file);
    fwrite(&(header->yppm), sizeof(int), 1, file);
    fwrite(&(header->colors), sizeof(int), 1, file);
    fwrite(&(header->icc), sizeof(int), 1, file);
}

/**
 * tester function
 */
void hello(){
    printf("\nHello from BMPPROCCESSOR.C\n");
}

/**
 * make BMP header based on width and height.
 * The purpose of this is to create a new BMPHeader struct using the information
 * from a PPMHeader when converting from PPM to BMP.
 *
 * @param  header: Pointer to the destination BMP header
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 *
 *

 typedef struct PPM_Header{
	//TODO:Finish struct
	char magicNum[2];
	//whitespace
	int width;
	//whitespace
	int  height;
	int maxval; //if maxval < 256: colors are 1 byte otherwise they`re 2
}PPM_Header;

typedef struct BMP_Header {
	//TODO:Finish struct
	char signature[2];
	int size;
	short reserved1;
	short reserved2;
	int offset;
}BMP_Header;
 */
void makeBMPHeader(struct BMP_Header* header, int width, int height){
    header->signature[0] = 'B'; //identifying start of file never changes
    header->signature[1] = 'M';
    header->size = sizeof(BMP_Header) + sizeof(DIB_Header) + sizeof(Pixel)*width*height; //size of the whole thing
    header->reserved1 = 0; //idk just copying these from all the other results
    header->reserved2 = 0;
    header->offset = 54; //14 for the BMP_Header and 40 for the DIB_Header
}//ends makeBMPHeader

/**
* Makes new DIB header based on width and height. Useful for converting files from PPM to BMP.
*
* @param  header: Pointer to the destination DIB header
* @param  width: Width of the image that this header is for
* @param  height: Height of the image that this header is for
 * typedef struct DIB_Header{
	//TODO:Finish struct
	int size;
	int width; //width of the pixel array in pixels
	int height; //height (in pixels) of array of pix
	short planes;
	short bpp;
	int compression;
	int imageSize;
	int xppm;
	int yppm;
	int colors;
	int icc;
}DIB_Header;
*/
void makeDIBHeader(struct DIB_Header* header, int width, int height){
    header->size = 40; //size of the DIB_Header
    header->width = width; //same as in PPM
    header->height = height; //same as in PPM
    header->planes = 1; //only one color plane used
    header->bpp = 24; //8 bits per color, 8*3 = 24 bits per pixel
    header->compression = 0; //no compression used
    header->imageSize = sizeof(Pixel)*width*height; //bytes size of raw data
    header->xppm = 2835; //copying from example 1 in wikipedia?
    header->yppm = 2835; //same
    header->colors = 0; //#of colors in the palette
    header->icc = 0; //0 means all colors are important
}
#endif //BMP_C