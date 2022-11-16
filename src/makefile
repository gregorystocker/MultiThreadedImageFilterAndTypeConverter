#The name of the makefile literally needs to be makeFile to easily recognize it so MYMakeFILE2 etc is NOT ALLOWED.

#This says target (what on the left of the colon is the target or the nickName for the compiled output)
#on the right side is the files we would include when compiling from command line

#objects is a variable Im defining to represent the list of object files I need in my program
#To access the variable, we wrap the var w/ $(<variable goes here>)

CFiles= StockerFilters.c BmpProcessor.c PpmProcessor.c PixelProcessor.c
CC=gcc
CFLAGS=-I.
DEPS = BmpProcessor.h PpmProcessor.h PixelProcessor.h
OBJ = StockerFilters.o BmpProcessor.o PpmProcessor.o PixelProcessor.o

# all calls the precondition (target) by default when we call make from the CL

#this line allows us to by default include the .c file w/ its own name. Example main.o will auto include main.c, but not main.h
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

target: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# compliling the individual object files` other dependencies

#tells the clean command to go through and remove all object files that we made


