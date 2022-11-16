#ifndef PIXEL_PROCESSOR_H
#define PIXEL_PROCESSOR_H



/**
 * Represents a pixel with 3 colors in the layout (b,g,r)
 * -for the 24 bits per pixel format only
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include<math.h>
#include "ThreadInfo.h"

typedef struct Pixel{
	//TODO:Finish struct
	unsigned char b;
	unsigned char g;
	unsigned char r;
}Pixel;

/**
 * Shift color of Pixel array. The dimension of the array is width * height. The shift value of RGB is 
 * rShift, gShift，bShift. Useful for color shift.
 *
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 * @param  rShift: the shift value of color r shift
 * @param  gShift: the shift value of color g shift 
 * @param  bShift: the shift value of color b shift 
 */
void colorShiftPixels(struct Pixel** pArr, int width, int height, int rShift, int gShift, int bShift);

/**
 * Prints the pixel array given a reference to the arr
 * @param arrRef reference to the Pixel array
 * @param width
 * @param height
 */
void colorShiftPixelsOffset(struct Pixel** pArr, int rShift, int gShift, int bShift,threadInfo info);
/*helper function for colorSjiftPixels*/
unsigned char calculateColor(int color);

void boxBlur(struct Pixel** arrRef, int width, int height);
void applyCheeseFilter(struct Pixel** pArr,  threadInfo info);
void singleCheeseFilter(struct Pixel** pArr, int width, int height);
void multiThreadedBoxBlur(struct Pixel** arrRef, threadInfo info);
void multiThreadedBoxBlurNoBuffer(struct Pixel** arrRef,  threadInfo info);
void printPixels(struct Pixel** arrRef, int width, int height);
int calculateRadius(int averageHoleSize);
int calculateCoordinate( int heightOrWidth);
int distance(float x1, float y1, float x2,float y2);

/**
 * Flips a raster of pixels so that it starts at the bottom instead of the top or vice versa.
 * bmp -> left to right & bottom to top
 * ppm -> left to right & bottom to top
 * @param arrRef reference to the Pixel array
 * @param width
 * @param height
 */
void flipPixels(struct Pixel** arrRef, int width, int height);

/**
 *  Swaps the place of the red and blue pixel
 * BMP => (b,g,r)
 * PPM => (r,g,b)
 * @param arrRef
 * @param width
 * @param height
 */
void PixelSwap(struct Pixel** arrRef, int width, int height);
#endif //PIXEL_PROCESSOR_H