
# The source file where the main() function is

SOURCEMAIN = Browser/browser.c

# Library files

SRC = Math/tools.c Math/trfm3D.c Math/plane.c Math/line.c Math/bbox.c\
      Geometry/glm.c Geometry/triangleMesh.c Geometry/geometryObject.c\
      Shading/light.c Shading/material.c Shading/texture.c Shading/textureManager.c Shading/materialManager.c Shading/lightManager.c\
      Camera/camera.c \
      SceneGraph/node.c SceneGraph/scene.c \
      Misc/list.c

# Don't change anything below
DEBUG = 1

INCLUDE_DIR = -I./Camera -I./Geometry  -I./Math -I./Misc -I./Shading -I./SceneGraph -I./gflsdk
LIBDIR = -L gflsdk/ 
LIBS = -lm -lglut -lGLU -lgfl -lgfle


ifdef DEBUG
OPTFLAGS = -g
else
OPTFLAGS = -O2
endif

CCOPTIONS = -Wall $(OPTFLAGS)
MEMBERS = $(SRC:.c=.o)
EXEC  = $(basename $(notdir $(SOURCEMAIN)))

all: $(EXEC)

%.o : %.c
	gcc -c -o $@ $(CCOPTIONS) $(INCLUDE_DIR) $< 

$(EXEC): $(TARGET) $(MEMBERS) $(SOURCEMAIN)
	gcc $(CCOPTIONS) -o $(EXEC) $(SOURCEMAIN) $(MEMBERS) $(INCLUDE_DIR) $(LIBDIR) $(LIBS)

.PHONY : all clean

clean:
	find . -type f -name '*.o' | xargs rm -f
	rm -f $(EXEC)

