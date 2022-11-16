
#include "PpmProcessor.h"

/**
 * read PPM header of a file. Useful for converting files from BMP to PPM.
 *	char magicNum[2];
	int width;
	int height;
	int maxval; //if maxval < 256: colors are 1 byte otherwise they`re 2
 * @param  file: A pointer to the file being read or written
 * @param  header: Pointer to the destination PPM header
 */

void readPPMHeader(FILE* file, struct PPM_Header* header){
    char width[30];
    char height[30];
    char maxval[30];
    fscanf(file,"\n%c",&(header->magicNum[0]));
    fscanf(file,"%c\n",&(header->magicNum[1]));
    fscanf(file,"%s\n",(width));
    fscanf(file,"%s\n",(height));
    fscanf(file,"%s\n",(maxval));
    header->width = atoi(width);
    header->height = atoi(height);
    header->maxval = atoi(maxval);
}
void printPPMHeader(struct PPM_Header* header){
    printf("\nPPM_HEADER INFORMATION:\n");
    printf("magicNum: %c%c\n", header->magicNum[0],header->magicNum[1]);
    printf("width :%d\n", header->width);
    printf("height:%d\n",header->width);
    printf("maxval: %d\n",header->maxval);

}

/**
 * write PPM header of a file. Useful for converting files from BMP to PPM.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: The header made by makePPMHeader function
 */
void writePPMHeader(FILE* file, struct PPM_Header* header){
   //getting string representations of the ints
    char width[30];
    sprintf(width, "%d",header->width);
    char height[30];
    sprintf(height, "%d",header->height);
    char maxval[30];
    sprintf(maxval, "%d",header->maxval);
    fprintf(file,"%c",header->magicNum[0]);
    fprintf(file,"%c\n",header->magicNum[1]);
    fprintf(file,"%s\n",width);
    fprintf(file,"%s\n",height);
    fprintf(file,"%s\n",maxval);
}


/**
 * read Pixels from PPM file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void readPixelsPPM(FILE* file, struct Pixel** pArr, int width, int height){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            fread(&((*pArr)[i*width + j].r), sizeof(unsigned char), 1, file);
            fread(&((*pArr)[i*width + j].g), sizeof(unsigned char), 1, file);
            fread(&((*pArr)[i*width + j].b), sizeof(unsigned char), 1, file);

        }//ends j
    }//ends i
}//ends readPixelsPPM




/**
 * write Pixels from PPM file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void writePixelsPPM(FILE* file, struct Pixel** pArr, int width, int height){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            fwrite(&((*pArr)[i*width + j].r), sizeof(unsigned char), 1, file);
            fwrite(&((*pArr)[i*width + j].g), sizeof(unsigned char), 1, file);
            fwrite(&((*pArr)[i*width + j].b), sizeof(unsigned char), 1, file);

        }//ends j
    }//ends i
}

/**
 * make PPM header based on width and height. Useful for converting files from BMP to PPM.
 *
 * @param  header: Pointer to the destination PPM header
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 * 	//TODO:Finish struct
	char magicNum[2];
	//whitespace
	int width;
	//whitespace
	int  height;
	int maxval; //if maxval < 256: colors are 1 byte otherwise they`re 2
 */
void makePPMHeader(struct PPM_Header* header, int width, int height){
    header->magicNum[0] = 'P'; //These never change for a magic number
    header->magicNum[1] = '6';
    header->width = width; //just copying straight over for width and height
    header->height = height;
    header->maxval = 255; //signifies the scope of one byte or a char
}
