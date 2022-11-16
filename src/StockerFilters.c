
/**
* Description:
 * Uses multi threading to apply a filter to an image. Ethier a 'blur' filter, or a 'cheese' filter.
*
Read in:
 input file name, output file name and image filter.
 These are NOT OPTIONAL
 Needs to support Command Line Input of the Form:
 ./StockerFilters -i input_file_name.bmp -o output_file_name -f b
 WHERE OPTIONS:
 -i = inputfile, -o = outputfile
 -f = filter => Possible values for the filter are 'b' for blur or 'c' for
 cheese filter

 THREADING INSTRUCTIONS:
 BLUR:
 Apply box blur to each pixel. Compute this in parallel w/ pthreads.
 Suggested is to divide by COLUMN. THREAD_COUNT is the amount of threads that the
 work should be split by. (This could be anything less than the images smallest dimension
 ex. 5X10 pixel matrix THREAD_COUNT will be less than 5)

 FOR EACH THREAD:
 -Allocate the memory neccesary for the intended thread`s column, then pass the
 allocated memory to the thread. DONT FORGET ABOUT THE PADDING, ALLOCATE MEMORY FOR THAT
 AS WELL. (The calculation for padding is probably the same as programming 3 as we have
 3 byte pixels and round off to %4 again.)
 SWISS CHEESE FILTER:
 * Apply the swiss cheese filter to the data. Compute it in parallel with pthreads.
 * Each threads only knows about its own assigned holes.
 * Each thread must have its own memory allocation of a minimal size.

 * #include <pthread.h>
*Compile With:
 * gcc StockerFilters.c PixelProcessor.c BmpProcessor.c PpmProcessor.c -lpthread -o main
*Run with:
 * FOR YELLOW TINT:
* ./main ttt.bmp -b -255 -o YellowISHPic.bmp
*
*
* Completion time:
* @author (Greg Stocker), (anyone elses name who's code you used... ie Acuna),
 *(Gagne, Galvin, Silberschatz)
* @version 1(start on 8/24/2021)
*
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDES
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>//Gives more detailed error messages
#include <pthread.h>

//Including modules
#include "ThreadInfo.h"
#include "BmpProcessor.h"
#include "PixelProcessor.h"
#include "PpmProcessor.h"

///////////////////THREAD COUNT MACRO/////////////////////////////////////////////////////////
#define THREAD_COUNT  12
///////////////////THREAD COUNT MACRO/////////////////////////////////////////////////////////

//GLOBAL VARIABLES
//color shift values
int rVal = 0;
int gVal = 0;
int bVal = 0;



Hole* holes;

//optional type argument for specifying the type of the output file
unsigned char filterType = 'x'; //takes in the value of the -f option. Can only be 'c' for cheese or 'b' for blur.
char outputType[4]; //only holds BMP or PPM and the nullchar terminator
int outFileGiven;
Pixel *pixels;


/**
 * Holds meta information about the input and output files
 * name->filename
 * type->b maps to bmp, p maps to ppm
 */
typedef struct FileInfo {
    char type;
    BMP_Header bmp;
    DIB_Header dib;
    PPM_Header ppm;
    char *name;
    FILE *file;
} FileInfo;


/**
 * Takes in a void* which could be anything
 * @param arg
 */

//FILEINFO GLOBAL STRUCTS===============================================================================================
FileInfo in;
FileInfo out;
//Forward Declarations for self defined helper functions to organize main
void printHole(threadInfo* info);
void printThreadInfo(threadInfo* info);
void multiThreadedCheesify(struct Pixel** pArr,threadInfo holesInfo);
void printThreadInfo(threadInfo* info);
void setupMultiThreadedCheeseFilter(struct Pixel** pArr, int width, int height);
void initializeSquare(threadInfo* holesInfo);
void initializeHole(threadInfo* holeInfo, int width, int height);
void colorYellow(struct Pixel** pArr,int width, int height);
void runner(int val);
void* colorIn(void * arg);
void* applyFilter(void * arg); //applies the filter using a single thread
void applyMultiThreadedFilter(int width, int height); // creates & associated info  for the threads and calls applyFilter on each
void fourThreadStripe(int width, int height);//Splits a Pic into 4 separate threads and assigns a different
//color to each that then apply it to the pic concurrently
void freeFileInfos(FileInfo *fi);//frees fileInfo structs
void readInInputFile(char *argv[]); //reads file name and type to fileInfo in
void readInOutputFile(char *argv[]); //reads file name and type to fileInfo out
void readInInputHeaders(); //reads in the proper input  header based on the type
void writeOutputHeaders();//reads in the proper output header based on the type
void readFileErrnoCheck(FileInfo finfo);// FOR DEBUGGING WHEN READING IN INPUT FILE
void readInInputPixels(Pixel **arrRef, int width, int height); //allocates space for and reads in the Pixel array
void getInPixelsWidthAndHeight(int *width, int *height); //reads in the height and width abstracts away the type
void writePixels(Pixel **arrRef, int *width, int *height); //prints out the pixel array in 2D format
void copyInputHeaderToOutput(); //copies in.<header> to out.<header> for BMP->BMP or PPM->PPM
void prepareOutputFileInfoAndFlip(int width, int height, Pixel** pArr); //will copy over the in file to the out if the same, or will call the conversion functions if different
void printFileInputs();//Prints out the FileInputStructs
void makeLowerCase(char** string, int length);
void applyDefaultsAndOptionToOutput(); //handles the -t option and assigns a default of the inputName.bmp if nothings entered.
//////////////////////START MAIN FUNCTION///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
    ////READ IN INPUT AND OUTPUT FILE NAME AND TYPES TO FILEINFO STRUCTS, AND COLORS SHIFT VARIABLES ======/////////////
    //INITIALIZE FILEINFO STRUCTS
    rVal = 0;//initialize so they dont start with garbage
    gVal = 0;
    bVal = 0;
    outFileGiven = 0;

    readInInputFile(argv); ////encapsulates the reading of the input file for readability works for both formats
    int opt; //stores the option char vale "not the actual argument"
    while ((opt = getopt(argc, argv,
                         "r:g:b:o:t:f: ")) !=
           -1) {                                     //if a char is followed by a : the option requires an argument
        //:: doesn`t actually work like it says in the docs
        //getopt returns -1 when its done
        //optarg is updated with the next argument after seeing a -
        char* temp;
        switch (opt) {
            case 'o': //SETS THE OUTPUT FILE
                readInOutputFile(argv); ////encapsulates the reading of the output file for readability
                outFileGiven = 1;
                break;
            case 't': //ARGUMENT TO GET THE TYPE FOR THE OUTPUT FILE
                 temp = (char*)malloc(sizeof(char)*(strlen(optarg) + 1));
                 strcpy(temp,optarg);
                 makeLowerCase(&temp,strlen(temp));
                 printf("\ntemp = %s\n",temp);
                 if(strcmp(temp, "bmp") == 0 || strcmp(temp, "ppm") == 0){
                     printf("\nValid option for -t\n");
                     strcpy(outputType, temp);
                 }else{
                     strcpy(outputType, "000"); //signals that there was nothing input for -t or it was invalid
                 }
                 free(temp);
                 printf("\nOutputType is now: %s", outputType);
                 temp = NULL;
            break;
            case 'f':
                filterType = *(optarg);
                printf("\nThe filterType is: %c",filterType);
                if(filterType == 'b'){
                    printf("\nFilter chosen: box blur\n");
                }else if(filterType == 'c'){
                printf("\nFilter chosen: cheese filter\n");
                }else{
                    printf("\nInvalid Filter chosen: Ignoring filter...\n");
                }
                break;
            case 'b':
                bVal = atoi(optarg);//atoi converts a char* into an int
                printf("\nblueVal: %d\n", bVal);
                break;
            case 'g':
                gVal = atoi(optarg);
                printf("\ngreenVal: %d\n", gVal);
                break;
            case 'r':
                rVal = atoi(optarg);
                printf("\nredVal: %d\n", rVal);
                break;
        }//ends switch
    }//ends while getopt()
    ////////////////////READ IN INPUT HEADERS///////////////////////////////////////////////////////////////////////////
    in.file = fopen(in.name, "rb"); //found that this works even for the ASCII parts
    readInInputHeaders(); //works for bmp and ppm
    //////////////////////READ IN INPUT PIXELS//////////////////////////////////////////////////////////////////////////
    //reading in the pixel array
    int width, height; //stores for either BMP or PPM
    getInPixelsWidthAndHeight(&width, &height); //works for bmp and ppm

    readInInputPixels(&pixels, width, height); //allocates the Pixels array and works for bmp and ppm

    /////////////////////////////COLOR SHIFT/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //singleCheeseFilter(&pixels,width,height);
    applyMultiThreadedFilter(width,height);
    //boxBlur(&pixels,width,height);
    //fourThreadStripe(width, height);
    //colorShiftPixels(&pixels,width,height, rVal, gVal, bVal); //color shifts if values were given, otherwise just returns
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    fclose(in.file);
////APPLY DEFAULTS IF USER DOESNT ENTER A NAME FOR -o OR THEY CHANGE THE TYPE WITH -t///////////////////////////////////
    applyDefaultsAndOptionToOutput();
    ////////////////////////WRITING TO OUTPUT FILES/////////////////////////////////////////////////////////////////////
    out.file = fopen(out.name, "wb"); //works for either
    prepareOutputFileInfoAndFlip(width,height,&pixels); //will copy over the in file headers to the out if the same,
    // or will call the correct conversion function. If converting, also calls the flipPixels function
    writeOutputHeaders(); //writes the correct headers to the output file
    printFileInputs();
    writePixels(&pixels, &width, &height); //works for bmp
    fclose(out.file);
/////////////////////////////////////FREE UP DYNAMICALLY ALLOCATED HEAP MEMORY//////////////////////////////////////////
    freeFileInfos(&in);
    freeFileInfos(&out);
    free(pixels);
}///////////////////////ENDS MAIN///////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////FUNCTION DESCRIPTIONS/////////////////////////////////////////////////////////////////////
/**
 * Frees up the fileInfo Structs
 */
void freeFileInfos(FileInfo *fi) {
    printf("\n %s is type: %c\n", fi->name, fi->type);
    free((fi)->name);
}
/**
 * Encapsulates the process of reading in the input file just to make the main
 * cleaner and easier to debug. Allocating memory for the name, and storing it`s attributes
 * in the global struct FileInfo in.
 * @param argv copy of mains argv
 */
void readInInputFile(char *argv[]) {
    //allocate memory for the char* name
    in.name = (char *) malloc(sizeof(char) * FILENAME_MAX);
    int inFileLength;//length of the input file
    inFileLength = strlen(argv[optind]);
    strcpy(in.name,
           argv[optind]); //optind gets the index of the first non option argument after the program name from argv[]
    if ((inFileLength >= 5) && (strcmp(&(in.name)[inFileLength - 4], ".bmp") == 0)) {
        printf("\nInput file`s name: %s\n", (in.name));
        in.type = 'b';
    } else if ((inFileLength >= 5) && (strcmp(&(in.name)[inFileLength - 4], ".ppm") == 0)) {
        in.type = 'p';
        //strcpy(file_output, optarg); //optarg stores the next unknown word in argv
        printf("\nInput file`s name: %s\n", (in.name));
    }
}//ends readInInputFile
/**
 * Identical to readInInputFile but for the output file
 * @param argv copy of mains argv
 */
void readInOutputFile(char *argv[]) {
    //allocate memory for the char* name
    out.name = (char *) malloc(sizeof(char) * FILENAME_MAX);
    int optvalue = optind;
    int outFileLength = strlen(optarg);//length of the input fileoutFileLength = strlen(argv[optind]);
    printf("\noptarg = %s\n", optarg);
    strcpy(out.name,
           optarg); //optind gets the index of the first non option argument after the program name from argv[]
    if ((outFileLength >= 5) && (strcmp(&(out.name)[outFileLength - 4], ".bmp") == 0)) {
        printf("\nOutput file`s name: %s\n", (out.name));
        out.type = 'b';
    } else if ((outFileLength >= 5) && (strcmp(&(out.name)[outFileLength - 4], ".ppm") == 0)) {
        out.type = 'p';
        //strcpy(file_output, optarg); //optarg stores the next unknown word in argv
        printf("\nOutput file`s name: %s\n", (out.name));
    }
}//ends readInOutFile
/**
 * Encapsulates reading in both bmp and ppm headers
 * @param file
 */
void readInInputHeaders() {
    readFileErrnoCheck(in); //CHECK FOR ERRORS RELATED TO READING IN THE FILE
    if (in.type == 'b') {
        readBMPHeader(in.file, &(in.bmp));
        readDIBHeader(in.file, &(in.dib));
        printBMPHeader(in.file, &(in.bmp));
        printDIBHeader(in.file, &(in.dib));
    } else {
        readPPMHeader(in.file, &(in.ppm));
        printPPMHeader(&(in.ppm));
    }
}//ends readInInputHeaders
/**
 * Encapsulates writing out both bmp and ppm headers to the output file
 *
 */
void writeOutputHeaders() {
    readFileErrnoCheck(out);
    if( out.type == 'b') {
        writeBMPHeader(out.file, &(out.bmp));
        writeDIBHeader(out.file, &(out.dib));
    } else if (out.type == 'p') {
        writePPMHeader(out.file, &(out.ppm));
    }
}//ends readInInputHeaders

/**
 * Prints out the error number given by errno.h along with the current working directory
 * for troubleshooting file not found errors that correspond to errno = 2
 * @param finfo The struct that holds the file and meta information about the file
 */
void readFileErrnoCheck(FileInfo finfo) {
    FILE *tf = fopen(finfo.name, "rb");
    if (tf == NULL) {
        printf("\nERROR: file was not opened\n");
        printf("\nValue of errno: %d\n", errno);
        char *buf;
        buf = (char *) malloc(100 * sizeof(char));
        getcwd(buf, 100);
        printf("\nCurrent Working Directory: %s \n", buf);
        free(buf);
    }
    if (tf != NULL)
        fclose(tf);
}

void readInInputPixels(Pixel **arrRef, int width, int height) {
    //Allocating space for the pixels
    if (in.type == 'b') {
        *arrRef = malloc(sizeof(Pixel) * in.dib.width * in.dib.height);
    } else if (in.type == 'p') {
        *arrRef = malloc(sizeof(Pixel) * in.ppm.width * in.ppm.height);
    }
    printf("\nreading in pixels from file: %s\n", in.name);
    if (in.type == 'b') {
        //this offsets to the position of the pixel array by offsetting the amount stored
        // in the bmpheader from the beginning of the file
        fseek(in.file, sizeof(char) * (in.bmp.offset), SEEK_SET);
        readPixelsBMP(in.file, arrRef, width, height);
    } else {
        readPixelsPPM(in.file, arrRef, width, height);
    }
    //printPixels(arrRef, width, height);
}

/**
 * Abstracts away the file type and gets the right height and width
 * @param height gets height from the correct struct
 * @param width same for width
 */
void getInPixelsWidthAndHeight(int *width, int *height) {
    if (in.type == 'b') {
        *height = in.dib.height;
        *width = in.dib.width;
    } else {
        //PPM width and height are stored as chars, so we need to convert back to integer
        *height = in.ppm.height;
        *width = in.ppm.width;
    }
    printf("\nGetting pixel width: %d and height: %d file: %s\n", *width, *height, in.name);
}

/**
 * Write out pixels to file
 * @param arrRef
 * @param width
 * @param height
 */
void writePixels(Pixel **arrRef, int *width, int *height) {
    if (out.type == 'b') {
        writePixelsBMP(out.file, arrRef, *width, *height);
    } else {
        writePixelsPPM(out.file, arrRef, *width, *height);
    }
}

void printFileInputs() {
    printf("\nFileInfo for in:\n");
    printf("in name:%s\n", in.name);
    if (in.type == 'b') {
        printf("in type: BMP\n");
        printBMPHeader(in.file, &in.bmp);
    } else if (in.type == 'p') {
        printf("in type: PPM\n");
        printPPMHeader(&in.ppm);
    }
    if (in.file != NULL) {
        printf("IN HAS VALID FILE\n");
    } else {
        printf("IN FILE IS NULL\n");
    }
    printf("\n------------------\n");
    printf("\nFileInfo for out:\n");
    printf("out name:%s\n", out.name);
    if (out.type == 'b') {
        printf("out type: BMP\n");
        printBMPHeader(out.file, &out.bmp);
    } else if (out.type == 'p') {
        printf("out type: PPM\n");
        printPPMHeader(&out.ppm);
    }
    if (in.file != NULL) {
        printf("OUT HAS VALID FILE\n");
    } else {
        printf("OUT FILE IS NULL\n");
    }
}

/**
 * Copies Header from in to out if BMP or PPM
 *
 */
void copyInputHeaderToOutput() {
    //case: in.type doesnt match out.type
    if (in.type != out.type) {
        printf("\nTypes need to be the same to copy over\n");
    }
    if (in.type == 'b') {
        //copy over BMPHeader
        out.bmp.signature[0] = in.bmp.signature[0];
        out.bmp.signature[1] = in.bmp.signature[1];
        out.bmp.size = in.bmp.size;
        out.bmp.reserved1 = in.bmp.reserved1;
        out.bmp.reserved2 = in.bmp.reserved2;
        out.bmp.offset = in.bmp.offset;
        //copy over DIBHeader
        out.dib.size = in.dib.size;
        out.dib.width = in.dib.width;
        out.dib.height = in.dib.height;
        out.dib.planes = in.dib.planes;
        out.dib.bpp = in.dib.bpp;
        out.dib.compression = in.dib.compression;
        out.dib.imageSize = in.dib.imageSize;
        out.dib.xppm = in.dib.xppm;
        out.dib.yppm = in.dib.yppm;
        out.dib.colors = in.dib.colors;
        out.dib.icc = in.dib.icc;
    } else {
        //copy over PPMHeader
        out.ppm.magicNum[0] = in.ppm.magicNum[0];
        out.ppm.magicNum[1] = in.ppm.magicNum[1];
        out.ppm.width = in.ppm.width;
        out.ppm.height = in.ppm.height;
        out.ppm.maxval = in.ppm.maxval;
    }
}//end function

/**
 * will copy over the in file to the out if the same, or will call the conversion functions if different
 */
void prepareOutputFileInfoAndFlip(int width, int height, Pixel** pArr){
    //if they have the same type just copy straight over
    if(in.type == out.type){
        copyInputHeaderToOutput();
    }else if(in.type == 'b' && out.type == 'p'){
        makePPMHeader(&(out.ppm), width, height); //call the 'make' function if different
        flipPixels(pArr,width,height);
    }else if(in.type == 'p' && out.type == 'b'){
        makeBMPHeader(&(out.bmp), width, height); //call the 'make' function if different
        makeDIBHeader(&(out.dib), width, height); //call the 'make' function if different
        flipPixels(pArr,width,height);
    }else{
        printf("\nInvalid file format combination, cant prepareOutputFile\n");
    }
}//ends prepareOutputFile

/**
 * Makes a string lowercase. For use with the -t option so that BMP BmP and bmp will be treated the same.
 * @param string
 * @param length
 */
void makeLowerCase(char** string, int length){
    for(int i = 0; i < length; i++){
        if((*string)[i] >= 65  &&  (*string)[i]<= 90){ //if the char is uppercase
            (*string)[i] += 32;
        }
    }
}

/**
 *  Handles the -t option and assigns a default of the <inputFileName>.bmp if nothings entered.
 */
void applyDefaultsAndOptionToOutput(){

    //Handles the case if no output file is given:
    //copy the input file back to the output file
    if(outFileGiven == 0){
        out.name = (char *) malloc(sizeof(char) * FILENAME_MAX);
        strcpy(out.name,in.name);
        out.type = in.type;
    }
    //If the -t option was bmp OR no option was given AND no outputFile was given,
    //the default is to convert the file to a bmp
    if(strcmp(outputType,"bmp") == 0 || (strcmp(outputType,"000") == 0&& !outFileGiven)){
        int fileLength = strlen(out.name);
        if ((fileLength >= 5) && (strcmp(&(out.name)[fileLength - 4], ".bmp")) != 0){ //if the file isnt a bmp
            out.name[fileLength - 4] = '.';
            out.name[fileLength - 3] = 'b';
            out.name[fileLength - 2] = 'm';
            out.name[fileLength - 1] = 'p';
        }
        //handles the option that the use input ppm for the -t option
    }else if(strcmp(outputType,"ppm") == 0){
        int fileLength = strlen(out.name);
        if ((fileLength >= 5) && (strcmp(&(out.name)[fileLength - 4], ".ppm")) != 0){ //if the file isnt a bmp
            out.name[fileLength - 4] = '.';
            out.name[fileLength - 3] = 'p';
            out.name[fileLength - 2] = 'p';
            out.name[fileLength - 1] = 'm';
        }
        out.type = 'p';
    }
}

void* colorIn(void * arg){
    printf("\nHello from colorIn!\n");
    threadInfo* tInfo = (threadInfo*)arg;
    //printThreadInfo(&(*info));
    ///////////////////////////////////////ends here////////////////////////////////////////////////////////////////
    printf("\nWont print this message\n");
    if (tInfo->threadNo %10 == 0) {
        colorShiftPixelsOffset(&pixels, 15, -230, -15, *tInfo);
    }
    if (tInfo->threadNo %9 == 0) {
        colorShiftPixelsOffset(&pixels, 0, -255, 0, *tInfo);
    }
    else if (tInfo->threadNo %8 == 0) {
        colorShiftPixelsOffset(&pixels, -15, -230, 15, *tInfo);
    }
    else if (tInfo->threadNo %7 == 0) {
        colorShiftPixelsOffset(&pixels, -45, -220, 45, *tInfo);
    }
    else if(tInfo->threadNo %6 == 0) {
        colorShiftPixelsOffset(&pixels, -70, -200, 70, *tInfo);
    }
    else if(tInfo->threadNo %5 == 0) {
        colorShiftPixelsOffset(&pixels, -100, -150, 100, *tInfo);
    }
    else if(tInfo->threadNo %4 == 0) {
        colorShiftPixelsOffset(&pixels, -150, -100, 150, *tInfo);
    }else if(tInfo->threadNo %3 == 0) {
        colorShiftPixelsOffset(&pixels, -200, -50, 200, *tInfo);
    }else if(tInfo->threadNo %2 == 0) {
        colorShiftPixelsOffset(&pixels, -250, 0, 255, *tInfo);
    }else{
        colorShiftPixelsOffset(&pixels, -200, 15, 200, *tInfo);
    }
    ///////////////////////////////////////doesnt print this////////////////////////////////////////////////////////////
    pthread_exit(0);
}//ends colorIn



/**
 * Stripes the picture in 4 different columns with 4 different threads!
 */
void fourThreadStripe(int width, int height) {
    //initialize array of pthreads and pthread info to go along with it
    const int howMany = 4;
    // need to stay as parallel arrays
    pthread_t workers[howMany];
    pthread_attr_t attributes[howMany]; //attributes (meta data like memory size for thread)
    threadInfo workerInfo[howMany];
    int startCounter = 0;
    //Initialize the thread info structs with default values.
    for (int i = 0; i < howMany; i++) {
        //workerInfo[i] = (threadInfo *) malloc(sizeof(threadInfo));
        workerInfo[i].originalWidth = width;
        workerInfo[i].originalHeight = height;
        workerInfo[i].start = startCounter;
        workerInfo[i].colWidth = width / howMany;
        workerInfo[i].end = workerInfo[i].start + workerInfo[i].colWidth;
        workerInfo[i].threadNo = i;
        startCounter += workerInfo[i].colWidth; //keeps track of where each thread starts in the picture
    }
    //Initialize the worker[] pThreads and their attributes, call the function and wrap it up!
    for (int i = 0; i < howMany; i++) {
        pthread_attr_init(&attributes[i]); //sets up out thread with a default attribute list
        pthread_create(&workers[i], &attributes[i], colorIn,
                       &workerInfo[i]); //passing in null attribute list for now
    }//ends for
    for (int i = 0; i < howMany; i++) {
        pthread_join(workers[i], NULL);
    }//ends fourThreadStripe
}

void* applyFilter(void * arg){
    //printf("\nHello from applyFilter!\n");
    threadInfo* tInfo = (threadInfo*)arg;
    if(filterType == 'b'){
        multiThreadedBoxBlur(&pixels,*tInfo);////////////////////////////////////////////////////////////////////////////////////////////
        //colorShiftPixelsOffset(struct Pixel** pArr,int rShift, int gShift, int bShift, threadInfo info);
        //colorIn( tInfo);
    }else if(filterType == 'c'){
        multiThreadedCheesify(&pixels, *tInfo);
    }
    pthread_exit(0);
}//ends colorIn

/**
 * Applies a filter chosen by the user with the number of threads also chosen by the user
 * @param width - original images width
 * @param height - original images height
 * @param howMany - # of threads
 * @param filterType - 'c' for cheeseFilter and 'b' for blur filter
 */
void applyMultiThreadedFilter(int width, int height) {
    //initialize array of pthreads and pthread info to go along with it
    // need to stay as parallel arrays
    pthread_t workers[THREAD_COUNT];
    pthread_attr_t attributes[THREAD_COUNT]; //attributes (meta data like memory size for thread)
    threadInfo workerInfo[THREAD_COUNT];
    /////////////////////////////////////////NEED TO ADD HOLESINFO!!!!!!!///////////////////////////////////////////////////////////////////////////////////////////////////////////
    int startCounter = 0;
    srand(time(NULL));
    //Initialize the thread info structs with default values.
    for (int i = 0; i < THREAD_COUNT; i++) {////SETUP FILTERTYPE = b
        if (filterType == 'b') {
            workerInfo[i].originalWidth = width;
            workerInfo[i].originalHeight = height;
            workerInfo[i].start = startCounter;
            workerInfo[i].colWidth = width / THREAD_COUNT;
            workerInfo[i].end = workerInfo[i].start + workerInfo[i].colWidth;
            workerInfo[i].threadNo = i;
            startCounter += workerInfo[i].colWidth; //keeps track of where each thread starts in the picture
        }//ENDS FILTERTYPE = b

            ////SETUP FILTERTYPE = C
        else if (filterType == 'c') {

            workerInfo[i].originalWidth = width;
            workerInfo[i].originalHeight = height;

            ////initialize hole
            //sets the hole related attributes of the threadInfo like x,y and radius

            //The number of holes should also be 10% of the
            //smallest side (e.g., if an image is 130 pixels, then there will be 13 holes
            if (width < height) {
                workerInfo[i].smallestSize = width;
            } else {
                workerInfo[i].smallestSize = height;
            }
            workerInfo[i].numHoles = workerInfo[i].smallestSize / 10;
            int averageHoleSize = workerInfo[i].smallestSize / 10;
            //initialize holes
            workerInfo[i].hole.r = calculateRadius(averageHoleSize);
            workerInfo[i].hole.x = calculateCoordinate(width);
            workerInfo[i].hole.y = calculateCoordinate(height);
            //sets the variables related to the square of memory that the thread focuses on.

            ////initialize square//////////////////////
            //initializeSquare(&(workerInfo[i]));
            workerInfo[i].top = workerInfo[i].hole.y + workerInfo[i].hole.r;
            workerInfo[i].start = workerInfo[i].hole.x - workerInfo[i].hole.r;
            workerInfo[i].end = workerInfo[i].hole.x + workerInfo[i].hole.r;
            workerInfo[i].bottom = workerInfo[i].hole.y - workerInfo[i].hole.r;
            //printHole(&workerInfo[i]);
        }//ends filterType == 'c')
    }//ends i loop


        ////APPLY YELLOW FILTER
        if (filterType == 'c') {
            colorYellow(&pixels, width, height);
        }
        //Initialize the worker[] pThreads and their attributes, call the function and wrap it up!
        for (int i = 0; i < THREAD_COUNT; i++) {
            pthread_attr_init(&attributes[i]); //sets up out thread with a default attribute list


            pthread_create(&workers[i], &attributes[i], applyFilter,
                           &workerInfo[i]); //passing in null attribute list for now

        }//ends for
        for (int i = 0; i < THREAD_COUNT; i++) {
            pthread_join(workers[i], NULL);
        }

}

void multiThreadedCheesify(struct Pixel** pArr, threadInfo holesInfo){
    //Add HOLES
    int top =holesInfo.originalHeight;
    int bottom = 0;
    int start = 0;
    int end =  holesInfo.originalHeight;
    int width = holesInfo.originalWidth;
    int height = holesInfo.originalHeight;

    printHole(&holesInfo);

    //loop through all the pixels
    for(int i = top; i >= bottom; i--){
        for(int j = start; j <= end; j++){
            //loop through all Holes
                //check if the distance between the test point and the hole`s center is within the radius of the circle
                int distanceToCenter = distance((float)holesInfo.hole.x,(float)holesInfo.hole.y,(float)i,(float)j);
                if(distanceToCenter <= holesInfo.hole.r){
                    //color the pixel black
                        (*pArr)[i * width + j].r = 0;
                        (*pArr)[i * width + j].b = 0;
                        (*pArr)[i * width + j].g = 0;
            }
        }//ends j
    }//ends i
}//ends cheeseFilter

void printHole(threadInfo* info){
printf(
       "\nHole:\n"
       "(%d,%d) radius: %d\n",
       info->hole.x, info->hole.y,  info->hole.r);

    printf("Square:\n"
           "top:%d, bottom:%d, start:%d, end:%d\n",
           info->top,info->bottom,info->start,info->end );
}
void printThreadInfo(threadInfo* info){
    // int smallestSize;
    // int averageHoleSize;
    // int numHoles;
    // int holesIdx;
    // Hole hole;

    printf("\nOriginalWidth:%d\tColWidth:%d\tOriginalHeight:%d\tstart:%d\tend:%d\tthreadNo:%d\n"
           "smallestSize:%d\taverageHoleSize:%d\tholesIdx:%d\t\n"
           "Hole:\n"
           "(%d,%d) radius: %d",
           info->originalWidth, info->colWidth, info->originalHeight,info->start, info->end,info->threadNo,
           info->smallestSize, info->averageHoleSize, info->holesIdx,
           info->hole.x, info->hole.y,  info->hole.r);
}

/**
 * sets the four corners of the threads memory top left = (top,start)    (top, end)
 * @param holesInfo                                       (bottom,start) (bottom,end)
 * @param width
 * @param height
 */
void initializeSquare(threadInfo* holesInfo){
    holesInfo->top = holesInfo->hole.y + holesInfo->hole.r;
    holesInfo->start = holesInfo->hole.x - holesInfo->hole.r;
    holesInfo->end = holesInfo->hole.x + holesInfo->hole.r;
    holesInfo->bottom = holesInfo->hole.y - holesInfo->hole.r;
}//ends square

void initializeHole(threadInfo* holeInfo, int width, int height){
    srand(time(NULL));
    //The number of holes should also be 10% of the
    //smallest side (e.g., if an image is 130 pixels, then there will be 13 holes
    if(width < height){
        holeInfo->smallestSize = width;
    }else{
        holeInfo->smallestSize = height;
    }
    holeInfo->numHoles = holeInfo->smallestSize/10;
    int averageHoleSize = holeInfo->smallestSize/10;
    //initialize holes
    holeInfo->hole.r = calculateRadius(averageHoleSize);
    holeInfo->hole.x = calculateCoordinate(width);
    holeInfo->hole.y = calculateCoordinate(height);
}//ends initializeHole

void colorYellow(struct Pixel** pArr,int width, int height){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            (*pArr)[i*width + j].b = calculateColor( (*pArr)[i*width + j].b - 50);
            (*pArr)[i*width + j].r = calculateColor((*pArr)[i*width + j].r + 50);
            (*pArr)[i*width + j].g = calculateColor( (*pArr)[i*width + j].g + 50);
            //(*pArr)[i*width + j].b -=50;
        }//ends j loop
    }//ends i loop
    //PICTURE IS YELLOW
}
