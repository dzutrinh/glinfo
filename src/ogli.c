/* OpenGL Information Query Library
**
** Copyrights (c) 2018 by Trinh D.D. Nguyen <dzutrinh[]yahoo.com>
** All Rights Reserved
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the following conditions are met:
** 
** * Redistributions of source code must retain the above copyright notice, 
**   this list of conditions and the following disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, 
**   this list of conditions and the following disclaimer in the documentation 
**   and/or other materials provided with the distribution.
** * The name of the author may be used to endorse or promote products 
**   derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
** THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <string.h>
#include "ogli.h"

#define GL_SHADING_LANGUAGE_VERSION  0x8B8C
#define GL_NUM_EXTENSIONS            0x821D

/*------------------------------------------------------------------------------------------------*/
/*                         PORTIONS ARE FROM GLEXT.H AND WGLEXT.H                                 */
/*------------------------------------------------------------------------------------------------*/
#ifndef __APPLE__
#   ifndef OGLI_USE_GLEW
#       ifndef APIENTRY
#           define APIENTRY
#       endif /* APIENTRY */
        typedef const GLubyte *(APIENTRY *  PFNGLGETSTRINGIPROC) (GLenum name, GLuint index); 
        PFNGLGETSTRINGIPROC glGetStringi = NULL;
#       ifdef _WIN32
            typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList); 
            PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
#       endif /* _WIN32 */
#   endif /* OGLI_USE_GLEW */
#endif /* __APPLE__ */

void ogliLog(const char * msg)
{
#ifdef OGLI_DEBUG
    fprintf(stderr, "%s\n", msg);
#endif
}

/*------------------------------------------------------------------------------------------------*/
/*                                   PLATFORM INDEPENDENT API                                     */
/*------------------------------------------------------------------------------------------------*/

#ifndef OGLI_USE_GLEW
#   ifdef  _WIN32
#       define ogliGetProcAddress(name)  wglGetProcAddress((char *) name)
#   elif __APPLE__
        /* since OSX has already initialized OpenGL extensions, there's nothing to do here */
#   else /* LINUX */
#       define ogliGetProcAddress(name)  glxGetProcAddress((char *) name)
#   endif /* LINUX */
#endif /* OGLI_USE_GLEW */

static GLboolean ogliInitCore()
{
#ifndef __APPLE__
#   ifndef OGLI_USE_GLEW
        glGetStringi = (PFNGLGETSTRINGIPROC) ogliGetProcAddress((GLubyte *) "glGetStringi");
        if (!glGetStringi)
        {
            ogliLog("Failed to obtain glGetStringi()");
            return GL_FALSE;
        }
        #ifdef  _WIN32
            wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) ogliGetProcAddress((GLubyte *) "wglCreateContextAttribsARB");
            if (!wglCreateContextAttribsARB)
            {
                ogliLog("Failed to obtain wglCreateContextAttribsARB()");
                return GL_FALSE;
            }
#       endif /* _WIN32 */
#   else
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK)
        {
            ogliLog("Failed to init GLEW library");
            return GL_FALSE;
        }
#   endif /* OGLI_USE_GLEW */
#endif  /* __APPLE__ */
    return GL_TRUE;
}

GLuint ogliGetVersion()
{
    return  (OGLI_MAJOR_VERSION << 8 | OGLI_MINOR_VERSION);
}

OGLI_CONTEXT * ogliInit(OGLI_PROFILE profile)
{
    OGLI_CONTEXT * ctx = (OGLI_CONTEXT *) malloc(sizeof(OGLI_CONTEXT));
    if (!ctx)
    {
        ogliLog("Not enough memory for context");
        return NULL;
    }

    ctx->profile = profile;

#ifdef _WIN32
    ctx->wnd = NULL;
    ctx->dc = NULL;
    ctx->rc = NULL;
#endif /* _WIN32 */

#ifdef __APPLE__
    ctx->context = NULL;
    ctx->contextOrig = NULL;
#endif /* __APPLE__ */

#ifdef __GLX__
#endif /* __GLX__ */

    ctx->active = GL_FALSE;
    memset(&ctx->iblock, 0, sizeof(GL_INFO_BLOCK));
    return ctx;
}

GLboolean ogliShutdown(OGLI_CONTEXT * ctx)
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

GLboolean ogliSupported(OGLI_CONTEXT * ctx, const char *extension)
{
    const char *start;
    const char *where, *terminator;

    if (!ctx)
    {
        ogliLog("Invalid OGLI context");
        return GL_FALSE;
    }
    
    if (!ctx->active)
    {
        ogliLog("OGLI is not ready");
        return GL_FALSE;
    }

    /* Extension names should not have spaces. */
    where = strchr(extension, ' ');
        if (where || *extension == '\0')
            return GL_FALSE;

    /*  It takes a bit of care to be fool-proof about parsing the
        OpenGL extensions string. Don't be fooled by sub-strings, etc. */
    for (start=ctx->iblock.glExtensions;;)
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

GLboolean ogliQuery(OGLI_CONTEXT * ctx)
{
    char *  glsl, * ext, * tmp;
    GLint numExts, idx;

    if (!ctx)
    {
        ogliLog("Invalid OGLI context");
        return GL_FALSE;
    }

    if (!ctx->active)
    {
        ogliLog("OGLI is not ready");
        return GL_FALSE;
    }
    
    /* reads the basic OpenGL information and store them into our information block */
    strcpy((char *) ctx->iblock.glRenderer,  (char *) glGetString(GL_RENDERER));
    strcpy((char *) ctx->iblock.glVendor,    (char *) glGetString(GL_VENDOR));
    strcpy((char *) ctx->iblock.glVersion,   (char *) glGetString(GL_VERSION));

    /* extracts the OpenGL version number */
    sscanf(ctx->iblock.glVersion, "%d.%d.%d", &ctx->iblock.versionGL.major, 
                                              &ctx->iblock.versionGL.minor,
                                              &ctx->iblock.versionGL.release);

    /* GLSL only present from OpenGL 2.0+ */
    if (ctx->iblock.versionGL.major >= 2)
    {
        /* double check :) */
        glsl = (char *) glGetString(GL_SHADING_LANGUAGE_VERSION); 
        if (!glsl)
            strcpy((char *) ctx->iblock.glSL, "None");
        else
        {
            strcpy((char *) ctx->iblock.glSL, glsl);
            /* extracts the GLSL version number */
            sscanf(ctx->iblock.glSL, "%d.%d", &ctx->iblock.versionGLSL.major, &ctx->iblock.versionGLSL.minor);
        }
    }
    else
        strcpy((char *) ctx->iblock.glSL, "None");

    /* stores the extensions list for later use */ 
#ifndef __APPLE__
    if (ctx->profile == OGLI_LEGACY ||                          /* use legacy profile */
        (ctx->profile == OGLI_CORE && glGetStringi == NULL))    /* or error while init core profile */
#else
    if (ctx->profile == OGLI_LEGACY)
#endif
    {                                                           
        /* copy the extensions string */
        ext = (char *) glGetString(GL_EXTENSIONS);
        if (ext)
            strcpy((char *) ctx->iblock.glExtensions, ext);
        else
            strcpy((char *) ctx->iblock.glExtensions, "");
        
        /* simplest way to count the number of extensions */
        for (tmp = ctx->iblock.glExtensions; *tmp; tmp++)
        {
            if (*tmp == ' ')
                ctx->iblock.totalExtensions++;
        }
    }
    else
    {
        /* form an extenion string just like the legacy one */
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExts);
        ctx->iblock.totalExtensions = numExts;
        for (idx = 0; idx < numExts; idx++) 
        {
            strcat(ctx->iblock.glExtensions, (char *) glGetStringi(GL_EXTENSIONS, idx));
            strcat(ctx->iblock.glExtensions, " ");
        }
    }
    
    /* OpenGL Utility Library */
    strcpy((char *) ctx->iblock.gluVersion,   (char *) gluGetString(GLU_VERSION));
    ext = (char *) gluGetString(GLU_EXTENSIONS);
    if (ext)
        strcpy((char *) ctx->iblock.gluExtensions, ext);
    else
        strcpy((char *) ctx->iblock.gluExtensions, "");

    return GL_TRUE; 
}

/*------------------------------------------------------------------------------------------------*/
/*                                WIN32 PLATFORM SPECIFIC CODE                                    */
/*------------------------------------------------------------------------------------------------*/
#ifdef  _WIN32

/* class name and window's title */
static char * g_WNDCLASS = "LIBOGLI";

GLboolean ogliCreateContext(OGLI_CONTEXT * ctx)
{
    GLint                   pf;
    WNDCLASS                wc;
    PIXELFORMATDESCRIPTOR   pfd;
#ifndef OGLI_USE_GLEW
    HGLRC                   rc3;
#endif /* OGLI_USE_GLEW */

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
    {
        ogliLog("OGLI library is not initialized");
        return GL_FALSE;
    }

    /* legacy profile */
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); /* preparing to obtain a pixel format */
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    pf = ChoosePixelFormat(ctx->dc, &pfd);          /* select a pixel format */
    if (!pf)
    {
        ogliLog("Cannot select pixel format");
        return GL_FALSE;
    }

    if (!SetPixelFormat(ctx->dc, pf, &pfd))
    {
        ogliLog("Pixel format is not accelerated");
        return GL_FALSE;
    }
    
    /* create rendering context */
    ctx->rc = wglCreateContext(ctx->dc);
    if (!ctx->rc)
    {
        ogliLog("Context creation error");
        return GL_FALSE;
    }

    /* make the rendering context current */
    if (!wglMakeCurrent(ctx->dc, ctx->rc))
    {
        ogliLog("Error making context current");
        return GL_FALSE;
    }

    if (ctx->profile == OGLI_CORE)
    {
        if (!ogliInitCore())
        {
            ogliLog("Error initialize core profile, switch back to legacy");
        }

#ifndef OGLI_USE_GLEW
        if (wglCreateContextAttribsARB != NULL)
        {
            rc3 = wglCreateContextAttribsARB(ctx->dc, 0, NULL);
            wglMakeCurrent(NULL, NULL);
		    wglDeleteContext(ctx->rc);
            ctx->rc = rc3;
		    wglMakeCurrent(ctx->dc, ctx->rc);
        }
#endif /* OGLI_USE_GLEW */
    }

    ctx->active = GL_TRUE;
    return GL_TRUE;
}

GLboolean ogliDestroyContext(OGLI_CONTEXT * ctx)
{
    if (!ctx)   /* validate input parameter */
    {
        ogliLog("Invalid OGLI context");
        return GL_FALSE;
    }

    if (!ctx->active)
    {
        ogliLog("OGLI is not ready");
        return GL_FALSE;
    }

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
#endif /* _WIN32 */

/*------------------------------------------------------------------------------------------------*/
/*                                 OSX PLATFORM SPECIFIC CODE                                     */
/*------------------------------------------------------------------------------------------------*/
#ifdef __APPLE__

GLboolean ogliCreateContext(OGLI_CONTEXT * ctx)
{
    CGLPixelFormatAttribute attribLegacy[] = {kCGLPFAOpenGLProfile, kCGLOGLPVersion_Legacy,   0};
    CGLPixelFormatAttribute attribCore[] =   {kCGLPFAOpenGLProfile, kCGLOGLPVersion_3_2_Core, 0};
    CGLError error;
    CGLPixelFormatObj pf;
    GLint npix;

    if (!ctx) 
        return GL_FALSE;
    
    if (ctx->profile == OGLI_CORE)
    {
        /* attempt to select core profile */
        error = CGLChoosePixelFormat(attribCore, &pf, &npix);
        if (error)
        {
            /* if core profile request failed, going back to legacy profile */
            error = CGLChoosePixelFormat(attribLegacy, &pf, &npix);
            if (error)
                return GL_FALSE;        /* still no? */
        }
    }
    else
        CGLChoosePixelFormat(attribLegacy, &pf, &npix);
        
    CGLCreateContext(pf, NULL, &ctx->context);
    ctx->contextOrig = CGLGetCurrentContext();
    CGLSetCurrentContext(ctx->context);        
    CGLReleasePixelFormat(pf);

    /* get glGetStringi entry point if core profile is requested */
    if (ctx->profile == OGLI_CORE)
    {
        if (!ogliInitCore())
            return GL_FALSE;
    }

    ctx->active = GL_TRUE;
        
    return GL_TRUE;
}

GLboolean ogliDestroyContext(OGLI_CONTEXT * ctx)
{
    if (!ctx) 
        return GL_FALSE;
    
    CGLSetCurrentContext(ctx->contextOrig);
    if (ctx->context) 
        CGLReleaseContext(ctx->context);

    ctx->active = GL_FALSE;
    return GL_TRUE;
}
#endif /* __APPLE__ */

/*------------------------------------------------------------------------------------------------*/
/*                                LINUX PLATFORM SPECIFIC CODE                                    */
/*------------------------------------------------------------------------------------------------*/
#ifdef __linux__

#endif
