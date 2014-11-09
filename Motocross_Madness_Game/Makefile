CC = g++
CFLAGS = -Wall
PROG = motocross

SRCS = Graphics_Assignment2.cpp vec3f.cpp imageloader.cpp
LIBS = -lglut -lGL -lGLU

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
