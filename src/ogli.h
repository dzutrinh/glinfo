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

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#	include <windows.h>
#   include <GL/gl.h>
#   include <GL/glu.h>
#else
#   ifdef   __APPLE__
#       include <OpenGL/OpenGL.h>
#       include <OpenGL/glu.h>
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
#	pragma warning (disable:4996)	/* enable preprocessor _CRT_SECURE_NO_WARNINGS */
#endif

#define MAX_INFO_LENGTH (128)		/* maximum length of an information string */
#define MAX_EXT_LENGTH  (10240)		/* maximum length of an extension string */

#define OGLI_MAJOR_VERSION  1       /* library version number */
#define OGLI_MINOR_VERSION  0

/* library platform */
#ifdef  _WIN32
#   ifdef _WIN64
#       define  OGLI_PLATFORM   ("OGLI-64")
#   else
#       define  OGLI_PLATFORM   ("OGLI-32")
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
} GL_VERSION_BLOCK;

/* GLSL version block */
typedef struct glsl_version_block
{
    GLint major;
    GLint minor;
} GLSL_VERSION_BLOCK;

/* OpenGL information block */
typedef struct gl_info_block
{
    char    glVendor[MAX_INFO_LENGTH];
    char    glRenderer[MAX_INFO_LENGTH];
    char    glVersion[MAX_INFO_LENGTH];
    char    glSL[MAX_INFO_LENGTH];
    char    glExtensions[MAX_EXT_LENGTH];
    char    gluVersion[MAX_INFO_LENGTH];
    char    gluExtensions[MAX_EXT_LENGTH];
    GLuint  totalExtensions;
    GL_VERSION_BLOCK    versionGL;
    GLSL_VERSION_BLOCK  versionGLSL;
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
} GL_INFO_CONTEXT;

/*
 * OpenGL information query pipeline:
 *   ogliInit() -> ogliCreateContext() -> ogliQuery() -+-> ogliSupported() -+ 
 *                                                     |                    |
 *                                                     +--------------------+
 *                                                                          |
 *                                 ogliShutdown() <- ogliDestroyContext() <-+
 */

GL_INFO_CONTEXT * ogliInit(OGLI_PROFILE profile);
GLboolean   ogliShutdown(GL_INFO_CONTEXT * ctx);
GLboolean   ogliCreateContext(GL_INFO_CONTEXT * ctx);
GLboolean   ogliDestroyContext(GL_INFO_CONTEXT * ctx);
GLboolean   ogliSupported(GL_INFO_CONTEXT * ctx, const char * extension);
GLboolean   ogliQuery(GL_INFO_CONTEXT * ctx);
GLuint      ogliGetVersion();

#ifdef __cplusplus
}
#endif 

#endif
