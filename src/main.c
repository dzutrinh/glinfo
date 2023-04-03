/* OpenGL Information Query Utility
**
** Copyrights (c) 2021-2023 by Trinh D.D. Nguyen <dzutrinh[]yahoo.com>
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

/* CHANGES
**
** v1.0: initial development
** v1.1: Linux support added
** v1.2: improving extensions listing 
** v1.3: fix buffer overflow bugs when query OpenGL information
**
*/

#include <stdio.h>
#include "ogli.h"

#define GLINFO_MAJOR_VERSION    1
#define GLINFO_MINOR_VERSION    3

const char * HELP_MSG = "OpenGL information query utility - v%d.%d (%s)\n"
                        "Coded by Trinh D.D. Nguyen\n\n"
                        "Usage: glinfo [-hiecv]\n"
                        "Where as: -h, --help       give this help message\n"
                        "          -c, --core       query by core profile, default is legacy profile\n"
                        "          -e, --extension  list all extensions only\n"
                        "          -i, --info       display OpenGL information, use combine with -e\n"
                        "          -v, --version    display program's version number\n";

GLboolean       extShow = GL_FALSE;
GLboolean       infShow = GL_TRUE;
GLboolean       valid = GL_FALSE;
OGLI_PROFILE    profile = OGLI_LEGACY;

void die(const char * msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(-1);
}

void showVersions()
{
    printf("OpenGL Information Query v%d.%d\n"
           "Coded by Trinh D.D. Nguyen\n", GLINFO_MAJOR_VERSION,
                                           GLINFO_MINOR_VERSION);
    printf("Information query engine v%d.%d (%s)\n", OGLI_MAJOR_VERSION,
                                                      OGLI_MINOR_VERSION, 
                                                      OGLI_PLATFORM);
#ifdef  OGLI_USE_GLEW
    printf("GLEW library v%s\n", glewGetString(GLEW_VERSION));
#endif
}

GLboolean parseArgs(int argc, char ** argv)
{
    GLint       idx;

    if (argc > 1)
    {
        valid = GL_FALSE;
        for(idx = 0; idx < argc; idx++)
        {
            if (strcmp(argv[idx], "-h") == 0 || strcmp(argv[idx], "--help") == 0)
            {
                printf(HELP_MSG, GLINFO_MAJOR_VERSION, GLINFO_MINOR_VERSION, OGLI_PLATFORM);
                return GL_FALSE;
            }

            if (strcmp(argv[idx], "-v") == 0 || strcmp(argv[idx], "--version") == 0)
            {
                showVersions();
                return GL_FALSE;
            }

            if (strcmp(argv[idx], "-c") == 0 || strcmp(argv[idx], "--core") == 0)
            {
                profile = OGLI_CORE;
                valid = GL_TRUE;
            }

            if (strcmp(argv[idx], "-e") == 0 || strcmp(argv[idx], "--extension") == 0)
            {
                extShow = GL_TRUE;
                infShow = GL_FALSE;
                valid = GL_TRUE;
            }

            if (strcmp(argv[idx], "-i") == 0 || strcmp(argv[idx], "--info") == 0)
            {
                infShow = GL_TRUE;
                valid = GL_TRUE;
            }
        }
        if (!valid)
            die("Unknown command line option.");
    }
    return GL_TRUE;
}

void showInfo(const OGLI_CONTEXT * ctx)
{
    GLubyte *token, *temp;
    GLuint  extLen, count = 0;

    printf(">>> OpenGL\n");
    if (infShow)
    {
        printf(" . Vendor      : %s\n", ctx->iblock.glVendor);
        printf(" . Renderer    : %s\n", ctx->iblock.glRenderer);
        printf(" . Version     : %s\n", ctx->iblock.glVersion);
        printf(" . GLSL        : %s\n", ctx->iblock.glSL);
        printf(" . Extensions  : %u total\n", ctx->iblock.totalExtensions);
    }

    if (extShow)
    {
        extLen = strlen(ctx->iblock.glExtensions);
        temp = (GLubyte *) malloc(extLen); 
        strcpy((char *) temp, ctx->iblock.glExtensions);
        token = (GLubyte *) strtok((char *) temp, " ");
        while (token)
        {
            printf("%4d %s\n", ++count, token);
            token = (GLubyte *) strtok(NULL, " ");
        }
        free(temp);
    }

    printf("\n>>> OpenGLU\n");
    if (infShow)
        printf(" . Version     : %s\n", ctx->iblock.gluVersion);

    if (extShow)
    {
        count = 0;
        extLen = strlen(ctx->iblock.gluExtensions);
        temp = (GLubyte *) malloc(extLen); 
        strcpy((char *) temp, ctx->iblock.gluExtensions);
        token = (GLubyte *) strtok((char *) temp, " ");
        while (token)
        {
            printf("%4d %s\n", ++count, token);
            token = (GLubyte *) strtok(NULL, " ");
        }
        free(temp);
    }
}

int main(int argc, char **argv)
{
    OGLI_CONTEXT        * ctx = NULL;

    if (!parseArgs(argc, argv))     return 0;
    ctx = ogliInit(profile);
    if (!ctx)                       die("Cannot init OGLI library.");
    if (!ogliCreateContext(ctx))    die("Error creating OpenGL context.");
    if (!ogliQuery(ctx))            die("Error fetching OpenGL information.");
    if (!ogliDestroyContext(ctx))   die("Error destroying rendering OpenGL context.");
    showInfo(ctx);
    ogliShutdown(ctx);

    return  0;
}
