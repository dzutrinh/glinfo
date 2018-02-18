CC=g++
CFLAGS=-o glinfo -lglew -framework OpenGL -framework GLUT

all: main.cpp
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -f main
