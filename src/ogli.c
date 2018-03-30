#include <stdio.h>
#include <string.h>
#include "ogli.h"

/* ogliInit(): initialize the OpenGL information query engine 
 * Input : major, minor: request minimum OpenGL version (major.minor)
 * Output: a pointer to an allocated GL_INFO_CONTEXT structure, NULL if error occured.
*/
GL_INFO_CONTEXT * ogliInit(int major, int minor)
{
  GL_INFO_CONTEXT * ctx = (GL_INFO_CONTEXT *) malloc(sizeof(GL_INFO_CONTEXT));
  if (!ctx)
    return NULL;

  ctx->requestMajor = major;
  ctx->requestMinor = minor;
  ctx->wnd = NULL;
  ctx->dc = NULL;
  ctx->rc = NULL;
  ctx->active = GL_FALSE;
  memset(&ctx->iblock, 0, sizeof(GL_INFO_BLOCK));
  return ctx;
}

/* ogliShutdown(): shutdown the OpenGL information query engine 
 * Input : pointer to the allocated GL_INFO_CONTEXT structure
 * Output: TRUE if suceed.
*/
GLboolean ogliShutdown(GL_INFO_CONTEXT * ctx)
{
  if (!ctx)
    return GL_FALSE;
  
  if (ctx->active)
  {
    if (!ogliDestroyContext(ctx))
      return GL_FALSE;
  }

  free(ctx);
  return GL_TRUE;
}

/* ogliSupported(): check if an extension is supported 
 * Input : ctx: GL_INFO_CONTEXT structure.
 *         extension: extension to check. 
 * Output: TRUE if supported.
*/
GLboolean ogliSupported(GL_INFO_CONTEXT * ctx, const char *extension)
{
  const char *start;
  const char *where, *terminator;

  if (!ctx)
    return GL_FALSE;
    
  if (!ctx->active)
    return GL_FALSE;

  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return GL_FALSE;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  for (start=ctx->iblock.extensions;;)
  {
    where = strstr(start, extension);
    if (!where)
      break;
    terminator = where + strlen(extension);
    if ( where == start || *(where - 1) == ' ' )
      if ( *terminator == ' ' || *terminator == '\0' )
        return GL_TRUE;
    start = terminator;
  }
  
  return GL_FALSE;
}

/* ogliQuery(): query information from the OpenGL renderer
 * Input : pointer to the allocated GL_INFO_CONTEXT structure
 * Output: TRUE if suceed.
*/
GLboolean ogliQuery(GL_INFO_CONTEXT * ctx)
{
  char *  glsl;

  if (!ctx)
    return GL_FALSE;

  if (!ctx->active)
    return GL_FALSE;

  /* reads the basic OpenGL information and store them into our information block */
  strcpy((char *) ctx->iblock.renderer,  (char *) glGetString(GL_RENDERER));
  strcpy((char *) ctx->iblock.vendor,    (char *) glGetString(GL_VENDOR));
  strcpy((char *) ctx->iblock.version,   (char *) glGetString(GL_VERSION));

  /* extracts the OpenGL version number */
  sscanf(ctx->iblock.version, "%d.%d.%d", &ctx->iblock.versionGL.major, 
                                          &ctx->iblock.versionGL.minor,
                                          &ctx->iblock.versionGL.release);

  /* some old graphics card does not support this operation, so we better double check it */
  glsl = (char *) glGetString(GL_SHADING_LANGUAGE_VERSION); 
  if (!glsl)
    strcpy((char *) ctx->iblock.glsl, "None");
  else
  {
    strcpy((char *) ctx->iblock.glsl, glsl);

    /* extracts the GLSL version number */
    sscanf(ctx->iblock.glsl, "%d.%d", &ctx->iblock.versionGLSL.major, &ctx->iblock.versionGLSL.minor);
  }

  /* stores the extensions list for later use */ 
  strcpy((char *) ctx->iblock.extensions, (char *) glGetString(GL_EXTENSIONS));

  if (ogliSupported(ctx, "GL_NV_gpu_program4"))
    ctx->iblock.sm = SM_40;
  else
  if (ogliSupported(ctx, "GL_NV_gpu_program3"))
    ctx->iblock.sm = SM_30;
  else
  if (ogliSupported(ctx, "GL_ARB_fragment_program"))
    ctx->iblock.sm = SM_20;
  else
    ctx->iblock.sm = SM_NONE;

  return GL_TRUE; 
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                     WIN32 PLATFORM SPECIFIC CODE                                         */
/*----------------------------------------------------------------------------------------------------------*/
#ifdef  _WIN32

/* class name and window's title */
static char * g_WNDCLASS = "LIBOGLI";

/* ogliCreateContext(): create an OpenGL rendering context using WGL API on Win32
 * Input : pointer to the allocated GL_INFO_CONTEXT structure
 * Output: TRUE if suceed.
*/
GLboolean ogliCreateContext(GL_INFO_CONTEXT * ctx)
{
  GLint pf;
  WNDCLASS wc;
  PIXELFORMATDESCRIPTOR pfd;

  if (!ctx)    /* validate input parameter */
    return GL_FALSE;

  /* register our window class */
  memset(&wc, 0, sizeof(WNDCLASS));
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpfnWndProc = DefWindowProc;
  wc.lpszClassName = g_WNDCLASS;
  if (!RegisterClass(&wc))
    return GL_FALSE;

  ctx->wnd = CreateWindow(g_WNDCLASS, g_WNDCLASS, 0,  /* create our OpenGL rendering window */
                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
                          NULL, NULL, GetModuleHandle(NULL), NULL);
  if (!ctx->wnd)
    return GL_FALSE;

  ctx->dc = GetDC(ctx->wnd);      /* obtain device context */
  if (!ctx->dc) 
    return GL_FALSE;

  memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); /* preparing to obtain a pixel format */
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
  pf = ChoosePixelFormat(ctx->dc, &pfd);          /* select a pixel format */
  if (!pf)
    return GL_FALSE;

  if (!SetPixelFormat(ctx->dc, pf, &pfd))
    return GL_FALSE;
    
  /* create rendering context */
  ctx->rc = wglCreateContext(ctx->dc);
  if (!ctx->rc)
    return GL_FALSE;

  /* make the rendering context current */
  if (!wglMakeCurrent(ctx->dc, ctx->rc))
    return GL_FALSE;

  ctx->active = GL_TRUE;
  return GL_TRUE;
}

/* ogliDestroyContext(): destroy the created OpenGL rendering context
 * Input : pointer to the allocated GL_INFO_CONTEXT structure
 * Output: TRUE if suceed.
*/
GLboolean ogliDestroyContext(GL_INFO_CONTEXT * ctx)
{
  if (!ctx)   /* validate input parameter */
    return GL_FALSE;

  if (!ctx->active)
    return GL_FALSE;

  if (ctx->rc)    /* release the rendering context */
    wglMakeCurrent(NULL, NULL);

  if (ctx->rc)    /* and destroy it */
    wglDeleteContext(wglGetCurrentContext());

  if (ctx->wnd && ctx->dc)    /* release device context */
    ReleaseDC(ctx->wnd, ctx->dc);

  if (ctx->wnd)   /* destroy rendering window */
    DestroyWindow(ctx->wnd);
  
  /* and unregister window class from the system */
  UnregisterClass(g_WNDCLASS, GetModuleHandle(NULL));  

  ctx->active = GL_FALSE;
  return GL_TRUE;
}
#endif
