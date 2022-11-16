#ifndef PPM_PROCESSOR_H
#define PPM_PROCESSOR_H

#include <stdlib.h>
#include <stdio.h>
#include "BmpProcessor.h"
#include "PixelProcessor.h"
#include <string.h>
#include <unistd.h>
#include <getopt.h>


/**
 * magicNum = P6
 * whitespace
 * width-formatted as ASCII decimal
 * whitespace
 * height
 * whitespace
 * maxval
 * (single whitespace character)
 * pixel array (3 bytes each BINARY FOR THE PIXELS) => format:(r,g,b)
 * formatted top down
 * raster of pixels has:
 * #rows = height
 * #pixels in a row= width
 * If maxval < 256 1 byte otherwise 2 bytes
 *
 *
 */
typedef struct PPM_Header{
	//TODO:Finish struct
	char magicNum[2];
	//whitespace
	int width;
	//whitespace
	int  height;
	int maxval; //if maxval < 256: colors are 1 byte otherwise they`re 2
}PPM_Header;

/**
 * read PPM header of a file. Useful for converting files from BMP to PPM.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: Pointer to the destination PPM header
 */
void readPPMHeader(FILE* file, struct PPM_Header* header);
void printPPMHeader(struct PPM_Header* header);

/**
 * write PPM header of a file. Useful for converting files from BMP to PPM.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: The header made by makePPMHeader function

 */
void writePPMHeader(FILE* file, struct PPM_Header* header);

/**
 * make PPM header based on width and height. Useful for converting files from BMP to PPM.
 *
 * @param  header: Pointer to the destination PPM header
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void makePPMHeader(struct PPM_Header* header, int width, int height);

/**
 * read Pixels from PPM file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for 
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void readPixelsPPM(FILE* file, struct Pixel** pArr, int width, int height);
/**
 * write Pixels from PPM file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void writePixelsPPM(FILE* file, struct Pixel** pArr, int width, int height);

#endif //PPM_PROCESSOR_H