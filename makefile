# Common macros
CC=gcc
PROJECT=glinfo
SOURCE=main.c ogli.c

# Platform detection
ifeq ($(OS),Windows_NT)							# WINDOWS
	BINARY=$(PROJECT).exe
	CFLAGS=-Wall -O2 -o $(BINARY)
	LFLAGS=-s -lgdi32 -lopengl32 -lglu32 
	RM=del /f
else
	UNAME_S := $(shell uname -s)
	BINARY=$(PROJECT)
	RM=rm -f

	ifeq ($(UNAME_S),Linux)						# LINUX
		CFLAGS=-O2 -o $(BINARY)
		LFLAGS=-lGL
	else
	ifeq ($(UNAME_S),Darwin)					# OSX
		CFLAGS=-Wno-deprecated -o $(BINARY)
		LFLAGS=-framework OpenGL 
	endif
	endif	
endif

# Build rules
all: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) $(LFLAGS)

clean:
	$(RM) $(BINARY)
