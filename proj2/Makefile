# compiler 
CC      = g++ 

# libraries we're linking against
LIBALIASES = -lGL -lGLU -lglut -lm -ljpeg

APPNAME = hw2

APPCODE = \
hw2.cxx \
raster.cxx \
raster-jpeg.cxx 

APPOBJS = $(APPCODE:.cxx=.o)

# silent running
.SILENT:

# make cxx into objects
%.o: %.cxx
	echo Compiling $< 
	$(CC) -c $<

################################
# build rules

all:
	echo
	make $(APPNAME)

# create program
$(APPNAME): $(APPOBJS)
	echo
	echo Building $(APPNAME) 
	$(CC) -Wall -o $(APPNAME) $(APPOBJS) $(LIBALIASES)
	echo Building Complete.

################################
# start a fresh build

clean:
	rm *.o $(APPNAME)
	echo Cleaned.
