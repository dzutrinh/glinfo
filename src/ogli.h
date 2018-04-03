#ifndef __GLINFO_H__
#define __GLINFO_H__

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
#	pragma comment (lib, "gdi32.lib")
#	pragma warning (disable:4996)	/* enable preprocessor _CRT_SECURE_NO_WARNINGS */
#endif

#define MAX_INFO_LENGTH (128)		/* maximum length of an information string */
#define MAX_EXT_LENGTH  (10240)		/* maximum length of an extension string */

#ifndef GL_SHADING_LANGUAGE_VERSION
#   define GL_SHADING_LANGUAGE_VERSION  0x8B8C
#endif

#ifndef GL_NUM_EXTENSIONS
#   define GL_NUM_EXTENSIONS            0x821D
#endif

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
    OGLI_PROFILE profile; /* query legacy or core profile */
    GL_INFO_BLOCK iblock; /* OpenGL information block */
    GLboolean active;     /* ready for information query flag */

  /*--- platform specific attributes ---*/
#ifdef _WIN32
    HWND wnd;     /* window's handle */
    HDC dc;       /* device's context */
    HGLRC rc;     /* rendering context */
#elif __APPLE__
    CGLContextObj context, contextOrig;
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
GLboolean ogliShutdown(GL_INFO_CONTEXT * ctx);
GLboolean ogliCreateContext(GL_INFO_CONTEXT * ctx);
GLboolean ogliDestroyContext(GL_INFO_CONTEXT * ctx);
GLboolean ogliSupported(GL_INFO_CONTEXT * ctx, const char * extension);
GLboolean ogliQuery(GL_INFO_CONTEXT * ctx);

#ifdef __cplusplus
}
#endif 

#endif
