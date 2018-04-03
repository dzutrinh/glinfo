# Common macros
CC=gcc
BIN=bin/
SRC=src/
PROJECT=glinfo
SOURCE=$(SRC)main.c $(SRC)ogli.c

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
		LFLAGS=-lGL -lX11
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
	$(RM) $(BIN)$(BINARY)
