#ifndef __GLINFO_H__
#define __GLINFO_H__

#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
#endif

#ifdef _MSC_VER
# pragma comment (lib, "opengl32.lib")
# pragma comment (lib, "glu32.lib")
# pragma comment (lib, "glew32.lib")
#endif

#define MAX_INFO_LENGTH (128)   /* maximum length of an information string */
#define MAX_EXT_LENGTH  (4096)  /* maximum length of an extension string */

/* Shader Model enums */
typedef enum {SM_NONE, SM_20, SM_30, SM_40} shader_model;

/* OpenGL information block */
typedef struct gl_info_block
{
  char  vendor[MAX_INFO_LENGTH];
  char  renderer[MAX_INFO_LENGTH];
  char  version[MAX_INFO_LENGTH];
  char  glsl[MAX_INFO_LENGTH];
  char  extensions[MAX_EXT_LENGTH];
  shader_model sm;
} GL_INFO_BLOCK;

/* Context for OpenGL information query */
typedef struct gl_info_context
{
  int requestMajor;
  int requestMinor;
  GL_INFO_BLOCK iblock;
} GL_INFO_CONTEXT;

GLboolean gliSupported(const char *extList, const char *extension);

GL_INFO_CONTEXT * gliInit(int major, int minor);
GLboolean gliShutdown(GL_INFO_CONTEXT ** ctx);

#endif
