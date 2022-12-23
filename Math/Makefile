
# The source file where the main() function is

SOURCEMAIN = test.cc

# Library files

SRC = vector3.cc trfm3D.cc plane.cc line.cc bbox.cc intersect.cc bsphere.cc ../Misc/tools.cc ../Misc/constants.cc

# Don't change anything below
DEBUG = 1
LIBDIR = -L/usr/lib/nvidia-367/ -L/usr/lib/nvidia-375/
LIBS = -lm -lglut -lGLU -lGL -lGLEW
INCLUDE_DIR = -I../Misc
ifdef DEBUG
OPTFLAGS = -g
else
OPTFLAGS = -O2
endif

CCOPTIONS = -std=c++11 -Wall $(OPTFLAGS)
MEMBERS = $(SRC:.cc=.o)
EXEC  = $(basename $(notdir $(SOURCEMAIN)))

all: $(EXEC)

%.o : %.cc
	g++ -c -o $@ $(CCOPTIONS) $(INCLUDE_DIR) $< 

$(EXEC): $(JPEG_LIB) $(TARGET) $(MEMBERS) $(SOURCEMAIN)
	g++ $(CCOPTIONS) -o $(EXEC) $(SOURCEMAIN) $(MEMBERS) $(INCLUDE_DIR) $(LIBDIR) $(LIBS)


$(JPEG_LIB):
	(cd $(JPEG_LIBDIR); ./configure --enable-static --disable-shared)
	(cd $(JPEG_LIBDIR); make)
	mv $(JPEG_LIBDIR)/.libs/libjpeg.a $(JPEG_LIBDIR)

.PHONY : all clean jpeglib_clean

clean:
	find . -type f -name '*.o' | xargs rm -f
	rm -f $(EXEC)

jpeglib_clean:
	(cd $(JPEG_LIBDIR); make clean)
