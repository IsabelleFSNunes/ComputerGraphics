INCPATH = -I../Include -I/usr/local/include/ATB -I/opt/ImageMagick/include/ImageMagick-6/ -I../Common/FreetypeGL -I/usr/include/freetype2 -I/usr/include/assimp -I/usr/include
# INCPATH = -I../../Include/ -I../../Include/GL/ -I../../Include/ATB  -I../../Common/freetype2
LIBPATH = -L/usr/lib/x86_64-linux-gnu/ -L/opt/ImageMagick/lib
LDFLAGS = -lGL -lGLU -lGLEW -lglut -lAntTweakBar -lMagick++-6.Q16 -lMagickCore-6.Q16 -lMagickWand-6.Q16 -lfreetype -lm -lfontconfig -lassimp

CFLAGS = -std=gnu++11 -DGL_GLEXT_PROTOTYPES

RM = rm

CPP = g++ -g

CC = gcc -g

all: $(PROJ)

main: camera.o Cuboide.o main.o Models.o world_transform.o 
	$(CPP) $(LIBPATH) -o $@ $@.o camera.o Cuboide.o main.o Models.o  world_transform.o  $(LDFLAGS)

%.o: %.cpp
	$(CPP) $(INCPATH) $(CFLAGS) -c $*.cpp

%.o: ../Common/%.cpp
	$(CPP) $(INCPATH) $(CFLAGS) -c ../Common/$*.cpp

%.o: ../Common/FreetypeGL/%.cpp
	$(CPP) $(INCPATH) $(CFLAGS) -c ../Common/FreetypeGL/$*.cpp

%.o: ../Common/FreetypeGL/%.c
	$(CC) $(INCPATH) -c ../Common/FreetypeGL/$*.c

clean:
	$(RM) *~ *.o ; \
	echo Clean done

realclean:
	$(RM) *~ *.o ; \
	$(RM) main ; \
	echo Realclean done
