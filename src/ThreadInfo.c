//
// Created by greg on 9/11/21.
//
#include "ThreadInfo.h"

void printThreadInfo(threadInfo* info){
    printf("\nOriginalWidth:%d\tColWidth:%d\tOriginalHeight:%d\tstart:%d\tend:%d\tthreadNo:%d\ttid:%d\n",
    info->originalWidth, info->colWidth, info->originalHeight,info->start, info->end,info->threadNo,info->tid );
}

