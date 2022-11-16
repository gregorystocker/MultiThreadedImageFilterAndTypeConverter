#include "PixelProcessor.h"

//
// Created by greg on 8/25/21.
//


/**
 * Prints the pixel array given a reference to the arr
 * @param arrRef reference to the Pixel array
 * @param width
 * @param height
 */
void printPixels(struct Pixel **arrRef, int width, int height) {
    printf("\nPixels:\n");
    for (int i = 0; i < height; i++) {
        printf("[");
        for (int j = 0; j < width; j++) {
            printf("(b=%d, g=%d, r=%d)",
                   (int) (*arrRef)[i * width + j].b,
                   (int) (*arrRef)[i * width + j].g,
                   (int) (*arrRef)[i * width + j].r
            );
        }
        printf("]\n");
    }
    printf("\n");
}//ends printPixels

/**
 * Flips a raster of pixels so that it starts at the bottom instead of the top or vice versa.
 * @param arrRef reference to the Pixel array
 * @param width
 * @param height
 */
void flipPixels(struct Pixel **arrRef, int width, int height) {
    Pixel *buffer = (Pixel *) malloc(sizeof(Pixel) * width * height);
    //copy over to the buffer in reverse with respect to the rows
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            buffer[i * width + j].b = (*arrRef)[(height - (1 + i)) * width + j].b;
            buffer[i * width + j].g = (*arrRef)[(height - (1 + i)) * width + j].g;
            buffer[i * width + j].r = (*arrRef)[(height - (1 + i)) * width + j].r;
        }
    }
    //copy straight back over from buffer to the original
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            (*arrRef)[i * width + j].b = buffer[i * width + j].b;
            (*arrRef)[i * width + j].g = buffer[i * width + j].g;
            (*arrRef)[i * width + j].r = buffer[i * width + j].r;
        }
    }
    free(buffer);
    buffer = NULL;
}//flipPixels


/**
 * Shift color of Pixel array. The dimension of the array is width * height. The shift value of RGB is
 * rShift, gShift，bShift. Useful for color shift. Clamped to 0 or 255
 *
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 * @param  rShift: the shift value of color r shift
 * @param  gShift: the shift value of color g shift
 * @param  bShift: the shift value of color b shift
 */
void colorShiftPixels(struct Pixel** pArr, int width, int height, int rShift, int gShift, int bShift){
    // oColor represents the original colors as integers
    // pColor represents the potential value after shift is applied (could be a bad value)
    // fColor represents the final result of the shift
    int oBlue, oGreen, oRed, pBlue, pGreen, pRed;
    unsigned char fBlue, fGreen, fRed;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            //calculate integer values of the original color
            oBlue = (int)(*pArr)[i*width + j].b;
            oGreen = (int)(*pArr)[i*width + j].g;
            oRed = (int)(*pArr)[i*width + j].r;
            //calculate the potential colors
            pBlue = oBlue + bShift;
            pGreen = oGreen + gShift;
            pRed = oRed + rShift;
            //calculate what the final color will be
            fBlue = calculateColor(pBlue);
            fGreen = calculateColor(pGreen);
            fRed = calculateColor(pRed);


            //assign this new color back to the array
            (*pArr)[i*width + j].b = fBlue;
            (*pArr)[i*width + j].g = fGreen;
            (*pArr)[i*width + j].r = fRed;
        }//ends j loop
    }//ends i loop
}//ends colorShiftPixels

/**
 * Shift color of Pixel array. The dimension of the array is width * height. The shift value of RGB is
 * rShift, gShift，bShift. Useful for color shift. Clamped to 0 or 255
 *
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 * @param  rShift: the shift value of color r shift
 * @param  gShift: the shift value of color g shift
 * @param  bShift: the shift value of color b shift
 */

//colorShiftPixelsOffset(struct Pixel** pArr, int width, int height, int rShift, int gShift, int bShift,threadInfo info);
void colorShiftPixelsOffset(struct Pixel** pArr,int rShift, int gShift, int bShift, threadInfo info){
    // oColor represents the original colors as integers
    // pColor represents the potential value after shift is applied (could be a bad value)
    // fColor represents the final result of the shift
    printf("\nOriginalWidth:%d\tColWidth:%d\tOriginalHeight:%d\tstart:%d\tend:%d\tthreadCount:%d\n",
           info.originalWidth, info.colWidth, info.originalHeight,info.start, info.end,info.threadNo);
    int oBlue, oGreen, oRed, pBlue, pGreen, pRed;
    unsigned char fBlue, fGreen, fRed;
    int width = info.originalWidth;
    int height = info.originalHeight;

    printf("\nHello from colorShift Pixels Offset!\n"
           "Start is:%d\tEnd is:%d\tcolWidth is:%d\n", info.start,info.end,info.colWidth);
    int colWidth = info.colWidth;
    for(int i = 0; i < height; i++){
        for(int j = info.start; j < info.end; j++){
            if(j < 0 || j >= width){
                break;
            }
            //calculate integer values of the original color
            oBlue = (int)(*pArr)[i*width + j].b;
            oGreen = (int)(*pArr)[i*width + j].g;
            oRed = (int)(*pArr)[i*width + j].r;
            //calculate the potential colors
            pBlue = oBlue + bShift;
            pGreen = oGreen + gShift;
            pRed = oRed + rShift;
            //calculate what the final color will be
            fBlue = calculateColor(pBlue);
            fGreen = calculateColor(pGreen);
            fRed = calculateColor(pRed);

            //assign this new color back to the array
            (*pArr)[i*width + j].b = fBlue;
            (*pArr)[i*width + j].g = fGreen;
            (*pArr)[i*width + j].r = fRed;
        }//ends j loop
    }//ends i loop
}//ends colorShiftPixels


void applyCheeseFilter(struct Pixel** pArr, threadInfo info){
    // oColor represents the original colors as integers
    // pColor represents the potential value after shift is applied (could be a bad value)
    // fColor represents the final result of the shift
    printf("\nOriginalWidth:%d\tColWidth:%d\tOriginalHeight:%d\tstart:%d\tend:%d\tthreadCount:%d\n",
           info.originalWidth, info.colWidth, info.originalHeight,info.start, info.end,info.threadNo);

    int width = info.originalWidth;
    int height = info.originalHeight;

    printf("\nHello from apply cheese filter!\n"
           "Start is:%d\tEnd is:%d\tcolWidth is:%d\n", info.start,info.end,info.colWidth);
    int colWidth = info.colWidth;

    //apply the yellow base
    for(int i = 0; i < height; i++){
        for(int j = info.start; j < info.end; j++){
            (*pArr)[i*width + j].b = calculateColor( (*pArr)[i*width + j].b - 50);
            (*pArr)[i*width + j].r = calculateColor((*pArr)[i*width + j].r + 50);
            (*pArr)[i*width + j].g = calculateColor( (*pArr)[i*width + j].g + 50);
            //(*pArr)[i*width + j].b -=50;
        }//ends j loop
    }//ends i loop

    //PICTURE IS YELLOW
    //ADD HOLES
}//ends cheeseFilter

/**
 * Helper for getting the distance between 2 points. Returns number rounded to an int
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @return distance between (x1,y1) and (x2,y2)
 */
int distance(float x1, float y1, float x2,float y2){
    int distance = abs((int)sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)));
    return distance;
}

void singleCheeseFilter(struct Pixel** pArr, int width, int height){
    // oColor represents the original colors as integers
    // pColor represents the potential value after shift is applied (could be a bad value)
    // fColor represents the final result of the shift
    //apply the yellow base
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            (*pArr)[i*width + j].b = calculateColor( (*pArr)[i*width + j].b - 50);
            (*pArr)[i*width + j].r = calculateColor((*pArr)[i*width + j].r + 50);
            (*pArr)[i*width + j].g = calculateColor( (*pArr)[i*width + j].g + 50);
            //(*pArr)[i*width + j].b -=50;
        }//ends j loop
    }//ends i loop
    //PICTURE IS YELLOW
    //ADD HOLES

    srand(time(NULL));
    //The number of holes should also be 10% of the
    //smallest side (e.g., if an image is 130 pixels, then there will be 13 holes
    int smallestSize;
    if(width < height){
        smallestSize = width;
    }else{
        smallestSize = height;
    }
    int numHoles = smallestSize/10;
    Hole* holes = (malloc(sizeof(Hole)*numHoles));
    int averageHoleSize = smallestSize/10;
    int radius;
    //calculate the offset from the averageSize
    for(int i = 0; i < numHoles; i++) {
        holes[i].r = calculateRadius(averageHoleSize);
        holes[i].x = calculateCoordinate(width);
        holes[i].y = calculateCoordinate(height);
    }//ends for i
    //draw all the circles "inefficciently
    //loop through all the pixels
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            //loop through all Holes
            for(int holeIdx = 0; holeIdx < numHoles; holeIdx++){
                //check if the distance between the test point and the hole`s center is within the radius of the circle
                int distanceToCenter = distance((float)holes[holeIdx].x,(float)holes[holeIdx].y,(float)i,(float)j);
                if(distanceToCenter <= holes[holeIdx].r){
                    //color the pixel black
                    (*pArr)[i * width + j].r = 0;
                    (*pArr)[i * width + j].b = 0;
                    (*pArr)[i * width + j].g = 0;
                }
            }
        }//ends j
    }//ends i
}//ends cheeseFilter




/**
 * Estimates a normal distribution with numbers becoming less common as they get away from the average case
 * @param averageHoleSize the average case. Should be smallestSize/10.
 * @return a radius as the result of the calculation
 */
int calculateRadius(int averageHoleSize){

    //calculate the sign of the offset from averageHoleSize
    int sign = 1; //is either -1 or 1 50% of the time, controls the direction of offset from the average
    int signFeed;
    signFeed = rand() % 10;
    if (signFeed < 5) {
        sign = -1;
    } else {
        sign = 1;
    }
    int min,max;
    int offsetPercent;
    int feedCategory = rand() % 10;
    if (feedCategory <= 5) {
        min = 0;
        max = 30;
    } else if (feedCategory <= 7) {
        min = 30;
        max = 60;
    }
    else if (feedCategory <= 9) {
        min = 7;
        max = 60;
    }else {
       min = 60;
       max = 70;
    }


    offsetPercent =  (rand() %(max - min + 1)) + min;
    float offsetFloat = ((float)offsetPercent)/100;
    return (int)(averageHoleSize + (sign*offsetFloat*averageHoleSize));
}//ends calculate radius

int calculateCoordinate(int heightOrWidth){

    //calculate the sign of the offset from averageHoleSize
    int sign = 1; //is either -1 or 1 50% of the time, controls the direction of offset from the average
    int signFeed;
    signFeed = rand() % 10;
    if (signFeed < 5) {
        sign = -1;
    } else {
        sign = 1;
    }
    int offsetPercent = rand()% 45;

    float offsetFloat = ((float)offsetPercent)/100;
    return (int)((heightOrWidth/2) + (sign*offsetFloat*heightOrWidth));
}

/**
 * Applies a box-blur algorithm.The algo adds the pixels to the top,bottom, left, right and center (itself) of a pixel
 * @param arrRef pixel array
 * @param width
 * @param height
 */
void boxBlur(struct Pixel** arrRef, int width, int height){
    //these will hold the average of each color for each 'box' of pixels
    unsigned char newBlue, newGreen, newRed;
    int numPixelsUsed = 0;
    Pixel top, bottom, right, left;

    //this will act as a "fresh copy" of the original pixels to read from so that we aren`t "re-blurring" them
    Pixel* buffer = malloc(sizeof(Pixel)*width*height);
    //allocate buffer
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            (buffer)[i * width + j].r  = (*arrRef)[i * width + j].r;
            (buffer)[i * width + j].g = (*arrRef)[i * width + j].g;
            (buffer)[i * width + j].b = (*arrRef)[i * width + j].b;
        }
    }//end allocating buffer
    //calculate the padding
    int sz = width * 3;
    int padding = 0;
    while(sz %4 != 0){
        padding++;
        sz++;
    }

    //apply the blur
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            //we dont want to do the last pixel as we cant add to the right of it

            //record the top pixel
            if(i < height - 1){
                top = (buffer)[(i + 1)*width + j];
                numPixelsUsed++;
            }else { //if not initialize it to an empty pixel so it doesnt effect calculations
                top.b=0;
                top.r=0;
                top.g=0;
            }
            //record the bottom pixel
            if(i > 0){
                bottom =  (buffer)[(i - 1)*width + j];
            }else{ //if not initialize it to an empty pixel so it doesn`t effect calculations
                //dont need to increment numPixelUsed because were essentially adding nothing here
                bottom.b=0;
                bottom.r=0;
                bottom.g=0;
            }
            //record the right pixel
            if(j < width - 1){
                numPixelsUsed++;
                right = (buffer)[i*width + (j + 1)];
            }else{ //if not initialize it to an empty pixel so it doesnt effect calculations
               right.b=0;
                right.r=0;
                right.g=0;
            }
            //record the left pixel
            if(j > 0) {
                left = (buffer)[i*width + (j - 1)];
                numPixelsUsed++;
            }
            else{ //if not initialize it to an empty pixel so it doesnt effect calculations
                left.b=0;
             left.r=0;
                left.g=0;
            }
            //average all pixel colors for this box:  (original pixel + left + right + top + bottom `s colors)/numPixelsUsed
            newRed = calculateColor( ((buffer)[i*width + j].r + left.r + right.r + top.r + bottom.r)/numPixelsUsed );
            newBlue = calculateColor( ((buffer)[i*width + j].b + left.b + right.b + top.b + bottom.b)/numPixelsUsed );
            newGreen = calculateColor( ((buffer)[i*width + j].g + left.g + right.g + top.g + bottom.g)/numPixelsUsed) ;
            (*arrRef)[i*width + j].r = newRed;
            (*arrRef)[i*width + j].g = newGreen;
            (*arrRef)[i*width + j].b = newBlue;
            //reset numPixelsUsed for averaging the next pixel
            numPixelsUsed = 0;
        }//end s j loop
    }//ends i loop

    free(buffer);
}//ends boxBlur

/**
 * applies the box blur, but for use with a multi-threaded program
 * @param arrRef
 * @param width
 * @param height
 */
void multiThreadedBoxBlur(struct Pixel** arrRef,  threadInfo info){

    int width = info.originalWidth;
    int height = info.originalHeight;
    int start = info.start;
    int end = info.end;
    //these will hold the average of each color for each 'box' of pixels
    unsigned char newBlue, newGreen, newRed;
    int numPixelsUsed = 0;
    Pixel top, bottom, right, left;
    //this will act as a "fresh copy" of the original pixels to read from so that we aren`t "re-blurring" them
    Pixel buffer[width*height];
    //allocate buffer
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            (buffer)[i * width + j].r = (*arrRef)[i * width + j].r;
            (buffer)[i * width + j].g = (*arrRef)[i * width + j].g;
            (buffer)[i * width + j].b = (*arrRef)[i * width + j].b;
        }
    }


    for(int i = 0; i < height; i++){
        for(int j = start; j < end; j++){
            //we dont want to do the last pixel as we cant add to the right of it
            //record the top pixel
            if(i < (height - 1)){
                top = (buffer)[(i + 1)*width + j];
                numPixelsUsed++;
            }else { //if not initialize it to an empty pixel so it doesnt effect calculations
                top.b=0;
                top.r=0;
                top.g=0;
            }
            //record the bottom pixel
            if(i > 0){
                bottom =  (buffer)[(i - 1)*width + j];
            }else{ //if not initialize it to an empty pixel so it doesn`t effect calculations
                //dont need to increment numPixelUsed because were essentially adding nothing here
                bottom.b=0;
                bottom.r=0;
                bottom.g=0;
            }
            //record the left pixel
            if(j < (end -1)){
                numPixelsUsed++;
                right = (buffer)[i*width + (j + 1)];
            }else{ //if not initialize it to an empty pixel so it doesnt effect calculations
                right.b=0;
                right.r=0;
                right.g=0;
            }
            //record the left pixel
            if(j > 0) {
                left = (buffer)[i*width + (j - 1)];
                numPixelsUsed++;
            }
            else{ //if not initialize it to an empty pixel so it doesnt effect calculations
                left.b=0;
                left.r=0;
                left.g=0;
            }
            //average all pixel colors for this box:  (original pixel + left + right + top + bottom `s colors)/numPixelsUsed
            newRed   =calculateColor(( (buffer)[i*width + j].r + left.r + right.r + top.r + bottom.r )/numPixelsUsed);
            newBlue  =calculateColor(( (buffer)[i*width + j].b + left.b + right.b + top.b + bottom.b )/numPixelsUsed);
            newGreen =calculateColor(( (buffer)[i*width + j].g + left.g + right.g + top.g + bottom.g )/numPixelsUsed);
            (*arrRef)[i*width + j].r = newRed;
            (*arrRef)[i*width + j].g = newGreen;
            (*arrRef)[i*width + j].b = newBlue;
            //reset numPixelsUsed for averaging the next pixel
            numPixelsUsed = 0;
        }//end s j loop
    }//ends i loop
}

void multiThreadedBoxBlurNoBuffer(struct Pixel** arrRef,  threadInfo info){
    int width = info.originalWidth;
    int height = info.originalHeight;
    int start = info.start;
    int end = info.end;
    //these will hold the average of each color for each 'box' of pixels
    unsigned char newBlue, newGreen, newRed;
    int numPixelsUsed = 0;
    Pixel top, bottom, right, left;
    //this will act as a "fresh copy" of the original pixels to read from so that we aren`t "re-blurring" them


    for(int i = 0; i < height; i++){
        for(int j = start; j < end; j++){
            //we don`t want to do the last pixel as we can`t add to the right of it
            //record the top pixel
            if(i < (height - 1)){
                top = (*arrRef)[(i + 1)*width + j];
                numPixelsUsed++;
            }else { //if not initialize it to an empty pixel so it doesnt effect calculations
                top.b=0;
                top.r=0;
                top.g=0;
            }
            //record the bottom pixel
            if(i < (height - 1) ){
                bottom =  (*arrRef)[(i - 1)*width + j];
            }else{ //if not initialize it to an empty pixel so it doesn`t effect calculations
                //dont need to increment numPixelUsed because were essentially adding nothing here
                bottom.b=0;
                bottom.r=0;
                bottom.g=0;
            }
            //record the left pixel
            if(j < end  && j < width - 1){
                numPixelsUsed++;
                right = (*arrRef)[i*width + (j + 1)];
            }else{ //if not initialize it to an empty pixel so it doesnt effect calculations
                right.b=0;
                right.r=0;
                right.g=0;
            }
            if(j > start  && j >0){
                left = (*arrRef)[i*width + (j - 1)];
                numPixelsUsed++;
            }
            else{ //if not initialize it to an empty pixel so it doesnt effect calculations
               left.b=0;
               left.r=0;
               left.g=0;
            }
            //average all pixel colors for this box:  (original pixel + left + right + top + bottom `s colors)/numPixelsUsed
            newRed = ((*arrRef)[i*width + j].r + left.r + right.r + top.r + bottom.r)/numPixelsUsed;
            newBlue = ((*arrRef)[i*width + j].b + left.b + right.b + top.b + bottom.b)/numPixelsUsed;
            newGreen = ((*arrRef)[i*width + j].g + left.g + right.g + top.g + bottom.g)/numPixelsUsed;
            (*arrRef)[i*width + j].r = newRed;
            (*arrRef)[i*width + j].g = newGreen;
            (*arrRef)[i*width + j].b = newBlue;
            //reset numPixelsUsed for averaging the next pixel
            numPixelsUsed = 0;
        }//end s j loop
    }//ends i loop
}


/*
 helper function to colorShiftPixels that checks edge cases < 0 & > 255
 * and decides what the final color will be.
 */
unsigned char calculateColor(int color){
    //check edge cases:
    if(color < 0){
        color = 0;
    }else if(color > 255){
        color = 255;
    }
    //return unsigned char version of the color
    return (unsigned char)color;
}//ends calculateColor
/*
 * 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
    Prints out an array of Pixels
 */

/**
 * Swaps the place of the red and blue pixel
 * BMP => (b,g,r)
 * PPM => (r,g,b)
 */
void PixelSwap(struct Pixel** arrRef, int width, int height){
    unsigned char temp;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
             temp = (*arrRef)[i * width + j].r;
             (*arrRef)[i * width + j].r = (*arrRef)[i * width + j].b;
             (*arrRef)[i * width + j].b = temp;
        }
    }

}//ends pixel swap



