# Common macros
CC=gcc
BIN=bin
SRC=src
PROJECT=glinfo
SOURCE=$(SRC)/main.c $(SRC)/ogli.c

# YES = enable GLEW | NO = disable GLEW
GLEW=NO

# Platform detection
ifeq ($(OS),Windows_NT)							# WINDOWS
	BINARY=$(BIN)/$(PROJECT).exe
	CFLAGS=-Wall -O2 -o $(BINARY)
	LFLAGS=-s -lgdi32 -lopengl32 -lglu32
	RM=del /f

	ifeq ($(GLEW), YES)
		CFLAGS+=-DOGLI_USE_GLEW
		LFLAGS+=-lglew32
	endif
	
	MD=if not exist $(BIN)/NUL mkdir $(BIN)

else
	UNAME_S := $(shell uname -s)
	BINARY=$(BIN)/$(PROJECT)
	RM=rm -f

	ifeq ($(UNAME_S),Linux)						# LINUX
		CFLAGS=-O2 -o $(BINARY)
		LFLAGS=-lGL -lGLU -lX11
		ifeq ($(GLEW), YES)
			CFLAGS+=-DOGLI_USE_GLEW
			LFLAGS+=-lglew
		endif	
	else
	ifeq ($(UNAME_S),Darwin)					# OSX
		CFLAGS=-Wno-deprecated -Wno-enum-conversion -Wno-\#warnings -o $(BINARY)
		LFLAGS=-framework OpenGL
	endif
	endif
	MD=mkdir -p $(BIN)
endif

# Build rules
all: $(SOURCE)
	$(MD)
	$(CC) $(CFLAGS) $(SOURCE) $(LFLAGS)

clean:
	$(RM) $(BINARY)
