CC=gcc
CFLAGS=-Wno-deprecated -o glinfo -lglew -framework OpenGL -framework GLUT

all: main.c
	$(CC) $(CFLAGS) main.c glinfo.c

clean:
	rm -f main
