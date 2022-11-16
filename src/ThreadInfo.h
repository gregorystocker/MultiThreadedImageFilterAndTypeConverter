//
// Created by greg on 9/11/21.
//

#ifndef BMPTHREADS_THREADINFO_H
#define BMPTHREADS_THREADINFO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

typedef struct Hole
{
    //info about the hole itself
    int x; //x coordinate of the center
    int y; //y coordinate of the center
    int r; //radius represented as an int

    //info about the square of memory the circle is inscribed in
    int bottomLeft;


}Hole;

typedef struct threadInfo{
    int originalWidth;
    int originalHeight;
    int colWidth;
    int start;
    int end;
    int top;
    int bottom;
    int threadNo;

    int smallestSize;
    int averageHoleSize;
    int numHoles;
    int holesIdx;
    Hole hole;
}threadInfo;

/**
 * Hoes for the cheese filter
 */



void printThreadInfo(threadInfo* info);
#endif //BMPTHREADS_THREADINFO_H
