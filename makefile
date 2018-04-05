# Common macros
CC=gcc
BIN=bin/
SRC=src/
PROJECT=glinfo
SOURCE=$(SRC)main.c $(SRC)ogli.c

# YES = enable GLEW | NO = disable GLEW
GLEW=NO

# Platform detection
ifeq ($(OS),Windows_NT)							# WINDOWS
	BINARY=$(BIN)$(PROJECT).exe
	CFLAGS=-Wall -O2 -o $(BINARY)
	LFLAGS=-s -lgdi32 -lopengl32 -lglu32
	RM=del /f
else
	UNAME_S := $(shell uname -s)
	BINARY=$(BIN)$(PROJECT)
	RM=rm -f

	ifeq ($(UNAME_S),Linux)						# LINUX
		CFLAGS=-O2 -o $(BINARY)
		LFLAGS=-lGL -lGLU -lX11
	else
	ifeq ($(UNAME_S),Darwin)					# OSX
		CFLAGS=-Wno-deprecated -Wno-enum-conversion -o $(BINARY)
		LFLAGS=-framework OpenGL
	endif
	endif	
endif

ifeq ($(GLEW), YES)
	CFLAGS+=-DOGLI_USE_GLEW
	LFLAGS+=-lglew32
endif

# Build rules
all: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) $(LFLAGS)

setup:
	mkdir bin

clean:
	$(RM) $(BINARY)
