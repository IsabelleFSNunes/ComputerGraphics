INCPATH = -I../Include

LIBPATH = -L/usr/lib/x86_64-linux-gnu/
LDFLAGS = -lGL -lGLU -lGLEW -lglut

CFLAGS = -std=gnu++11 -DGL_GLEXT_PROTOTYPES
RM = rm

CC = g++ -g

all: $(PROJ)

main: main.o ogldev_util.o  Icosahedron.o Moldes.o Cuboide.o
	$(CC) $(LIBPATH) -o $@ $@.o ogldev_util.o $(LDFLAGS)

%.o: %.cpp
	$(CC) $(INCPATH) $(CFLAGS) -c $*.cpp

%.o: ../Common/%.cpp
	$(CC) $(INCPATH) $(CFLAGS) -c ../Common/$*.cpp

clean:
	$(RM) *~ *.o ; \
	echo Clean done

realclean:
	$(RM) *~ *.o ; \
	$(RM) main ; \
	echo Realclean done
