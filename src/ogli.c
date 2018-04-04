/*
 *  OGLI.CPP
 *  Implementation for OpenGL information query library. Coded by Trinh D.D. Nguyen
 *  This library is a cross platform library, it runs on Windows (VC/MinGW), OSX and Linux.
 *  OGLI is released under MIT licensed, please see LICENSE for more information.
 */
#include <stdio.h>
#include <string.h>
#include "ogli.h"

/*----------------------------------------------------------------------------------------------------------*/
/*                              PORTIONS ARE FROM GLEXT.H AND WGLEXT.H                                      */
/*----------------------------------------------------------------------------------------------------------*/

#define GL_SHADING_LANGUAGE_VERSION  0x8B8C
#define GL_NUM_EXTENSIONS            0x821D

typedef const GLubyte *(APIENTRY *  PFNGLGETSTRINGIPROC) (GLenum name, GLuint index); 
PFNGLGETSTRINGIPROC glGetStringi = NULL;

#ifdef _WIN32
    typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList); 
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
#endif

#define _OGLI_DEBUG_    1

void ogliLog(const char * msg)
{
#ifdef _OGLI_DEBUG_
    fprintf(stderr, "%s\n", msg);
#endif
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                        PLATFORM INDEPENDENT API                                          */
/*----------------------------------------------------------------------------------------------------------*/

/* 
 * ogliGetProcAddress(): cross platform function pointer fetcher
 * Input: name of the function to fetch
 * Output: entry-point to the function
 */
#ifdef  _WIN32
#   define ogliGetProcAddress(name)  wglGetProcAddress((char *) name)
#elif   __APPLE__
void * NSGetProcAddress (const GLubyte *name)
{
    static const struct mach_header* image = NULL;
    NSSymbol symbol;
    char* symbolName;

    if (NULL == image)
        image = NSAddImage("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", NSADDIMAGE_OPTION_RETURN_ON_ERROR);

    symbolName = malloc(strlen((const char*)name) + 2);
    strcpy(symbolName+1, (const char*)name);
    symbolName[0] = '_';
    symbol = NULL;
    symbol = image ? NSLookupSymbolInImage(image, symbolName, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND | NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR) : NULL;
    free(symbolName);
    if(symbol)
        return NSAddressOfSymbol(symbol);
    return NULL;
}

#   define ogliGetProcAddress(name)  NSGetProcAddress(name)
#else
#   define ogliGetProcAddress(name)  glxGetProcAddress((char *) name)
#endif

/* 
 * ogliInitCore(): initialize the OpenGL for core profile 
 * Input : none
 * Output: GL_TRUE if success.
 */
static GLboolean ogliInitCore()
{
    glGetStringi = (PFNGLGETSTRINGIPROC) ogliGetProcAddress((GLubyte *) "glGetStringi");
    if (!glGetStringi)
    {
        ogliLog("Error getting glGetStringi()");
        return GL_FALSE;
    }

#ifdef  _WIN32
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) ogliGetProcAddress((GLubyte *) "wglCreateContextAttribsARB");
    if (!wglCreateContextAttribsARB)
    {
        ogliLog("Error getting wglCreateContextAttribsARB()");
        return GL_FALSE;
    }
#endif

    return GL_TRUE;
}

GLuint ogliGetVersion()
{
    return  (OGLI_MAJOR_VERSION << 8 | OGLI_MINOR_VERSION);
}

/* 
 * ogliInit(): initialize the OpenGL information query engine 
 * Input : profile to request
 * Output: a pointer to an allocated GL_INFO_CONTEXT structure, NULL if error occured.
 */
GL_INFO_CONTEXT * ogliInit(OGLI_PROFILE profile)
{
    GL_INFO_CONTEXT * ctx = (GL_INFO_CONTEXT *) malloc(sizeof(GL_INFO_CONTEXT));
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
#endif

#ifdef __APPLE__
    ctx->context = NULL;
    ctx->contextOrig = NULL;
#endif

#ifdef __GLX__
#endif

    ctx->active = GL_FALSE;
    memset(&ctx->iblock, 0, sizeof(GL_INFO_BLOCK));
    return ctx;
}

/* 
 * ogliShutdown(): shutdown the OpenGL information query engine 
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

/* 
 * ogliSupported(): check if an extension is supported 
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

/* 
 * ogliQuery(): query information from the OpenGL renderer
 * Input : pointer to the allocated GL_INFO_CONTEXT structure
 * Output: TRUE if suceed.
 */
GLboolean ogliQuery(GL_INFO_CONTEXT * ctx)
{
    char *  glsl, * ext, * tmp;
    GLint numExts, idx;

    if (!ctx)
        return GL_FALSE;

    if (!ctx->active)
        return GL_FALSE;
    
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
    if (ctx->profile == OGLI_LEGACY)
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
            //ogliLog((char *) glGetStringi(GL_EXTENSIONS, idx));
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

/*----------------------------------------------------------------------------------------------------------*/
/*                                     WIN32 PLATFORM SPECIFIC CODE                                         */
/*----------------------------------------------------------------------------------------------------------*/
#ifdef  _WIN32

/* class name and window's title */
static char * g_WNDCLASS = "LIBOGLI";

/* 
 * ogliCreateContext(): create an OpenGL rendering context using WGL API on Win32
 * Input : pointer to the allocated GL_INFO_CONTEXT structure
 * Output: TRUE if suceed.
 */
GLboolean ogliCreateContext(GL_INFO_CONTEXT * ctx)
{
    GLint                   pf;
    HGLRC                   rc3;
    WNDCLASS                wc;
    PIXELFORMATDESCRIPTOR   pfd;

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
            ogliLog("Error initialize core profile");
            return GL_FALSE;
        }

        rc3 = wglCreateContextAttribsARB(ctx->dc, 0, NULL);
        wglMakeCurrent(NULL, NULL);
		wglDeleteContext(ctx->rc);
        ctx->rc = rc3;
		wglMakeCurrent(ctx->dc, ctx->rc);
    }

    ctx->active = GL_TRUE;
    return GL_TRUE;
}

/* 
 * ogliDestroyContext(): destroy the created OpenGL rendering context
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

/*----------------------------------------------------------------------------------------------------------*/
/*                                      OSX PLATFORM SPECIFIC CODE                                          */
/*----------------------------------------------------------------------------------------------------------*/
#ifdef __APPLE__

/* 
 * ogliCreateContext(): create an OpenGL rendering context using WGL API on Win32
 * Input : pointer to the allocated GL_INFO_CONTEXT structure
 * Output: TRUE if suceed.
 */
GLboolean ogliCreateContext(GL_INFO_CONTEXT * ctx)
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
    if (profile == OGLI_CORE)
    {
        if (!ogliInitCore()))
            return GL_FALSE;
    }

    ctx->active = GL_TRUE;
        
    return GL_TRUE;
}

/* 
 * ogliDestroyContext(): destroy the created OpenGL rendering context
 * Input : pointer to the allocated GL_INFO_CONTEXT structure
 * Output: TRUE if suceed.
 */
GLboolean ogliDestroyContext(GL_INFO_CONTEXT * ctx)
{
    if (!ctx) 
        return GL_FALSE;
    
    CGLSetCurrentContext(ctx->contextOrig);
    if (ctx->context) 
        CGLReleaseContext(ctx->context);

    ctx->active = GL_FALSE;
    return GL_TRUE;
}

#endif