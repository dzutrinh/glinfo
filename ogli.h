#ifndef __GLINFO_H__
#define __GLINFO_H__

#include <stdlib.h>
#include <string.h>
#include <GL/glu.h>
/* #include <GL/glext.h> */     /* removed due to compatible problem */

#ifdef  _WIN32
# include <windows.h>
#endif

#ifdef _MSC_VER                 /* Microsoft Visual C++ compiler? */
# pragma comment (lib, "opengl32.lib")
# pragma comment (lib, "gdi32.lib")
#endif

#define MAX_INFO_LENGTH (128)   /* maximum length of an information string */
#define MAX_EXT_LENGTH  (10240) /* maximum length of an extension string */

#ifndef GL_SHADING_LANGUAGE_VERSION
# define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#endif

/* Shader Model enums */
typedef enum {SM_NONE, SM_20, SM_30, SM_40} shader_model;

/* OpenGL version block */
typedef struct gl_version_block
{
  GLint major;
  GLint minor;
  GLint release;
} GL_VERSION_BLOCK;

typedef struct glsl_version_block
{
  GLint major;
  GLint minor;
} GLSL_VERSION_BLOCK;

/* OpenGL information block */
typedef struct gl_info_block
{
  char  vendor[MAX_INFO_LENGTH];
  char  renderer[MAX_INFO_LENGTH];
  char  version[MAX_INFO_LENGTH];
  char  glsl[MAX_INFO_LENGTH];
  char  extensions[MAX_EXT_LENGTH];
  shader_model sm;
  GL_VERSION_BLOCK versionGL;
  GLSL_VERSION_BLOCK versionGLSL;
} GL_INFO_BLOCK;

/* Context for OpenGL information query */
typedef struct gl_info_context
{
  GLint requestMajor;   /* request minimum OpenGL version number */
  GLint requestMinor;
  GL_INFO_BLOCK iblock; /* OpenGL information block */
  GLboolean active;     /* ready for information query flag */

  /*--- platform specific attributes ---*/
#ifdef _WIN32
  HWND wnd;     /* window's handle */
  HDC dc;       /* device's context */
  HGLRC rc;     /* rendering context */
#endif
} GL_INFO_CONTEXT;

/*
  OpenGL information query pipeline:
    ogliInit() -> ogliCreateContext() -> ogliQuery() -+-> ogliSupported() -+ 
                                                      |                    |
                                                      +--------------------+
                                                                           |
                                  ogliShutdown() <- ogliDestroyContext() <-+
*/

GL_INFO_CONTEXT * ogliInit(int major, int minor);
GLboolean ogliShutdown(GL_INFO_CONTEXT * ctx);
GLboolean ogliCreateContext(GL_INFO_CONTEXT * ctx);
GLboolean ogliDestroyContext(GL_INFO_CONTEXT * ctx);
GLboolean ogliSupported(GL_INFO_CONTEXT * ctx, const char * extension);
GLboolean ogliQuery(GL_INFO_CONTEXT * ctx);

#endif
