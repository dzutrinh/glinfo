# glinfo [WIP]
Tiny OpenGL Information utility. It displays OpenGL version, vendor, renderer and GLSL version.
Currently support Windows and OSX, querying using legacy and core OpenGL profile. Linux support
is under BETA testing phase.

![linf](https://github.com/dzutrinh/glinfo/blob/master/screens/glinfo_linux.png)

## Requirements
* Windows: MinGW/Visual Studio 2015
* OSX: gcc 
* Linux: gcc

## Compile
* Just type 'make'

## To do
* Export results to a text file for later references

## Usage

`$glinfo [-hiecv]`

​	`-h	show help message`

​	`-v 	display utility version`

​	`-c	use core profile to query`

​	`-e	display OpenGL extensions only`

​	`-i	display basic OpenGL, use combine with -e to display both`

To force software OpenGL using Mesa on Linux, use the following command

`$LIBGL_ALWAYS_SOFTWARE=1 bin/glinfo`

