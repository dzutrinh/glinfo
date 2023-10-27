## OpenGL Information Query Utility
##
## Copyrights (c) 2021-2023 by Trinh D.D. Nguyen <dzutrinh[]yahoo.com>
## All Rights Reserved
## 
## Redistribution and use in source and binary forms, with or without 
## modification, are permitted provided that the following conditions are met:
## 
## * Redistributions of source code must retain the above copyright notice, 
##   this list of conditions and the following disclaimer.
## * Redistributions in binary form must reproduce the above copyright notice, 
##   this list of conditions and the following disclaimer in the documentation 
##   and/or other materials provided with the distribution.
## * The name of the author may be used to endorse or promote products 
##   derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
## LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
## CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
## THE POSSIBILITY OF SUCH DAMAGE.

# Common macros
CC		= gcc
BIN		= bin
SRC		= src
PROJECT	= glinfo
SOURCE	= $(SRC)/main.c $(SRC)/ogli.c
BINARY	= $(BIN)/$(PROJECT)

# YES = enable GLEW | NO = disable GLEW
GLEW	= NO

# Platform detection
ifeq ($(OS),Windows_NT)							# WINDOWS
	TARGET	= $(BINARY)-win32
	CFLAGS	= -Wall -O2 -o $(TARGET)
	LFLAGS	= -s -lgdi32 -lopengl32 -lglu32
	RM		= $(del /f)
	MD		= $(IF NOT  exist $(BIN) mkdir $(BIN))
	ifeq ($(GLEW), YES)
		CFLAGS += -DOGLI_USE_GLEW
		LFLAGS += -lglew32
	endif
else
	UNAME_S := $(shell uname -s)
	RM		= rm -f
	MD		= mkdir -p $(BIN)
	ifeq ($(UNAME_S),Linux)						# LINUX
		TARGET	= $(BINARY)-linux
		CFLAGS = -O2 -o $(TARGET)
		LFLAGS = -lGL -lGLU -lX11
		ifeq ($(GLEW), YES)
			CFLAGS += -DOGLI_USE_GLEW
			LFLAGS += -lGLEW
		endif	
	else
	ifeq ($(UNAME_S),Darwin)					# OSX
		TARGET	= $(BINARY)-darwin
		CFLAGS = -Wno-deprecated -Wno-enum-conversion -Wno-\#warnings -o $(TARGET)
		LFLAGS = -framework OpenGL
	endif
	endif
endif

# Build rules
all: $(SOURCE)
	$(MD)
	$(CC) $(CFLAGS) $(SOURCE) $(LFLAGS)

clean:
	$(RM) $(TARGET)
	
