# glinfo
Tiny OpenGL Information utility. It displays OpenGL version, vendor, renderer and GLSL version.
Currently support Windows and OSX, querying using legacy and core OpenGL profile. Linux support
is under testing phase.

![linf](https://github.com/dzutrinh/glinfo/blob/master/screens/glinfo_linux.png)

## Requirements
* Windows: MinGW/Visual Studio 2015
* OSX: GCC/Clang 
* Linux: GCC

## Compile

`$make`

The output binary would be placed under the newly created `./bin` folder. To run the utility, type:

`$./bin/glinfo`

## Usage

`$glinfo [-hiecv]`

​	`-h show help message`

​	`-v display utility version`

​	`-c use core profile to query`

​	`-e display OpenGL extensions only`

​	`-i display basic OpenGL, use combine with -e to display both`

To force software OpenGL using Mesa on Linux, use the following command

`$LIBGL_ALWAYS_SOFTWARE=1 bin/glinfo`

## To do
* Export results to a text file for later references
* Considering to remove optional GLEW usage from the library
