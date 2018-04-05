/*
 *  OGLI.H
 *  Header for OpenGL information query library. Coded by Trinh D.D. Nguyen
 *  This library is a cross platform library, it runs on Windows (VC/MinGW), OSX and Linux.
 *  OGLI is released under MIT licensed, please see LICENSE for more information.
 */
#ifndef _OGLI_LIB_
#define _OGLI_LIB_    1

#ifdef __cplusplus
extern "C" {
#endif 

/* to make use of the GLEW library, enable the line below */
//#define OGLI_USE_GLEW 1

/* for debugging purpose, enable the line below */
//#define OGLI_DEBUG    1

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#	include <windows.h>
#   ifndef  OGLI_USE_GLEW
#       include <GL/gl.h>
#       include <GL/glu.h>
#   else
#       include <GL/glew.h>
#       include <GL/wglew.h>
#   endif
#else
#   ifdef   __APPLE__
#       ifndef  OGLI_USE_GLEW
#           include <OpenGL/OpenGL.h>
#           include <OpenGL/glu.h>
#       else
#           include <GL/glew.h>
#       endif
#       include <mach-o/dyld.h>
#   else
#       include <unistd.h>
#       include <X11/Xlib.h>
#       include <X11/Xutil.h>
#       include <GL/gl.h>
#       include <GL/glx.h>
#   endif
#endif

#ifdef _MSC_VER						/* Microsoft Visual C++ compiler */
#	pragma comment (lib, "opengl32.lib")
#	pragma comment (lib, "glu32.lib")
#	pragma comment (lib, "gdi32.lib")
#   ifdef   OGLI_USE_GLEW
#       pragma comment (lib, "glew32.lib")
#   endif
#	pragma warning (disable:4996)	/* enable preprocessor _CRT_SECURE_NO_WARNINGS */
#endif

#define OGLI_MAX_INFO_LENGTH (128)		/* maximum length of an information string */
#define OGLI_MAX_EXT_LENGTH  (10240)	/* maximum length of an extension string */

#define OGLI_MAJOR_VERSION  1           /* library version number */
#define OGLI_MINOR_VERSION  0

/* library platform */
#ifdef  _WIN32
#   ifdef _WIN64
#       define  OGLI_PLATFORM   ("OGLI-W64")
#   else
#       define  OGLI_PLATFORM   ("OGLI-W32")
#   endif
#elif   __APPLE__
#   define  OGLI_PLATFORM    ("OGLI-OSX")
#else
#   define  OGLI_PLATFORM    ("OGLI-LINUX")
#endif

/* OpenGL profile for querying information */
typedef enum {OGLI_LEGACY, OGLI_CORE} OGLI_PROFILE;

/* OpenGL version block */
typedef struct gl_version_block
{
    GLint major;
    GLint minor;
    GLint release;
} OGLI_GL_VERSION_BLOCK;

/* GLSL version block */
typedef struct glsl_version_block
{
    GLint major;
    GLint minor;
} OGLI_GLSL_VERSION_BLOCK;

/* OpenGL information block */
typedef struct gl_info_block
{
    char    glVendor[OGLI_MAX_INFO_LENGTH];
    char    glRenderer[OGLI_MAX_INFO_LENGTH];
    char    glVersion[OGLI_MAX_INFO_LENGTH];
    char    glSL[OGLI_MAX_INFO_LENGTH];
    char    glExtensions[OGLI_MAX_EXT_LENGTH];
    char    gluVersion[OGLI_MAX_INFO_LENGTH];
    char    gluExtensions[OGLI_MAX_EXT_LENGTH];
    GLuint  totalExtensions;
    OGLI_GL_VERSION_BLOCK    versionGL;
    OGLI_GLSL_VERSION_BLOCK  versionGLSL;
} GL_INFO_BLOCK;

/* Context for OpenGL information query */
typedef struct gl_info_context
{
    OGLI_PROFILE    profile;    /* query legacy or core profile */
    GL_INFO_BLOCK   iblock;     /* OpenGL information block */
    GLboolean       active;     /* ready for information query flag */

  /*--- platform specific attributes ---*/
#ifdef _WIN32
    HWND            wnd;        /* window's handle */
    HDC             dc;         /* device's context */
    HGLRC           rc;         /* rendering context */
#elif __APPLE__
    CGLContextObj   context;    /* context to use */
    CGLContextObj   contextOrig;/* original context */
#else
    /* POSIX */
#endif
} OGLI_CONTEXT;

/*
 * OpenGL information query pipeline:
 *   ogliInit() -> ogliCreateContext() -> ogliQuery() -+-> ogliSupported() -+ 
 *                                                     |                    |
 *                                                     +--------------------+
 *                                                                          |
 *                                 ogliShutdown() <- ogliDestroyContext() <-+
 */

OGLI_CONTEXT * ogliInit(OGLI_PROFILE profile);
GLboolean   ogliShutdown(OGLI_CONTEXT * ctx);
GLboolean   ogliCreateContext(OGLI_CONTEXT * ctx);
GLboolean   ogliDestroyContext(OGLI_CONTEXT * ctx);
GLboolean   ogliSupported(OGLI_CONTEXT * ctx, const char * extension);
GLboolean   ogliQuery(OGLI_CONTEXT * ctx);
GLuint      ogliGetVersion();

#ifdef __cplusplus
}
#endif 

#endif
